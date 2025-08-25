#pragma once

#include <cstdlib>
#include <ctime>
#include <vector>

#include "asset_manager.h"
#include "collectibles.h"
#include "common.h"
#include "enemy.h"
#include "map.h"
#include "player.h"
#include "raylib.h"

#define ENEMY_SPAWN_COUNT 5
#define MAX_COLLECTIBLE_HEALTH_COUNT 2
#define MAX_COLLECTIBLE_BULLET_COUNT 4

struct App {
  Player player{};
  Map map{};
  std::vector<Enemy> enemies{};
  std::vector<Collectible> collectibles{};

  App() {
  }
  ~App() {
  }

  void init() {
    srand(time(NULL));

    // SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_W, WINDOW_H, "Masacre");
    SetTargetFPS(GetMonitorRefreshRate(0));

    player.init();
    map.init();
    asset_manager.init();

    reset();
  }

  void reset() {
    map.reset();
    player.reset(map);
    enemies.clear();
    collectibles.clear();
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

    // Collision checks.
    for (auto& enemy : enemies) {
      for (auto& bullet : player.bullets) {
        if (CheckCollisionPointCircle(bullet.pos, enemy.pos, enemy.circle_frame_radius)) {
          enemy.kill();
          bullet.kill();
          player.kill_count++;
        }
      }

      if (CheckCollisionCircles(player.pos, player.circle_frame_radius, enemy.pos, enemy.circle_frame_radius)) {
        player.hurt();
      }
    }

    for (auto& collectible : collectibles) {
      if (CheckCollisionCircles(collectible.pos, collectible.circle_frame_radius, player.pos,
                                player.circle_frame_radius)) {
        collectible.is_dead = true;
        player.consume(collectible);
      }
    }

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const auto& e) { return e.is_dead; }),
                  enemies.end());
    collectibles.erase(
        std::remove_if(collectibles.begin(), collectibles.end(), [](const auto& e) { return e.is_dead; }),
        collectibles.end());

    if (enemies.empty()) {
      for (int i = 0; i < ENEMY_SPAWN_COUNT; i++) enemies.emplace_back(available_random_spot());
    }

    if (collectible_count_of_type(CollectibleType::Health) < MAX_COLLECTIBLE_HEALTH_COUNT) {
      collectibles.emplace_back(available_random_spot(), CollectibleType::Health);
    }
    if (collectible_count_of_type(CollectibleType::Bullet) < MAX_COLLECTIBLE_BULLET_COUNT) {
      collectibles.emplace_back(available_random_spot(), CollectibleType::Bullet);
    }
  }

  void draw() const {
    map.draw(player.pos);

    for (auto const& enemy : enemies) enemy.draw(map);
    for (auto const& collectible : collectibles) collectible.draw(map);

    player.draw(map);

    DrawFPS(10, 10);
  }

 private:
  int collectible_count_of_type(CollectibleType ty) const {
    int count{0};
    for (auto const& collectible : collectibles) {
      if (collectible.ty == ty) count++;
    }
    return count;
  }

  Vector2 available_random_spot() const {
    Vector2 pos;

    while (true) {
      pos = Vector2{(float)(rand() % map.map_image.width), (float)(rand() % map.map_image.height)};

      if (!map.is_hit(pos)) return pos;
    }
  }
};
