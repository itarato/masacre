#pragma once

#include <cstdlib>

#include "asset_manager.h"
#include "common.h"
#include "map.h"
#include "raylib.h"

constexpr float COLLECTIBLE_RADIUS = 20.f;

enum class CollectibleType {
  Bullet,
  Health,
};

struct Collectible {
  Vector2 pos;
  CollectibleType ty;
  bool should_be_deleted{false};
  float circle_frame_radius;

  Collectible(Vector2 _pos, CollectibleType _ty) : pos(_pos), ty(_ty) {
    circle_frame_radius = asset_manager.textures[ASSET_COLLECTIBLE_HEALTH_TEXTURE].width / 2.f;
  }

  void draw(Map const& map) const {
    int texture_id;
    switch (ty) {
      case CollectibleType::Bullet:
        texture_id = ASSET_COLLECTIBLE_BULLET_TEXTURE;
        break;
      case CollectibleType::Health:
        texture_id = ASSET_COLLECTIBLE_HEALTH_TEXTURE;
        break;
      default:
        TraceLog(LOG_ERROR, "Unhandled collectible type");
        exit(EXIT_FAILURE);
        break;
    }

    draw_texture(asset_manager.textures[texture_id], Vector2Add(pos, map.world_offset), 0.f);
  }
};
