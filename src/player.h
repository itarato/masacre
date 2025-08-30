#pragma once

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

#include "asset_manager.h"
#include "bullet.h"
#include "collectibles.h"
#include "common.h"
#include "map.h"
#include "particles.h"
#include "path_finder.h"
#include "raylib.h"
#include "raymath.h"

constexpr float PLAYER_MAX_SPEED = 400.f;
constexpr float PLAYER_ANGLE_SPEED = 300.f;
constexpr float PLAYER_WALL_COLLIDE_ANGLE_ADJUST = PI / 3.f;
constexpr float PLAYER_WALL_COLLIDE_DISTANCE_ADJUST = 1.0f;
constexpr int PLAYER_MAX_HEALTH = 100;
constexpr int PLAYER_STARTER_BULLET_COUNT = 100;
constexpr int PLAYER_HEALTH_COLLECT = 25;
constexpr int PLAYER_BULLET_COLLECT = 50;

struct Player {
  std::shared_ptr<Vector2> pos = std::make_shared<Vector2>();
  float circle_frame_radius{};
  float angle{};         // Degree.
  float target_angle{};  // Degree.
  float velocity{};
  std::vector<Bullet> bullets{};
  int bullet_count{};
  float health{};
  int kill_count{};

  ParticleManager particle_manager{};
  RepeatedTask smoke_particle_scheduler{0.08};
  RepeatedTask wheel_trace_particle_scheduler{0.05};
  RepeatedTask rapid_fire_scheduler{0.05};

  void init() {
    circle_frame_radius = asset_manager.textures[ASSET_PLAYER_TEXTURE].width / 2.f;
  }

  void reset(PathFinder const &path_finder) {
    angle = 270.f;  // Up.
    target_angle = 270.f;
    velocity = 0.f;
    bullets.clear();
    pos->x = path_finder.start_pos.x * CELL_DISTANCE;
    pos->y = path_finder.start_pos.y * CELL_DISTANCE;
    bullet_count = PLAYER_STARTER_BULLET_COUNT;
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
    if (!should_be_deleted()) {
      draw_texture(asset_manager.textures[ASSET_PLAYER_TEXTURE], screen_relative_center(map.world_offset),
                   target_angle);
    } else {
      draw_texture(asset_manager.textures[ASSET_PLAYER_BROKEN_TEXTURE], screen_relative_center(map.world_offset),
                   target_angle);
    }

    // Draw HUD.
    DrawRectangle(GetScreenWidth() - 144, GetScreenHeight() - 104, 140, 100, ColorAlpha(DARKGRAY, 0.9f));

    // Health bar.
    DrawRectangle(GetScreenWidth() - 112, GetScreenHeight() - 96, (health / PLAYER_MAX_HEALTH) * 100.f, 12, RED);
    DrawRectangleLinesEx(
        Rectangle{static_cast<float>(GetScreenWidth() - 116), static_cast<float>(GetScreenHeight() - 100), 108.f, 20.f},
        2.f, WHITE);
    DrawTexture(asset_manager.textures[ASSET_ICON_HEALTH_TEXTURE], GetScreenWidth() - 140, GetScreenHeight() - 100,
                WHITE);

    // Kill bar.
    DrawTexture(asset_manager.textures[ASSET_ICON_KILLS_TEXTURE], GetScreenWidth() - 140, GetScreenHeight() - 76,
                WHITE);
    DrawText(TextFormat("%d", kill_count), GetScreenWidth() - 116, GetScreenHeight() - 76, 20, WHITE);

    // Bullet bar.
    DrawTexture(asset_manager.textures[ASSET_ICON_BULLET_TEXTURE], GetScreenWidth() - 140, GetScreenHeight() - 52,
                WHITE);
    DrawText(TextFormat("%d", bullet_count), GetScreenWidth() - 116, GetScreenHeight() - 52, 20, WHITE);

    // FPS bar.
    DrawTexture(asset_manager.textures[ASSET_ICON_FPS_TEXTURE], GetScreenWidth() - 140, GetScreenHeight() - 28, WHITE);
    DrawText(TextFormat("%d FPS", GetFPS()), GetScreenWidth() - 116, GetScreenHeight() - 28, 20, WHITE);

    // Draw particles.
    particle_manager.draw(map);
  }

  void update_rotation() {
    if (IsKeyDown(KEY_LEFT)) target_angle -= PLAYER_ANGLE_SPEED * GetFrameTime();
    if (IsKeyDown(KEY_RIGHT)) target_angle += PLAYER_ANGLE_SPEED * GetFrameTime();

    if (IsGamepadAvailable(0)) {
      float turn_axis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
      if (turn_axis != 0.f) target_angle += PLAYER_ANGLE_SPEED * GetFrameTime() * turn_axis;
    }
  }

