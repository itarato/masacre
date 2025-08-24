#pragma once

#include <vector>

#include "common.h"
#include "enemy.h"
#include "map.h"
#include "player.h"
#include "raylib.h"

struct App {
  Player player{};
  Map map{};
  std::vector<Enemy> enemies{};

  App() {
  }
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
    player.reset(map);

    enemies.clear();
    enemies.emplace_back();
  }

  void run() {
    while (!WindowShouldClose()) {
      update();

      BeginDrawing();
      ClearBackground(RAYWHITE);

      draw();

      EndDrawing();
    }
  }

  void update() {
    player.update(map);
    for (auto& enemy : enemies) enemy.update();
    map.update(player.pos);
  }

  void draw() const {
    map.draw(player.pos);

    for (auto const& enemy : enemies) enemy.draw();

    player.draw(map);

    DrawFPS(10, 10);
  }
};
