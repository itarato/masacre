#pragma once

#include <list>
#include <memory>

#include "asset_manager.h"
#include "common.h"
#include "game_scope.h"
#include "map.h"
#include "particles.h"
#include "path_finder.h"
#include "raylib.h"
#include "zapper.h"

constexpr float ENEMY_SPEED = 200.f;
constexpr float ENEMY_TARGET_REACH_THRESHOLD = 1.f;
constexpr float ENEMY_EXPLOSION_SPEED = 200.f;
constexpr float ENEMY_PLAYER_MIN_CHASE_DISTANCE = 100.f;
constexpr float ENEMY_SPAWNER_MAX_HEALTH = 800.f;

struct Enemy : AttackDamage {
  u_int64_t object_id;
  Vector2 pos;
  Vector2 move_target{};
  float circle_frame_radius{};
  float angle{};
  bool is_dead{false};
  TimedTask dying_lifetime{2.0};
  RepeatedTask shooting_task{2.0, 2.0};
  ParticleManager particle_manager{};
  float barrel_angle_rad{};
  float collision_avoidance_slowdown{1.f};
  RepeatedTask smoke_particle_scheduler{0.1};
  float health{30.f};

  explicit Enemy(Vector2 _pos) : pos(_pos), move_target(_pos) {
    // The frame is derived from the image which is designed for turning.
    // Once the turning works, let's reduce the wheel size so we can use it to assume a frame size.
    circle_frame_radius = asset_manager.textures[ASSET_ENEMY_WHEEL_TEXTURE].width / 3.f;

    object_id = global_object_id++;
  }

  void update(Vector2 const &player_pos, Map const &map, PathFinder const &path_finder) {
    if (!is_dead) {
      if (Vector2Distance(pos, move_target) <= ENEMY_TARGET_REACH_THRESHOLD) {
        update_move_target(player_pos, map, path_finder);
      } else {
        update_movement_towards_target(player_pos);
      }

      barrel_angle_rad = abs_angle_of_points(pos, player_pos);

      // Shoot the player.
      if (shooting_task.did_tick) {
        float aim_jitter_rad = ((rand() % 31) - 15) * DEG2RAD;
        Vector2 bullet_v{cosf(barrel_angle_rad + aim_jitter_rad) * BULLET_SPEED * GetFrameTime(),
                         sinf(barrel_angle_rad + aim_jitter_rad) * BULLET_SPEED * GetFrameTime()};
        game_scope.enemy_bullets.emplace_back(pos, bullet_v, BULLET_SINGLE_ATTACK_DAMAGE);

        PlaySound(asset_manager.sounds[ASSET_SOUND_ENEMY_SHOOT]);
      }
    } else {
      smoke_particle_scheduler.update();
      if (smoke_particle_scheduler.did_tick) particle_manager.particles.push_back(std::make_unique<SmokeParticle>(pos));
    }

    particle_manager.update();
    shooting_task.update();
  }

  void draw(Map const &map, Vector2 const &player_pos) const {
    if (is_dead) {
      draw_texture(asset_manager.textures[ASSET_ENEMY_BROKEN_TEXTURE], Vector2Add(pos, map.world_offset), angle);
    } else {
      draw_texture(asset_manager.textures[ASSET_ENEMY_WHEEL_TEXTURE], Vector2Add(pos, map.world_offset), angle);
      draw_texture(asset_manager.textures[ASSET_ENEMY_BARREL_TEXTURE], Vector2Add(pos, map.world_offset),
                   barrel_angle_rad * RAD2DEG);
    }

    particle_manager.draw(map);
  }

  void hurt(AttackDamage const &damager) {
    if (is_dead) return;

    health -= damager.get_attack_damage();

    if (health <= 0.f) {
      health = 0.f;
      is_dead = true;
      dying_lifetime.reset();
      make_explosion(particle_manager, pos, ENEMY_EXPLOSION_SPEED, 32, GOLD);
    }
  }

  [[nodiscard]] bool should_be_deleted() const {
    return is_dead && dying_lifetime.is_completed();
  }

  [[nodiscard]] float get_attack_damage() const override {
    if (is_dead) return 0.f;

    return 10.f * GetFrameTime();
  }

