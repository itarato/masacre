#pragma once

#include "common.h"
#include "player.h"
#include "raylib.h"

struct App {
  Player player{};
  Vector2 world_offset{};

  App() {}
  ~App() {}

  void init() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_W, WINDOW_H, "Masacre");
    SetTargetFPS(GetMonitorRefreshRate(0));

    player.init();

    reset();
  }

  void reset() {
    world_offset.x = GetScreenWidth() / 2;
    world_offset.y = GetScreenHeight() / 2;

    player.reset();
  }

  void run() {
    while (!WindowShouldClose()) {
      BeginDrawing();
      ClearBackground(RAYWHITE);

      update();
      draw();

      EndDrawing();
    }
  }

  void update() {
    player.update(world_offset);
    update_world_offset();
  }

  void draw() const {
    player.draw(world_offset);

    for (int i = -10; i <= 10; i++) {
      Color color = LIGHTGRAY;
      if (i == 0) {
        color = RED;
      }
      DrawLine(i * 100 + world_offset.x, -1000 + world_offset.y, i * 100 + world_offset.x, 1000 + world_offset.y, color);
      DrawLine(-1000 + world_offset.x, i * 100 + world_offset.y, 1000 + world_offset.x, i * 100 + world_offset.y, color);
    }

    DrawFPS(10, 10);
  }

  void update_world_offset() {
    Vector2 player_rel_pos = player.screen_relative_center(world_offset);

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
