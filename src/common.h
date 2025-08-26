#pragma once

#include <cmath>

#include "raylib.h"
#include "raymath.h"

constexpr int WINDOW_W = 1000;
constexpr int WINDOW_H = 1000;

#define BULLET_SPEED 800.f
#define CELL_DISTANCE 50
#define REFERENCE_FPS 144

/**
 * Returns a random between 0.0 and 1.0 (both included).
 */
float randf() {
  return static_cast<float>(rand() % 1001) / 1000.0;
}

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
  bool did_tick{false};
  bool is_paused{false};
  double additional_jitter{0.f};
  double current_jitter{0.f};
  double last_tick;

  explicit RepeatedTask(double _interval_seconds) : interval_seconds(_interval_seconds), last_tick(GetTime()) {
  }

  explicit RepeatedTask(double _interval_seconds, double _additional_jitter)
      : interval_seconds(_interval_seconds), additional_jitter(_additional_jitter), last_tick(GetTime()) {
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

    if (last_tick + interval_seconds + current_jitter < GetTime()) {
      did_tick = true;
      last_tick += interval_seconds + current_jitter;

      if (additional_jitter > 0.f) {
        current_jitter = randf() * additional_jitter;
      }
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

void draw_texture(Texture2D const &texture, Vector2 const &pos, float angle_deg) {
  DrawTexturePro(texture, {0.f, 0.f, (float)texture.width, (float)texture.height},
                 {pos.x, pos.y, (float)texture.width, (float)texture.height},
                 Vector2{texture.width / 2.f, texture.height / 2.f}, angle_deg, WHITE);
}

float fps_independent_multiplier() {
  return static_cast<float>(REFERENCE_FPS) / static_cast<float>(GetFPS());
}
