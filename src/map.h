#pragma once

#include "common.h"
#include "raylib.h"
#include "raymath.h"

struct Map {
  Vector2 world_offset{};
  Texture2D map_texture;
  Image map_image;

  Map() {}
  ~Map() {
    UnloadTexture(map_texture);
    UnloadImage(map_image);
  }

  void init() {
    map_texture = LoadTexture("./assets/images/map_0.png");
    map_image = LoadImage("./assets/images/map_0.png");
  }

  void reset() {
    world_offset.x = GetScreenWidth() / 2;
    world_offset.y = GetScreenHeight() / 2;
  }

  void update(Vector2 player_pos) {
    update_world_offset(player_pos);
  }

  void draw() const {
    Vector2 map_corner_pos{-map_texture.width / 2.0f, -map_texture.height / 2.0f};
    DrawTextureV(map_texture, Vector2Add(map_corner_pos, world_offset), WHITE);

    for (int i = -10; i <= 10; i++) {
      Color color = LIGHTGRAY;
      if (i == 0) {
        color = RED;
      }
      DrawLine(i * 100 + world_offset.x, -1000 + world_offset.y, i * 100 + world_offset.x, 1000 + world_offset.y, color);
      DrawLine(-1000 + world_offset.x, i * 100 + world_offset.y, 1000 + world_offset.x, i * 100 + world_offset.y, color);
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
};
