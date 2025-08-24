#pragma once

#include <cmath>

#include "raylib.h"
#include "raymath.h"

const int WINDOW_W = 1000;
const int WINDOW_H = 1000;

#define PLAYER_SPEED 400.f
#define PLAYER_ANGLE_SPEED 300.f
#define PLAYER_WALL_COLLIDE_ANGLE_ADJUST PI / 3.f
#define PLAYER_WALL_COLLIDE_DISTANCE_ADJUST 0.5f

#define WORLD_PLAYER_MIDZONE_MARGIN_PERCENTAGE 0.3f

#define BULLET_SPEED 800.f

#define CELL_DISTANCE 50

struct IntVector2 {
  int x{};
  int y{};

  bool operator==(const IntVector2 &other) const {
    return x == other.x && y == other.y;
  }
};

struct PFCell {
  float prefix{};  // G
  float suffix{};  // H
  IntVector2 p{};

  PFCell(float _prefix, float _suffix, IntVector2 _p) : prefix(_prefix), suffix(_suffix), p(_p) {
  }

  float total() const {
    return prefix + suffix;
  }

  bool operator>(const PFCell &other) const {
    return total() > other.total();
  }
};

Vector2 point_move_with_angle_and_distance(Vector2 p, float angle_rad, float dist) {
  return Vector2{p.x + cosf(angle_rad) * dist, p.y + sinf(angle_rad) * dist};
}

float abs_angle_of_points(Vector2 const &lhs, Vector2 const &rhs) {
  Vector2 rhs_adjusted = Vector2Subtract(rhs, lhs);
  return Vector2Angle({5.f, 0.f}, rhs_adjusted);
}

IntVector2 cell_idx_from_coord(Vector2 const &p) {
  return IntVector2{(int)roundf(p.x / CELL_DISTANCE), (int)roundf(p.y / CELL_DISTANCE)};
}
