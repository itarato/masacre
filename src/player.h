#pragma once

#include <algorithm>
#include <vector>

#include "asset_manager.h"
#include "bullet.h"
#include "collectibles.h"
#include "common.h"
#include "map.h"
#include "raylib.h"
#include "raymath.h"

#define PLAYER_MAX_HEALTH 100

struct Player {
  Vector2 pos{};
  float circle_frame_radius{};
  float angle;
  std::vector<Bullet> bullets{};

  int bullet_count{};
  int health{};
  int kill_count{};

  Player() {
  }

  ~Player() {
  }

  void init() {
    circle_frame_radius = asset_manager.textures[ASSET_PLAYER_TEXTURE].width / 2.f;
  }

  void reset(Map const &map) {
    angle = 270.f;  // Up.
    bullets.clear();
    pos.x = map.map_image.width / 2.f;
    pos.y = map.map_image.height / 2.f;
    bullet_count = 16;
    health = PLAYER_MAX_HEALTH;
    kill_count = 0;
    _is_dead = false;
  }

  void update(Map const &map) {
    if (!is_dead()) {
      update_movement(map);
      update_rotation();
      update_shooting();
    }

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const auto &e) { return e.is_dead; }),
                  bullets.end());
    for (auto &bullet : bullets) bullet.update(map);
  }

  void draw(Map const &map) const {
    draw_texture(asset_manager.textures[ASSET_PLAYER_TEXTURE], screen_relative_center(map.world_offset), angle);

    for (auto const &bullet : bullets) bullet.draw(map.world_offset);

    DrawText(TextFormat("Player: %.2f:%.2f", pos.x, pos.y), 10, 70, 20, ORANGE);
    DrawText(map.is_hit(pos) ? "hit" : "miss", 10, 30, 20, ORANGE);

    DrawRectangle(0, GetScreenHeight() - 22, GetScreenWidth(), 22, DARKGRAY);
    DrawText(TextFormat("[ Health: %d ][ Ammo: %d ][ Kills: %d ]", health, bullet_count, kill_count), 10,
             GetScreenHeight() - 20, 20, RAYWHITE);
  }

  void update_rotation() {
    if (IsKeyDown(KEY_LEFT)) angle -= PLAYER_ANGLE_SPEED * GetFrameTime();
    if (IsKeyDown(KEY_RIGHT)) angle += PLAYER_ANGLE_SPEED * GetFrameTime();
  }

  void update_shooting() {
    if (IsKeyPressed(KEY_LEFT_CONTROL) && bullet_count > 0) {
      bullet_count--;

      float bullet_angle_rad = angle * DEG2RAD;
      Vector2 bullet_v{cosf(bullet_angle_rad) * BULLET_SPEED * GetFrameTime(),
                       sinf(bullet_angle_rad) * BULLET_SPEED * GetFrameTime()};
      bullets.emplace_back(pos, bullet_v);
    }
  }

  void update_movement(Map const &map) {
    Vector2 old_pos = pos;

    if (IsKeyDown(KEY_UP)) move_to_relative_direction(0.f);
    if (IsKeyDown(KEY_DOWN)) move_to_relative_direction(PI);

    if (IsKeyDown(KEY_A)) move_to_relative_direction(-PI / 2.f);
    if (IsKeyDown(KEY_D)) move_to_relative_direction(PI / 2.f);

    if (!Vector2Equals(pos, old_pos) && map.is_hit(pos)) {
      float move_angle_rad = abs_angle_of_points(old_pos, pos);

      float dist = Vector2Distance(old_pos, pos);
      float angle_left_attempt = move_angle_rad - PLAYER_WALL_COLLIDE_ANGLE_ADJUST;
      Vector2 left_attempt =
          point_move_with_angle_and_distance(old_pos, angle_left_attempt, dist * PLAYER_WALL_COLLIDE_DISTANCE_ADJUST);
      if (!map.is_hit(left_attempt)) {
        pos = left_attempt;
        return;
      }

      float angle_right_attempt = move_angle_rad + PLAYER_WALL_COLLIDE_ANGLE_ADJUST;
      Vector2 right_attempt =
          point_move_with_angle_and_distance(old_pos, angle_right_attempt, dist * PLAYER_WALL_COLLIDE_DISTANCE_ADJUST);
      if (!map.is_hit(right_attempt)) {
        pos = right_attempt;
        return;
      }

      pos = old_pos;
    }
  }

  Vector2 screen_relative_center(Vector2 world_offset) const {
    return Vector2Add(pos, world_offset);
  }

  void move_to_relative_direction(float rel_rad_angle) {
    float abs_angle = (angle * DEG2RAD) + rel_rad_angle;
    pos.x += cosf(abs_angle) * PLAYER_SPEED * GetFrameTime();
    pos.y += sinf(abs_angle) * PLAYER_SPEED * GetFrameTime();
  }

  void hurt() {
    int hurt_val = 1;
    if (health > hurt_val) {
      //  TODO: This is frame rate dependent. Add 1s sleep.
      health -= hurt_val;
    } else {
      health = 0;
    }

    if (health == 0) {
      _is_dead = true;
    }
  }

  bool is_dead() const {
    return _is_dead;
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
  bool _is_dead{false};
};
