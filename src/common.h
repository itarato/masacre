#pragma once

#include <cmath>

#include "raylib.h"
#include "raymath.h"

constexpr int WINDOW_W = 800;
constexpr int WINDOW_H = 800;
constexpr float BULLET_SPEED = 800.f;
constexpr int CELL_DISTANCE = 50;
constexpr int REFERENCE_FPS = 144;

static u_int64_t global_object_id{0};

/**
 * Returns a random between 0.0 and 1.0 (both included).
 */
inline float randf() {
  return static_cast<float>(rand() % 1001) / 1000.f;
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

inline Vector2 int_vector2_to_vector2(IntVector2 p) {
  return Vector2{static_cast<float>(p.x * CELL_DISTANCE), static_cast<float>(p.y * CELL_DISTANCE)};
}

struct PFCell {
  float prefix{};  // G
  float suffix{};  // H
  IntVector2 p{};

  PFCell(float _prefix, float _suffix, IntVector2 _p) : prefix(_prefix), suffix(_suffix), p(_p) {
  }

  [[nodiscard]] constexpr float total() const {
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
    update_jitter();
  }

  explicit RepeatedTask(double _interval_seconds, double _additional_jitter)
      : interval_seconds(_interval_seconds), additional_jitter(_additional_jitter), last_tick(GetTime()) {
    update_jitter();
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
      last_tick = GetTime();

      if (additional_jitter > 0.f) update_jitter();
    }
  }

 private:
  void update_jitter() {
    current_jitter = randf() * additional_jitter;
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

float mod_reduced(float v, float mod) {
  return v - fmod(v, mod);
}

std::vector<IntVector2> ordered_cell_indices_from_coord(Vector2 const &p) {
  int x_closest, x_farthest, y_closest, y_farthest;

  if (p.x <= mod_reduced(p.x, CELL_DISTANCE) + CELL_DISTANCE / 2.f) {
    x_closest = static_cast<int>(floorf(p.x / CELL_DISTANCE));
    x_farthest = static_cast<int>(ceilf(p.x / CELL_DISTANCE));
  } else {
    x_closest = static_cast<int>(ceilf(p.x / CELL_DISTANCE));
    x_farthest = static_cast<int>(floorf(p.x / CELL_DISTANCE));
  }

  if (p.y <= mod_reduced(p.y, CELL_DISTANCE) + CELL_DISTANCE / 2.f) {
    y_closest = static_cast<int>(floorf(p.y / CELL_DISTANCE));
    y_farthest = static_cast<int>(ceilf(p.y / CELL_DISTANCE));
  } else {
    y_closest = static_cast<int>(ceilf(p.y / CELL_DISTANCE));
    y_farthest = static_cast<int>(floorf(p.y / CELL_DISTANCE));
  }

  std::vector<IntVector2> out{};
  out.resize(4);

  out[0] = {x_closest, y_closest};
  out[1] = {x_closest, y_farthest};
  out[2] = {x_farthest, y_closest};
  out[3] = {x_farthest, y_farthest};

  return out;
}

// Draws texture at the center point as origin.
void draw_texture(Texture2D const &texture, Vector2 const &pos, float angle_deg) {
  DrawTexturePro(texture, {0.f, 0.f, (float)texture.width, (float)texture.height},
                 {pos.x, pos.y, (float)texture.width, (float)texture.height},
                 Vector2{texture.width / 2.f, texture.height / 2.f}, angle_deg, WHITE);
}

float fps_independent_multiplier() {
  return static_cast<float>(REFERENCE_FPS) / static_cast<float>(GetFPS());
}

void fps_independent_multiply(float *v, float mul) {
  *v *= powf(mul, fps_independent_multiplier());
}
