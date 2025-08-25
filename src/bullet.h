#pragma once

#include "raylib.h"
#include "raymath.h"

struct Bullet {
  Vector2 pos{};
  Vector2 v{};
  bool is_dead{false};

  Bullet(Vector2 _pos, Vector2 _v) : pos(_pos), v(_v) {
  }

  void draw(Vector2 const &world_offset) const {
    DrawCircleV(get_rel_pos(world_offset), 4.f, DARKGREEN);
  }

  void update(Map const &map) {
    pos.x += v.x;
    pos.y += v.y;

    if (!is_dead) {
      if (map.is_hit(pos)) {
        is_dead = true;
      } else {
        Vector2 rel_pos = get_rel_pos(map.world_offset);
        is_dead |= rel_pos.x < 0.f || rel_pos.y < 0.f || rel_pos.x > GetScreenWidth() || rel_pos.y > GetScreenHeight();
      }
    }
  }

  Vector2 get_rel_pos(Vector2 const &world_offset) const {
    return Vector2Add(pos, world_offset);
  }

  void kill() {
    is_dead = true;
  }
};
