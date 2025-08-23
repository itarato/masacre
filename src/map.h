#pragma once

#include "common.h"
#include "path_finder.h"
#include "raylib.h"
#include "raymath.h"

struct Map {
  Vector2 world_offset{};
  Texture2D map_texture;
  Image map_image;
  PathFinder path_finder{};

  Map() {
  }
  ~Map() {
    UnloadTexture(map_texture);
    UnloadImage(map_image);
  }

  void init() {
    map_texture = LoadTexture("./assets/images/map_0.png");
    map_image = LoadImage("./assets/images/map_0.png");

    path_finder_init();
  }

  void reset() {
    world_offset.x = GetScreenWidth() / 2;
    world_offset.y = GetScreenHeight() / 2;
  }

  void update(Vector2 player_pos) {
    update_world_offset(player_pos);
  }

  void draw() const {
    Vector2 _map_corner_pos = map_corner_pos();
    DrawTextureV(map_texture, Vector2Add(_map_corner_pos, world_offset), WHITE);

    for (int i = -10; i <= 10; i++) {
      Color color = LIGHTGRAY;
      if (i == 0) {
        color = RED;
      }
      DrawLine(i * 100 + world_offset.x, -1000 + world_offset.y, i * 100 + world_offset.x, 1000 + world_offset.y,
               color);
      DrawLine(-1000 + world_offset.x, i * 100 + world_offset.y, 1000 + world_offset.x, i * 100 + world_offset.y,
               color);
    }

    TraceLog(LOG_INFO, "H=%d", path_finder.cells_h);
    for (int y = 0; y <= path_finder.cells_h; y++) {
      for (int x = 0; x <= path_finder.cells_w; x++) {
        if (path_finder.cells[y * path_finder.cells_w + x] > 0) {
          DrawCircle(x * CELL_DISTANCE + world_offset.x + _map_corner_pos.x,
                     y * CELL_DISTANCE + world_offset.y + _map_corner_pos.y, 6, BROWN);
        }
      }
    }
  }

  Vector2 map_corner_pos() const {
    return Vector2{-map_texture.width / 2.0f, -map_texture.height / 2.0f};
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

  bool is_hit_on_world_coordinate(Vector2 point) const {
    Vector2 point_abs_adjusted = Vector2Subtract(point, map_corner_pos());
    // TraceLog(LOG_INFO, "Col: %d", GetImageColor(map_image, point_abs_adjusted.x, point_abs_adjusted.y).r);
    return is_hit_on_image_absolute_coordinate(point_abs_adjusted);
  }

  bool is_hit_on_image_absolute_coordinate(Vector2 point) const {
    // TraceLog(LOG_INFO, "Col: %d", GetImageColor(map_image, point_abs_adjusted.x, point_abs_adjusted.y).r);
    return GetImageColor(map_image, point.x, point.y).r < 255;
  }

  void path_finder_init() {
    path_finder.cells_w = map_image.width / CELL_DISTANCE;
    path_finder.cells_h = map_image.height / CELL_DISTANCE;

    if (path_finder.cells_w * path_finder.cells_w >= MAX_GRID_CELLS) {
      TraceLog(LOG_ERROR, "Map too large");
      exit(EXIT_FAILURE);
    }

    for (int y = 0; y <= path_finder.cells_h; y++) {
      for (int x = 0; x <= path_finder.cells_w; x++) {
        if (is_hit_on_image_absolute_coordinate(Vector2{(float)(x * CELL_DISTANCE), (float)(y * CELL_DISTANCE)})) {
          path_finder.cells[y * path_finder.cells_w + x] = 0b0;
        } else {
          path_finder.cells[y * path_finder.cells_w + x] = 0b1;
        }
      }
    }
  }
};
