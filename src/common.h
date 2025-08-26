#pragma once

#include <cmath>

#include "raylib.h"
#include "raymath.h"

constexpr int WINDOW_W = 1000;
constexpr int WINDOW_H = 1000;

#define PLAYER_SPEED 400.f
#define PLAYER_ANGLE_SPEED 300.f
#define PLAYER_WALL_COLLIDE_ANGLE_ADJUST PI / 3.f
#define PLAYER_WALL_COLLIDE_DISTANCE_ADJUST 0.5f

#define WORLD_PLAYER_MIDZONE_MARGIN_PERCENTAGE 0.3f

#define BULLET_SPEED 800.f

#define CELL_DISTANCE 50

#define ASSET_PLAYER_TEXTURE 0
#define ASSET_MAP_TEXTURE 1
#define ASSET_ENEMY_TEXTURE 2
#define ASSET_COLLECTIBLE_HEALTH_TEXTURE 3
#define ASSET_COLLECTIBLE_BULLET_TEXTURE 4
#define ASSET_ICON_HEALTH_TEXTURE 5
#define ASSET_ICON_BULLET_TEXTURE 6
#define ASSET_ICON_FPS_TEXTURE 7

#define ASSET_MAP_IMAGE 0

#define REFERENCE_FPS 144

struct IntVector2 {
  int x{};
  int y{};

  constexpr bool operator==(const IntVector2 &other) const {
    return x == other.x && y == other.y;
  }
};

constexpr float int_vector2_dist(IntVector2 lhs, IntVector2 rhs) {
  return sqrtf(powf(lhs.x - rhs.x, 2.f) + powf(lhs.y - rhs.y, 2.f));
}

struct PFCell {
  float prefix{};  // G
  float suffix{};  // H
  IntVector2 p{};

  PFCell(float _prefix, float _suffix, IntVector2 _p) : prefix(_prefix), suffix(_suffix), p(_p) {
  }

  constexpr float total() const {
    return prefix + suffix;
  }

  constexpr bool operator>(const PFCell &other) const {
    return total() > other.total();
  }
};

struct TimedTask {
  double lifetime_seconds;
  double lifetime_end{};

  TimedTask(double _lifetime_seconds) : lifetime_seconds(_lifetime_seconds) {
    reset();
  }

  bool is_completed() const {
    return GetTime() > lifetime_end;
  }

  void reset() {
    lifetime_end = GetTime() + lifetime_seconds;
  }
};

struct RepeatedTask {
  double interval_seconds;
  double last_tick{0.0};
  bool did_tick{false};
  bool is_paused{false};

  explicit RepeatedTask(double _interval_seconds) : interval_seconds(_interval_seconds) {
  }

  void pause() {
    is_paused = true;
  }

  void resume() {
    is_paused = false;
  }

  void set_interval(double _interval_seconds) {
    interval_seconds = _interval_seconds;
  }

  void update() {
    did_tick = false;

    if (is_paused) return;

    if (last_tick + interval_seconds < GetTime()) {
      did_tick = true;
      last_tick += interval_seconds;
    }
  }
};

constexpr Vector2 point_move_with_angle_and_distance(Vector2 p, float angle_rad, float dist) {
  return Vector2{p.x + cosf(angle_rad) * dist, p.y + sinf(angle_rad) * dist};
}

float abs_angle_of_points(Vector2 const &lhs, Vector2 const &rhs) {
  Vector2 rhs_adjusted = Vector2Subtract(rhs, lhs);
  return Vector2Angle({5.f, 0.f}, rhs_adjusted);
}

constexpr IntVector2 cell_idx_from_coord(Vector2 const &p) {
  return IntVector2{(int)roundf(p.x / CELL_DISTANCE), (int)roundf(p.y / CELL_DISTANCE)};
}

void draw_texture(Texture2D const &texture, Vector2 const &pos, float angle) {
  DrawTexturePro(texture, {0.f, 0.f, (float)texture.width, (float)texture.height},
                 {pos.x, pos.y, (float)texture.width, (float)texture.height},
                 Vector2{texture.width / 2.f, texture.height / 2.f}, angle, WHITE);
}
