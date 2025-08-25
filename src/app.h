#pragma once

#include <cstdlib>
#include <ctime>
#include <vector>

#include "common.h"
#include "enemy.h"
#include "map.h"
#include "player.h"
#include "raylib.h"
#define ENEMY_SPAWN_COUNT 5

struct App {
  Player player{};
  Map map{};
  std::vector<Enemy> enemies{};

  App() {
  }
  ~App() {
  }

  void init() {
    srand(time(NULL));

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
    if (IsKeyPressed(KEY_R)) reset();

    player.update(map);
    for (auto& enemy : enemies) enemy.update(player.pos, map);
    map.update(player.pos);

    for (auto& enemy : enemies) {
      for (auto& bullet : player.bullets) {
        if (CheckCollisionPointRec(bullet.pos, enemy.frame())) {
          enemy.kill();
          bullet.kill();
        }
      }
    }

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const auto& e) { return e.is_dead; }),
                  enemies.end());

    if (enemies.empty()) {
      for (int i = 0; i < ENEMY_SPAWN_COUNT; i++) {
        while (true) {
          Vector2 new_enemy_pos{(float)(rand() % map.map_image.width), (float)(rand() % map.map_image.height)};

          if (!map.is_hit(new_enemy_pos)) {
            enemies.emplace_back(new_enemy_pos);
            break;
          };
        }
      }
    }
  }

  void draw() const {
    map.draw(player.pos);

    for (auto const& enemy : enemies) enemy.draw(map);

    player.draw(map);

    DrawFPS(10, 10);
  }
};
