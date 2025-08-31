#pragma once

#include <cstdlib>
#include <ctime>
#include <list>

#include "asset_manager.h"
#include "collectibles.h"
#include "common.h"
#include "enemy.h"
#include "intrinsic.h"
#include "map.h"
#include "minimap.h"
#include "path_finder.h"
#include "player.h"
#include "raylib.h"

constexpr int ENEMY_SPAWN_COUNT = 0;
constexpr int MAX_COLLECTIBLE_HEALTH_COUNT = 1;
constexpr int MAX_COLLECTIBLE_BULLET_COUNT = 5;
constexpr int MAX_COLLECTIBLE_MINE_COUNT = 3;
constexpr int ENEMY_JAM_CONTROL_CLOSE = CELL_DISTANCE;
constexpr float ENEMY_JAM_CONTROL_TOO_CLOSE = CELL_DISTANCE / 2.f;

struct App {
  Player player{};
  Map map{};
  std::list<Enemy> enemies{};
  std::list<Collectible> collectibles{};
  PerfChart perf_chart{};
  PathFinder path_finder{};
  Minimap minimap{};

  void init() {
    srand(time(nullptr));

    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_W, WINDOW_H, "Masacre");
    InitAudioDevice();
    SetTargetFPS(GetMonitorRefreshRate(0));
    // SetTargetFPS(60);

    asset_manager.init();
    player.init();
    map.init();
    path_finder.init(map);

    reset();
  }

  void reset() {
    map.reset();
    player.reset(path_finder);
    enemies.clear();
    collectibles.clear();
    game_scope.reset();
  }

  void run() {
    while (!WindowShouldClose()) {
      perf_chart.register_active_frame_start();
      update();

      BeginDrawing();
      ClearBackground(BLACK);

      draw();
      perf_chart.register_active_frame_end();

      EndDrawing();
    }
  }

  void update() {
    if (IsKeyPressed(KEY_R) || IsGamepadButtonPressed(0, 5)) reset();

    player.update(map);

    for (auto& enemy : enemies) enemy.update(*player.pos, map, path_finder);

    map.update(*player.pos);

    game_scope.update(map, player);

    update_enemy_collision_checks();
    update_collectible_collisions();
    update_enemy_jam_control();

    // Delete disposables.
    std::erase_if(enemies, [](const auto& e) { return e.should_be_deleted(); });
    std::erase_if(collectibles, [](const auto& e) { return e.should_be_deleted; });

    if (enemies.empty()) {
      for (int i = 0; i < ENEMY_SPAWN_COUNT; i++) enemies.emplace_back(discoverable_random_spot());
    }

    if (collectible_count_of_type(CollectibleType::Health) < MAX_COLLECTIBLE_HEALTH_COUNT) {
      collectibles.emplace_back(discoverable_random_spot(), CollectibleType::Health);
    }
    if (collectible_count_of_type(CollectibleType::Bullet) < MAX_COLLECTIBLE_BULLET_COUNT) {
      collectibles.emplace_back(discoverable_random_spot(), CollectibleType::Bullet);
    }
    if (collectible_count_of_type((CollectibleType::Mine)) < MAX_COLLECTIBLE_MINE_COUNT) {
      collectibles.emplace_back(discoverable_random_spot(), CollectibleType::Mine);
    }

    perf_chart.update();
  }

  void draw() const {
    map.draw();
    for (auto const& enemy : enemies) enemy.draw(map, *player.pos);
    for (auto const& collectible : collectibles) collectible.draw(map);
    game_scope.draw(map);
    player.draw(map);
    perf_chart.draw();
    path_finder.draw(map);
    minimap.draw(map, player, enemies);
    // draw_debug_path_finding(*player.pos);
  }

 private:
  Vector2 discoverable_random_spot() const {
    return int_vector2_to_vector2(path_finder.discoverable_random_spot());
  }

  int collectible_count_of_type(CollectibleType ty) const {
    int count{0};
    for (auto const& collectible : collectibles) {
      if (collectible.ty == ty) count++;
    }
    return count;
  }

  void draw_debug_path_finding(Vector2 const& player_pos) const {
    auto path = path_finder.find_path(Vector2Subtract(GetMousePosition(), map.world_offset), player_pos);
    if (path.empty()) return;

    for (unsigned int i = 0; i < path.size() - 1; i++) {
      DrawLineEx(Vector2Add(int_vector2_to_vector2(path[i]), map.world_offset),
                 Vector2Add(int_vector2_to_vector2(path[i + 1]), map.world_offset), 3.f, MAROON);
    }
  }

  void update_enemy_collision_checks() {
    for (auto& enemy : enemies) {
      if (enemy.is_dead) continue;

      for (auto& bullet : player.bullets) {
        if (CheckCollisionPointCircle(bullet.pos, enemy.pos, enemy.circle_frame_radius)) {
          bullet.kill();
          enemy.kill();
          player.kill_count++;
        }
      }

      for (auto& mine : player.mines) {
        if (CheckCollisionCircles(mine.pos, mine.circle_frame_radius, enemy.pos, enemy.circle_frame_radius)) {
          mine.kill();

          for (auto& _enemy : enemies) {
            if (CheckCollisionCircles(mine.pos, mine.blast_radius(), _enemy.pos, _enemy.circle_frame_radius)) {
              _enemy.kill();
            }
          }
        }
      }

      if (CheckCollisionCircles(*player.pos, player.circle_frame_radius, enemy.pos, enemy.circle_frame_radius)) {
        player.hurt(enemy.attack_damage());
      }
    }
  }

  void update_collectible_collisions() {
    for (auto& collectible : collectibles) {
      if (CheckCollisionCircles(collectible.pos, collectible.circle_frame_radius, *player.pos,
                                player.circle_frame_radius)) {
        collectible.should_be_deleted = true;
        player.consume(collectible);

        PlaySound(asset_manager.sounds[ASSET_SOUND_PICKUP]);
      }
    }
  }

  void update_enemy_jam_control() {
    for (auto& enemy : enemies) enemy.collision_avoidance_slowdown = 1.f;
    for (auto& enemy_lhs : enemies) {
      for (auto& enemy_rhs : enemies) {
        if (enemy_lhs.is_dead || enemy_rhs.is_dead) continue;
        if (enemy_lhs.object_id == enemy_rhs.object_id) continue;

        if (Vector2Equals(enemy_lhs.pos, enemy_rhs.pos)) {
          if (enemy_lhs.object_id < enemy_rhs.object_id) {
            enemy_lhs.collision_avoidance_slowdown *= 0.f;
          } else {
            enemy_rhs.collision_avoidance_slowdown = 0.f;
          }
        } else if (Vector2Distance(enemy_lhs.pos, enemy_rhs.pos) < ENEMY_JAM_CONTROL_TOO_CLOSE) {
          if (Vector2Distance(enemy_lhs.pos, *player.pos) < Vector2Distance(enemy_rhs.pos, *player.pos)) {
            enemy_rhs.collision_avoidance_slowdown = 0.f;
          } else {
            enemy_lhs.collision_avoidance_slowdown *= 0.f;
          }
        } else if (Vector2Distance(enemy_lhs.pos, enemy_rhs.pos) < ENEMY_JAM_CONTROL_CLOSE) {
          if (Vector2Distance(enemy_lhs.pos, *player.pos) < Vector2Distance(enemy_rhs.pos, *player.pos)) {
            enemy_rhs.collision_avoidance_slowdown *= 0.9f;
          } else {
            enemy_lhs.collision_avoidance_slowdown *= 0.9f;
          }
        }
      }
    }
  }
};
