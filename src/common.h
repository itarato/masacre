#pragma once

const int WINDOW_W = 1000;
const int WINDOW_H = 1000;

#define int_t short

#define PLAYER_SPEED 400.f
#define PLAYER_ANGLE_SPEED 300.f

#define WORLD_PLAYER_MIDZONE_MARGIN_PERCENTAGE 0.3f

#define BULLET_SPEED 800.f

struct int_point {
  int_t x;
  int_t y;
};
