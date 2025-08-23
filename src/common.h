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

Vector2 point_move_with_angle_and_distance(Vector2 p, float angle_rad, float dist) {
  return Vector2{p.x + cosf(angle_rad) * dist, p.y + sinf(angle_rad) * dist};
}

float abs_angle_of_points(Vector2 const &lhs, Vector2 const &rhs) {
  Vector2 rhs_adjusted = Vector2Subtract(rhs, lhs);
  return Vector2Angle({5.f, 0.f}, rhs_adjusted);
}
