#pragma once

#include "asset_manager.h"
#include "common.h"
#include "path_finder.h"
#include "raylib.h"
#include "raymath.h"

#define WORLD_PLAYER_MIDZONE_MARGIN_PERCENTAGE 0.3f
#define WORLD_RANDOM_SPOT_MAX_ATTEMPTS 32

struct Map {
  Vector2 world_offset{};
  PathFinder path_finder{};

  void init() {
    path_finder_init();

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
  }

  bool is_hit(Vector2 point) const {
    // TraceLog(LOG_INFO, "Col: %d", GetImageColor(map_image, point_abs_adjusted.x, point_abs_adjusted.y).r);
    return GetImageColor(asset_manager.images[ASSET_MAP_IMAGE], point.x, point.y).r < 255;
  }

  void path_finder_init() {
    path_finder.cells_w = asset_manager.images[ASSET_MAP_IMAGE].width / CELL_DISTANCE;
    path_finder.cells_h = asset_manager.images[ASSET_MAP_IMAGE].height / CELL_DISTANCE;

    if (path_finder.cells_w * path_finder.cells_w >= MAX_GRID_CELLS) {
      TraceLog(LOG_ERROR, "Map too large");
      exit(EXIT_FAILURE);
    }

    for (int y = 0; y <= path_finder.cells_h; y++) {
      for (int x = 0; x <= path_finder.cells_w; x++) {
        if (is_hit(Vector2{static_cast<float>(x * CELL_DISTANCE), static_cast<float>(y * CELL_DISTANCE)})) {
          path_finder.cells[y * path_finder.cells_w + x] = 0;
        } else {
          path_finder.cells[y * path_finder.cells_w + x] = PF_CELL_ACCESSIBLE_FLAG;
        }
      }
    }

    path_finder.post_init();
  }

  int width() const {
    return asset_manager.images[ASSET_MAP_IMAGE].width;
  }

  int height() const {
    return asset_manager.images[ASSET_MAP_IMAGE].height;
  }

  Vector2 discoverable_random_spot() const {
    IntVector2 pos = path_finder.discoverable_random_spot();
    return Vector2{static_cast<float>(pos.x * CELL_DISTANCE), static_cast<float>(pos.y * CELL_DISTANCE)};
  }
};
