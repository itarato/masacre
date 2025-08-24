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

    reset();
  }

  void reset() {
    world_offset.x = -(map_image.width - GetScreenWidth()) / 2.f;
    world_offset.y = -(map_image.height - GetScreenHeight()) / 2.f;
  }

  void update(Vector2 player_pos) {
    update_world_offset(player_pos);

    path_finder.find_path(player_pos, Vector2Add(GetMousePosition(), world_offset));
  }

  void draw() const {
    DrawTextureV(map_texture, world_offset, WHITE);

    // for (int i = -10; i <= 10; i++) {
    //   Color color = LIGHTGRAY;
    //   if (i == 0) {
    //     color = RED;
    //   }
    //   DrawLine(i * 100 + world_offset.x, -1000 + world_offset.y, i * 100 + world_offset.x, 1000 + world_offset.y,
    //            color);
    //   DrawLine(-1000 + world_offset.x, i * 100 + world_offset.y, 1000 + world_offset.x, i * 100 + world_offset.y,
    //            color);
    // }

    for (int y = 0; y <= path_finder.cells_h; y++) {
      for (int x = 0; x <= path_finder.cells_w; x++) {
        if (path_finder.cells[y * path_finder.cells_w + x] > 0) {
          DrawCircle(x * CELL_DISTANCE + world_offset.x, y * CELL_DISTANCE + world_offset.y, 6, BROWN);
        }
      }
    }
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
        if (is_hit(Vector2{(float)(x * CELL_DISTANCE), (float)(y * CELL_DISTANCE)})) {
          path_finder.cells[y * path_finder.cells_w + x] = 0b0;
        } else {
          path_finder.cells[y * path_finder.cells_w + x] = 0b1;
        }
      }
    }
  }
};
