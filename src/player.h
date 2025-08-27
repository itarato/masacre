#pragma once

#include <memory>
#include <vector>

#include "asset_manager.h"
#include "bullet.h"
#include "collectibles.h"
#include "common.h"
#include "map.h"
#include "particles.h"
#include "raylib.h"
#include "raymath.h"

#define PLAYER_SPEED 400.f
#define PLAYER_ANGLE_SPEED 300.f
#define PLAYER_WALL_COLLIDE_ANGLE_ADJUST PI / 3.f
#define PLAYER_WALL_COLLIDE_DISTANCE_ADJUST 0.5f
#define PLAYER_MAX_HEALTH 100

struct Player {
  std::shared_ptr<Vector2> pos = std::make_shared<Vector2>();
  float circle_frame_radius{};
  float angle;
  std::vector<Bullet> bullets{};

  int bullet_count{};
  float health{};
  int kill_count{};

  ParticleManager particle_manager{};
  RepeatedTask smoke_particle_scheduler{0.08};

  void init() {
    circle_frame_radius = asset_manager.textures[ASSET_PLAYER_TEXTURE].width / 2.f;
  }

  void reset(Map const &map) {
    angle = 270.f;  // Up.
    bullets.clear();
    pos->x = map.width() / 2.f;
    pos->y = map.height() / 2.f;
    bullet_count = 16;
    health = PLAYER_MAX_HEALTH;
    kill_count = 0;
    _should_be_deleted = false;
    particle_manager.reset();
  }

  void update(Map const &map) {
    if (!should_be_deleted()) {
      update_movement(map);
      update_rotation();
      update_shooting();
    }

    std::erase_if(bullets, [](auto e) { return e.should_be_deleted; });
    for (auto &bullet : bullets) bullet.update(map);

    particle_manager.update();

    if (health >= PLAYER_MAX_HEALTH * 0.95) {
      smoke_particle_scheduler.pause();
    } else if (health >= PLAYER_MAX_HEALTH * 0.25) {
      smoke_particle_scheduler.resume();
      smoke_particle_scheduler.set_interval(0.08);
    } else {
      smoke_particle_scheduler.resume();
      smoke_particle_scheduler.set_interval(0.02);
    }

    smoke_particle_scheduler.update();

    if (smoke_particle_scheduler.did_tick) particle_manager.particles.push_back(std::make_unique<SmokeParticle>(*pos));
  }

  void draw(Map const &map) const {
    // Draw all bullets.
    for (auto const &bullet : bullets) bullet.draw(map);

    // Draw main player.
    draw_texture(asset_manager.textures[ASSET_PLAYER_TEXTURE], screen_relative_center(map.world_offset), angle);

    // Draw HUD.
    DrawRectangle(GetScreenWidth() - 144, GetScreenHeight() - 80, 140, 76, ColorAlpha(DARKGRAY, 0.9f));

    // Health bar.
    DrawRectangle(GetScreenWidth() - 112, GetScreenHeight() - 24, (health / PLAYER_MAX_HEALTH) * 100.f, 12, RED);
    DrawRectangleLinesEx(
        Rectangle{static_cast<float>(GetScreenWidth() - 116), static_cast<float>(GetScreenHeight() - 28), 108.f, 20.f},
        2.f, WHITE);
    DrawTexture(asset_manager.textures[ASSET_ICON_HEALTH_TEXTURE], GetScreenWidth() - 140, GetScreenHeight() - 28,
                WHITE);

    // Bullet bar.
    DrawTexture(asset_manager.textures[ASSET_ICON_BULLET_TEXTURE], GetScreenWidth() - 140, GetScreenHeight() - 52,
                WHITE);
    DrawText(TextFormat("%d", bullet_count), GetScreenWidth() - 116, GetScreenHeight() - 52, 20, WHITE);

    // FPS bar.
    DrawTexture(asset_manager.textures[ASSET_ICON_FPS_TEXTURE], GetScreenWidth() - 140, GetScreenHeight() - 76, WHITE);
    DrawText(TextFormat("%d FPS", GetFPS()), GetScreenWidth() - 116, GetScreenHeight() - 76, 20, WHITE);

    // Draw particles.
    particle_manager.draw(map);
  }

