#pragma once

#include "common.h"
#include "map.h"
#include "raylib.h"

constexpr float MINE_RADIUS = 10.f;

struct Mine final : AttackDamage {
  Vector2 pos;
  const float circle_frame_radius{MINE_RADIUS};
  bool should_be_deleted{false};

  Mine(Vector2 _pos) : AttackDamage(150.f), pos(_pos) {
  }

  void draw(Map const &map) const {
    DrawCircleV(Vector2Add(pos, map.world_offset), MINE_RADIUS, MAROON);
    DrawCircleV(Vector2Add(pos, map.world_offset), MINE_RADIUS / 2.f, GOLD);
  }

  void kill() {
    should_be_deleted = true;
  }

  static float blast_radius() {
    return MINE_RADIUS * 10.f;
  }
};
