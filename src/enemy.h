#pragma once

#include "common.h"
#include "map.h"
#include "raylib.h"

#define ENEMY_SPEED 300.f

struct Enemy {
  Vector2 pos;
  Vector2 move_target;

  void update() {
  }

  void draw(Map const &map) const {
    DrawCircleV(GetMousePosition(), 10, RED);
  }
};