  void update_shooting() {
    if (bullet_count <= 0) return;

    if (IsKeyPressed(KEY_LEFT_CONTROL) || IsGamepadButtonPressed(0, 7)) {
      unconditional_shoot();
    }

    if (IsKeyDown(KEY_LEFT_ALT) || IsGamepadButtonDown(0, 8)) {
      rapid_fire_scheduler.update();
      if (rapid_fire_scheduler.did_tick) {
        unconditional_shoot();
      }
    }
  }

  void unconditional_shoot() {
    bullet_count--;

    float bullet_angle_rad = target_angle * DEG2RAD;
    Vector2 bullet_v{cosf(bullet_angle_rad) * BULLET_SPEED * GetFrameTime(),
                     sinf(bullet_angle_rad) * BULLET_SPEED * GetFrameTime()};
    bullets.emplace_back(*pos, bullet_v);

    PlaySound(asset_manager.sounds[ASSET_SOUND_PLAYER_SHOOT]);
  }

  void update_movement(Map const &map) {
    const Vector2 old_pos = *pos;

    bool had_movement = false;
    if (IsKeyDown(KEY_UP)) {
      velocity += GetFrameTime() * 500.f;
      had_movement = true;
    }
    if (IsKeyDown(KEY_DOWN)) {
      velocity -= GetFrameTime() * 500.f;
      had_movement = true;
    }

    // Disabling gamepad - the keyboard is pretending to be one and the axis is 0.0 instead of -1.0 as default state
    // which makes a movement triggered which prevents slowing down.
    // if (IsGamepadAvailable(0)) {
    // TraceLog(LOG_INFO, "GP0=%.2f %s", GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER), GetGamepadName(0));
    //   float move_vertical_axis_fwd = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER);
    //   if (move_vertical_axis_fwd > -1.f) {
    //     velocity += GetFrameTime() * 500.f * ((move_vertical_axis_fwd + 1.f) / 2.f);
    //     had_movement = true;
    //   }
    //
    //   float move_vertical_axis_bwd = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER);
    //   if (move_vertical_axis_bwd > -1.f) {
    //     velocity -= GetFrameTime() * 500.f * ((move_vertical_axis_bwd + 1.f) / 2.f);
    //     had_movement = true;
    //   }
    // }

    // Apply velocity + angle.
    pos->x += cosf(angle * DEG2RAD) * velocity * GetFrameTime();
    pos->y += sinf(angle * DEG2RAD) * velocity * GetFrameTime();

    // Clamp velocity.
    if (velocity > PLAYER_MAX_SPEED) velocity = PLAYER_MAX_SPEED;
    if (velocity < -PLAYER_MAX_SPEED) velocity = -PLAYER_MAX_SPEED;

    // Slowdown.
    if (had_movement) {
      angle = smoothstep(angle, target_angle, 0.1f);
    } else {
      fps_independent_multiply(&velocity, 0.98f);

      if (fabs(velocity) < 60.f) velocity = 0.f;
    }

    if (!Vector2Equals(*pos, old_pos) && map.is_hit(*pos)) {
      float angle_left_attempt = (angle * DEG2RAD) - PLAYER_WALL_COLLIDE_ANGLE_ADJUST;
      Vector2 left_attempt = point_move_with_angle_and_distance(old_pos, angle_left_attempt, 300.f * GetFrameTime());

      float angle_right_attempt = (angle * DEG2RAD) + PLAYER_WALL_COLLIDE_ANGLE_ADJUST;
      Vector2 right_attempt = point_move_with_angle_and_distance(old_pos, angle_right_attempt, 300.f * GetFrameTime());

      if (!map.is_hit(left_attempt)) {
        // TraceLog(LOG_DEBUG, "Left glide | Angle=%.2f | TargetAngle=%.2f", angle, target_angle);
        *pos = left_attempt;
      } else if (!map.is_hit(right_attempt)) {
        // TraceLog(LOG_DEBUG, "Right glide | Angle=%.2f | TargetAngle=%.2f", angle, target_angle);
        *pos = right_attempt;
      } else {
        // TraceLog(
        //     LOG_DEBUG,
        //     "No glide | Angle=%.2f | TargetAngle=%.2f | Old=%.2f:%.2f New=%.2f:%.2f Left=%.2f:%.2f Right=%.2f:%.2f",
        //     angle, target_angle, XY(old_pos), XY((*pos)), XY(left_attempt), XY(right_attempt));
        *pos = old_pos;
        velocity = 0.f;
      }
    }

    if (!Vector2Equals(*pos, old_pos)) {
      wheel_trace_particle_scheduler.update();

      if (wheel_trace_particle_scheduler.did_tick) {
        particle_manager.particles.emplace_back(std::make_unique<TraceParticle>(*pos, angle + 90));
      }
    }
  }

  [[nodiscard]] Vector2 screen_relative_center(const Vector2 &world_offset) const {
    return Vector2Add(*pos, world_offset);
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
        bullet_count += PLAYER_BULLET_COLLECT;
        break;
      case CollectibleType::Health:
        health += PLAYER_HEALTH_COLLECT;
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
