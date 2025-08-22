#pragma once

#include "raylib.h"
#include "raymath.h"

struct Bullet {
  Vector2 pos{};
  Vector2 v{};

  Bullet(Vector2 pos, Vector2 v) : pos(pos), v(v) {
  }

  void draw(Vector2 const &world_offset) const {
    DrawCircleV(get_rel_pos(world_offset), 4.f, DARKGREEN);
  }

  void update() {
    pos.x += v.x;
    pos.y += v.y;
  }

  Vector2 get_rel_pos(Vector2 const &world_offset) const {
    return Vector2Add(pos, world_offset);
  }

  bool is_dead(Vector2 const &world_offset) const {
    Vector2 rel_pos = get_rel_pos(world_offset);
    return rel_pos.x < 0.f || rel_pos.y < 0.f || rel_pos.x > GetScreenWidth() || rel_pos.y > GetScreenHeight();
  }
};
