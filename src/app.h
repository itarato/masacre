#pragma once

#include <cstdlib>
#include <ctime>
#include <vector>

#include "asset_manager.h"
#include "collectibles.h"
#include "common.h"
#include "enemy.h"
#include "map.h"
#include "particles.h"
#include "player.h"
#include "raylib.h"

#define ENEMY_SPAWN_COUNT 6
#define MAX_COLLECTIBLE_HEALTH_COUNT 1
#define MAX_COLLECTIBLE_BULLET_COUNT 5

struct App {
  Player player{};
  Map map{};
  std::vector<Enemy> enemies{};
  std::vector<Collectible> collectibles{};

  void init() {
    srand(time(nullptr));

    // SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_W, WINDOW_H, "Masacre");
    SetTargetFPS(GetMonitorRefreshRate(0));

    asset_manager.init();
    player.init();
    map.init();

    reset();
  }

  void reset() {
    map.reset();
    player.reset(map);
    enemies.clear();
    collectibles.clear();
    game_scope.reset();
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
    if (IsKeyPressed(KEY_R) || IsGamepadButtonPressed(0, 5)) reset();

    player.update(map);

    for (auto& enemy : enemies) enemy.update(*player.pos, map);

    map.update(*player.pos);

    game_scope.update(map, player);

    // Collision checks.
    for (auto& enemy : enemies) {
      for (auto& bullet : player.bullets) {
        if (CheckCollisionPointCircle(bullet.pos, enemy.pos, enemy.circle_frame_radius)) {
          enemy.kill();
          bullet.kill();
          player.kill_count++;
        }
      }

      if (CheckCollisionCircles(*player.pos, player.circle_frame_radius, enemy.pos, enemy.circle_frame_radius)) {
        player.hurt(enemy.attack_damage());
      }
    }

    for (auto& collectible : collectibles) {
      if (CheckCollisionCircles(collectible.pos, collectible.circle_frame_radius, *player.pos,
                                player.circle_frame_radius)) {
        collectible.should_be_deleted = true;
        player.consume(collectible);
      }
    }

    std::erase_if(enemies, [](const auto& e) { return e.should_be_deleted(); });
    std::erase_if(collectibles, [](const auto& e) { return e.should_be_deleted; });

    if (enemies.empty()) {
      for (int i = 0; i < ENEMY_SPAWN_COUNT; i++) enemies.emplace_back(map.available_random_spot());
    }

    if (collectible_count_of_type(CollectibleType::Health) < MAX_COLLECTIBLE_HEALTH_COUNT) {
      collectibles.emplace_back(map.available_random_spot(), CollectibleType::Health);
    }
    if (collectible_count_of_type(CollectibleType::Bullet) < MAX_COLLECTIBLE_BULLET_COUNT) {
      collectibles.emplace_back(map.available_random_spot(), CollectibleType::Bullet);
    }
  }

  void draw() const {
    map.draw();

    for (auto const& enemy : enemies) enemy.draw(map, *player.pos);
    for (auto const& collectible : collectibles) collectible.draw(map);

    game_scope.draw(map);

    player.draw(map);
  }

 private:
  int collectible_count_of_type(CollectibleType ty) const {
    int count{0};
    for (auto const& collectible : collectibles) {
      if (collectible.ty == ty) count++;
    }
    return count;
  }
};