 private:
  void update_move_target(Vector2 const &player_pos, Map const &map, PathFinder const &path_finder) {
    if (Vector2Distance(pos, player_pos) <= ENEMY_PLAYER_MIN_CHASE_DISTANCE) return;

    auto path = path_finder.find_path(pos, player_pos);
    if (path.empty()) {
      TraceLog(LOG_INFO, "No path from enemy to player");
      return;
    }

    // Likely the enemy is already at the zone of player.
    if (path.size() <= 1) return;

    move_target = Vector2(path[path.size() - 2].x * CELL_DISTANCE, path[path.size() - 2].y * CELL_DISTANCE);
  }

  void update_movement_towards_target(Vector2 const &player_pos) {
    if (Vector2Distance(pos, player_pos) <= ENEMY_PLAYER_MIN_CHASE_DISTANCE) return;

    Vector2 delta = Vector2Subtract(pos, move_target);
    float total_dist = Vector2Distance(pos, move_target);
    float move_dist = ENEMY_SPEED * GetFrameTime() * collision_avoidance_slowdown;

    Vector2 old_pos{pos};

    if (move_dist >= total_dist) {
      pos = move_target;
    } else {
      Vector2 delta_adjusted_to_move_dist = Vector2Divide(delta, {total_dist / move_dist, total_dist / move_dist});
      pos = Vector2Subtract(pos, delta_adjusted_to_move_dist);
    }

    angle = abs_angle_of_points(old_pos, pos) * RAD2DEG;
  }
};

struct EnemySpawner {
  Vector2 pos;
  std::shared_ptr<SharedMusic> zapper_music;
  RepeatedTask spawn_repeater{5.0};
  float circle_frame_radius{30.f};
  float health{ENEMY_SPAWNER_MAX_HEALTH};
  RepeatedTask smoke_repeater{1.f};
  ParticleManager particle_manager{};
  Zapper zapper{};

  EnemySpawner(Vector2 _pos, std::shared_ptr<SharedMusic> _zapper_music)
      : pos(_pos), zapper_music(std::move(_zapper_music)) {
    smoke_repeater.pause();
  }

  void update(std::list<Enemy> &enemies, Player &player) {
    const bool player_too_close = Vector2Distance(pos, *player.pos) <= 256.f;
    zapper.visible = player_too_close;
    zapper.start = pos;
    zapper.end = *player.pos;

    if (!is_dead()) {
      if (spawn_repeater.update()) enemies.emplace_back(pos);

      if (player_too_close) {
        zapper_music->request();
        player.hurt(zapper);
      }
    }

    particle_manager.update();
    if (smoke_repeater.update()) {
      particle_manager.particles.push_back(std::make_unique<SmokeParticle>(pos, circle_frame_radius));
    }
  }

  void draw(Map const &map) const {
    if (!is_dead()) {
      zapper.draw(map);
    }

    draw_texture(asset_manager.textures[ASSET_ENEMY_SPAWNER_TEXTURE], Vector2Add(pos, map.world_offset), 0.f);
    DrawRectangle(pos.x - circle_frame_radius + map.world_offset.x - 2.f,
                  pos.y - circle_frame_radius - 8 + map.world_offset.y - 2.f, (circle_frame_radius * 2) + 4.f,
                  8.f + 4.f, DARKGRAY);
    DrawRectangle(pos.x - circle_frame_radius + map.world_offset.x,
                  pos.y - circle_frame_radius - 8 + map.world_offset.y,
                  (circle_frame_radius * 2) * health / ENEMY_SPAWNER_MAX_HEALTH, 8.f, RED);

    particle_manager.draw(map);
  }

  void hurt(AttackDamage const &damager) {
    health -= damager.get_attack_damage();
    if (health <= 0.f) {
      health = 0.f;
      smoke_repeater.pause();
      make_explosion(particle_manager, pos, ENEMY_EXPLOSION_SPEED, 32, GOLD);
    } else {
      smoke_repeater.resume();
      smoke_repeater.set_interval((health / ENEMY_SPAWNER_MAX_HEALTH) * 0.3f + 0.01f);
    }
  }

  bool is_dead() const {
    return health <= 0.f;
  }
};
