#pragma once

#include <memory>

#include "asset_manager.h"
#include "common.h"
#include "game_scope.h"
#include "map.h"
#include "particles.h"
#include "raylib.h"

#define ENEMY_SPEED 200.f
#define ENEMY_TARGET_REACH_THRESHOLD 1.f
#define ENEMY_EXPLOSION_SPEED 200.f

struct Enemy {
  Vector2 pos;
  Vector2 move_target{};
  float circle_frame_radius{};
  float angle{};
  bool is_dead{false};
  TimedTask dying_lifetime{2.0};
  RepeatedTask shooting_task{2.0, 2.0};
  ParticleManager particle_manager{};
  float barrel_angle_rad{};

  Enemy(Vector2 _pos) : pos(_pos), move_target(_pos) {
    // The frame is derived from the image which is designed for turning.
    // Once the turning works, let's reduce the wheel size so we can use it to assume a frame size.
    circle_frame_radius = asset_manager.textures[ASSET_ENEMY_WHEEL_TEXTURE].width / 3.f;
  }

  void update(Vector2 const &player_pos, Map const &map) {
    if (!is_dead) {
      if (Vector2Distance(pos, move_target) <= ENEMY_TARGET_REACH_THRESHOLD) {
        update_move_target(player_pos, map);
      } else {
        update_movement_towards_target();
      }

      barrel_angle_rad = abs_angle_of_points(pos, player_pos);

      if (shooting_task.did_tick) {
        Vector2 bullet_v{cosf(barrel_angle_rad) * BULLET_SPEED * GetFrameTime(),
                         sinf(barrel_angle_rad) * BULLET_SPEED * GetFrameTime()};
        game_scope.enemy_bullets.emplace_back(pos, bullet_v);
      }
    }

    particle_manager.update();
    shooting_task.update();
  }

  void draw(Map const &map, Vector2 const &player_pos) const {
    draw_texture(asset_manager.textures[ASSET_ENEMY_WHEEL_TEXTURE], Vector2Add(pos, map.world_offset), angle);
    draw_texture(asset_manager.textures[ASSET_ENEMY_BARREL_TEXTURE], Vector2Add(pos, map.world_offset),
                 barrel_angle_rad * RAD2DEG);

    particle_manager.draw(map);
  }

  void kill() {
    if (is_dead) return;

    is_dead = true;
    dying_lifetime.reset();

    for (int i = 0; i < 32; i++) {
      float particle_angle = rand() % 360 * DEG2RAD;
      float particle_speed_jitter = static_cast<float>(rand() % 100) / 200.f + 0.75f;
      Vector2 v{cosf(particle_angle) * ENEMY_EXPLOSION_SPEED * GetFrameTime() * particle_speed_jitter,
                sinf(particle_angle) * ENEMY_EXPLOSION_SPEED * GetFrameTime() * particle_speed_jitter};
      particle_manager.particles.push_back(std::make_unique<ExplosionParticle>(pos, v));
    }
  }

  bool should_be_deleted() const {
    return is_dead && dying_lifetime.is_completed();
  }

  float attack_damage() {
    if (is_dead) return 0;

    return 10;
  }

 private:
  void update_move_target(Vector2 const &player_pos, Map const &map) {
    auto path = map.path_finder.find_path(pos, player_pos);
    if (path.empty()) {
      TraceLog(LOG_INFO, "No path from enemy to player");
      return;
    }

    // Likely the enemy is already at the zone of player.
    if (path.size() <= 1) {
      move_target = player_pos;
      return;
    };

    move_target = Vector2(path[path.size() - 2].x * CELL_DISTANCE, path[path.size() - 2].y * CELL_DISTANCE);
  }

  void update_movement_towards_target() {
    Vector2 delta = Vector2Subtract(pos, move_target);
    float total_dist = Vector2Distance(pos, move_target);
    float move_dist = ENEMY_SPEED * GetFrameTime();

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
