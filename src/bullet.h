#pragma once

#include "raylib.h"
#include "raymath.h"

struct Bullet {
  Vector2 pos{};
  Vector2 v{};

  Bullet(Vector2 pos, Vector2 v) : pos(pos), v(v) {
  }

  void draw(Vector2 world_offset) const {
    DrawCircleV(Vector2Add(pos, world_offset), 4.f, DARKGREEN);
  }

  void update() {
    pos.x += v.x;
    pos.y += v.y;
  }

  bool is_dead() const {
    return pos.x < 0.f || pos.y < 0.f || pos.x > GetScreenWidth() || pos.y > GetScreenHeight();
  }
};