  void update_rotation() {
    if (IsKeyDown(KEY_LEFT)) angle -= PLAYER_ANGLE_SPEED * GetFrameTime();
    if (IsKeyDown(KEY_RIGHT)) angle += PLAYER_ANGLE_SPEED * GetFrameTime();

    float turn_axis = GetGamepadAxisMovement(0, 0);
    if (turn_axis != 0.f) angle += PLAYER_ANGLE_SPEED * GetFrameTime() * turn_axis;
  }

  void update_shooting() {
    if (bullet_count <= 0) return;

    if (IsKeyPressed(KEY_LEFT_CONTROL) || IsGamepadButtonPressed(0, 7)) {
      bullet_count--;

      float bullet_angle_rad = angle * DEG2RAD;
      Vector2 bullet_v{cosf(bullet_angle_rad) * BULLET_SPEED * GetFrameTime(),
                       sinf(bullet_angle_rad) * BULLET_SPEED * GetFrameTime()};
      bullets.emplace_back(*pos, bullet_v);
    }
  }

  void update_movement(Map const &map) {
    Vector2 old_pos = *pos;

    if (IsKeyDown(KEY_UP)) move_to_relative_direction(0.f, 1.f);
    if (IsKeyDown(KEY_DOWN)) move_to_relative_direction(PI, 1.f);

    if (IsKeyDown(KEY_A)) move_to_relative_direction(-PI / 2.f, 1.f);
    if (IsKeyDown(KEY_D)) move_to_relative_direction(PI / 2.f, 1.f);

    float move_vertical_axis_fwd = GetGamepadAxisMovement(0, 5);
    if (move_vertical_axis_fwd > -1.f) move_to_relative_direction(0.f, (move_vertical_axis_fwd + 1.f) / 2.f);

    float move_vertical_axis_bwd = GetGamepadAxisMovement(0, 4);
    if (move_vertical_axis_bwd > -1.f) move_to_relative_direction(PI, (move_vertical_axis_bwd + 1.f) / 2.f);

    float move_horizontal_axis = GetGamepadAxisMovement(0, 2);
    if (move_horizontal_axis != 0.f) move_to_relative_direction(-PI / 2.f, -move_horizontal_axis / 2.f);

    if (!Vector2Equals(*pos, old_pos) && map.is_hit(*pos)) {
      float move_angle_rad = abs_angle_of_points(old_pos, *pos);

      float dist = Vector2Distance(old_pos, *pos);
      float angle_left_attempt = move_angle_rad - PLAYER_WALL_COLLIDE_ANGLE_ADJUST;
      Vector2 left_attempt =
          point_move_with_angle_and_distance(old_pos, angle_left_attempt, dist * PLAYER_WALL_COLLIDE_DISTANCE_ADJUST);
      if (!map.is_hit(left_attempt)) {
        *pos = left_attempt;
        return;
      }

      float angle_right_attempt = move_angle_rad + PLAYER_WALL_COLLIDE_ANGLE_ADJUST;
      Vector2 right_attempt =
          point_move_with_angle_and_distance(old_pos, angle_right_attempt, dist * PLAYER_WALL_COLLIDE_DISTANCE_ADJUST);
      if (!map.is_hit(right_attempt)) {
        *pos = right_attempt;
        return;
      }

      *pos = old_pos;
    }
  }

  [[nodiscard]] Vector2 screen_relative_center(const Vector2 &world_offset) const {
    return Vector2Add(*pos, world_offset);
  }

  void move_to_relative_direction(const float rel_rad_angle, float rate) {
    const float abs_angle = (angle * DEG2RAD) + rel_rad_angle;
    pos->x += cosf(abs_angle) * PLAYER_SPEED * GetFrameTime() * rate;
    pos->y += sinf(abs_angle) * PLAYER_SPEED * GetFrameTime() * rate;
  }

  void hurt(float hurt_val) {
    health -= hurt_val;

    if (health <= 0) {
      _should_be_deleted = true;
    }
  }

  [[nodiscard]] bool should_be_deleted() const {
    return _should_be_deleted;
  }

  void consume(Collectible const &collectible) {
    switch (collectible.ty) {
      case CollectibleType::Bullet:
        bullet_count += 10;
        break;
      case CollectibleType::Health:
        health += 20;
        if (health > PLAYER_MAX_HEALTH) health = PLAYER_MAX_HEALTH;
        break;
      default:
        TraceLog(LOG_ERROR, "Unhandled collectible type");
        exit(EXIT_FAILURE);
    }
  }

 private:
  bool _should_be_deleted{false};
};
