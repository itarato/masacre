#pragma once

#include "common.h"
#include "map.h"
#include "player.h"
#include "raylib.h"

struct App {
  Player player{};
  Map map{};

  App() {}
  ~App() {
  }

  void init() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_W, WINDOW_H, "Masacre");
    SetTargetFPS(GetMonitorRefreshRate(0));


    player.init();
    map.init();

    reset();
  }

  void reset() {
    map.reset();
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
    player.update(map);
    map.update(player.pos);
  }

  void draw() const {
    map.draw();
    player.draw(map);

    DrawFPS(10, 10);
  }
};
