#pragma once

#include "asset_manager.h"
#include "map.h"
#include "raylib.h"
#include "raymath.h"

struct Bullet {
  Vector2 pos{};
  Vector2 v{};
  bool should_be_deleted{false};
  float angle_deg{};

  Bullet(Vector2 _pos, Vector2 _v) : pos(_pos), v(_v) {
    angle_deg = abs_angle_of_points(Vector2(), v) * RAD2DEG;
  }

  void draw(Map const &map) const {
    draw_texture(asset_manager.textures[ASSET_BULLET_TEXTURE], Vector2Add(pos, map.world_offset), angle_deg);
  }

  void update(Map const &map) {
    pos.x += v.x;
    pos.y += v.y;

    if (!should_be_deleted) {
      if (map.is_hit(pos)) {
        should_be_deleted = true;
      } else {
        Vector2 rel_pos = get_rel_pos(map.world_offset);
        should_be_deleted |=
            rel_pos.x < 0.f || rel_pos.y < 0.f || rel_pos.x > GetScreenWidth() || rel_pos.y > GetScreenHeight();
      }
    }
  }

  Vector2 get_rel_pos(Vector2 const &world_offset) const {
    return Vector2Add(pos, world_offset);
  }

  void kill() {
    should_be_deleted = true;
  }

  float attack_damage() const {
    return 10.0f;
  }
};
