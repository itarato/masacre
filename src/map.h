#pragma once

#include "asset_manager.h"
#include "common.h"
#include "raylib.h"
#include "raymath.h"

constexpr float WORLD_PLAYER_MIDZONE_MARGIN_PERCENTAGE = 0.3f;
constexpr int WORLD_RANDOM_SPOT_MAX_ATTEMPTS = 32;
constexpr float WORLD_OFFSET_MARGIN = 64.f;

struct Map {
  Vector2 world_offset{};

  void init() {
    reset();
  }

  void reset() {
    world_offset.x = -(asset_manager.images[ASSET_MAP_IMAGE].width - GetScreenWidth()) / 2.f;
    world_offset.y = -(asset_manager.images[ASSET_MAP_IMAGE].height - GetScreenHeight()) / 2.f;
  }

  void update(Vector2 const& player_pos) {
    update_world_offset(player_pos);
  }

  void draw() const {
    DrawTextureV(asset_manager.textures[ASSET_MAP_TEXTURE], world_offset, WHITE);
  }

  void update_world_offset(Vector2 player_pos) {
    Vector2 player_rel_pos = Vector2Add(player_pos, world_offset);

    float margin_left{GetScreenWidth() * WORLD_PLAYER_MIDZONE_MARGIN_PERCENTAGE};
    if (player_rel_pos.x < margin_left) world_offset.x += margin_left - player_rel_pos.x;

    float margin_right{GetScreenWidth() * (1.0f - WORLD_PLAYER_MIDZONE_MARGIN_PERCENTAGE)};
    if (player_rel_pos.x > margin_right) world_offset.x += margin_right - player_rel_pos.x;

    float margin_top{GetScreenHeight() * WORLD_PLAYER_MIDZONE_MARGIN_PERCENTAGE};
    if (player_rel_pos.y < margin_top) world_offset.y += margin_top - player_rel_pos.y;

    float margin_bottom{GetScreenHeight() * (1.0f - WORLD_PLAYER_MIDZONE_MARGIN_PERCENTAGE)};
    if (player_rel_pos.y > margin_bottom) world_offset.y += margin_bottom - player_rel_pos.y;

    if (world_offset.x > WORLD_OFFSET_MARGIN) world_offset.x = WORLD_OFFSET_MARGIN;
    if (world_offset.y > WORLD_OFFSET_MARGIN) world_offset.y = WORLD_OFFSET_MARGIN;
    if (world_offset.x < -width() + GetScreenWidth() - WORLD_OFFSET_MARGIN)
      world_offset.x = -width() + GetScreenWidth() - WORLD_OFFSET_MARGIN;
    if (world_offset.y < -height() + GetScreenHeight() - WORLD_OFFSET_MARGIN)
      world_offset.y = -height() + GetScreenHeight() - WORLD_OFFSET_MARGIN;
  }

  bool is_hit(Vector2 point) const {
    if (point.x < 0.f || point.y < 0.f || point.x >= width() || point.y >= height()) return true;

    // TraceLog(LOG_INFO, "Col: %d", GetImageColor(map_image, point_abs_adjusted.x, point_abs_adjusted.y).r);
    return GetImageColor(asset_manager.images[ASSET_MAP_IMAGE], point.x, point.y).r < 255;
  }

  int width() const {
    return asset_manager.images[ASSET_MAP_IMAGE].width;
  }

  int height() const {
    return asset_manager.images[ASSET_MAP_IMAGE].height;
  }
};
