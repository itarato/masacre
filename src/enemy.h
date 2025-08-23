#pragma once

#include "raylib.h"

struct Enemy {
  void update() {
  }

  void draw() const {
    DrawCircleV(GetMousePosition(), 10, RED);
  }
};
