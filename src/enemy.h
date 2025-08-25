#pragma once

#include "common.h"
#include "map.h"
#include "raylib.h"

#define ENEMY_SPEED 200.f
#define ENEMY_TARGET_REACH_THRESHOLD 1.f
#define ENEMY_SIZE 40.f

struct Enemy {
  Vector2 pos;
  Vector2 move_target{};
  bool is_dead{false};

  Enemy(Vector2 _pos) : pos(_pos), move_target(_pos) {
  }

  void update(Vector2 const &player_pos, Map const &map) {
    if (Vector2Distance(pos, move_target) <= ENEMY_TARGET_REACH_THRESHOLD) {
      update_move_target(player_pos, map);
    } else {
      update_movement_towards_target();
    }
  }

  void draw(Map const &map) const {
    DrawCircleV(Vector2Add(pos, map.world_offset), (ENEMY_SIZE / 2.f), RED);
  }

  Rectangle frame() const {
    return Rectangle(pos.x - (ENEMY_SIZE / 2.f), pos.y - (ENEMY_SIZE / 2.f), ENEMY_SIZE, ENEMY_SIZE);
  }

  void kill() {
    is_dead = true;
  }

 private:
  void update_move_target(Vector2 const &player_pos, Map const &map) {
    auto path = map.path_finder.find_path(pos, player_pos);
    if (path.empty()) {
      TraceLog(LOG_INFO, "No path from enemy to player");
      return;
    }

    // Likely enemy is already at the zone of player.
    if (path.size() <= 1) {
      move_target = player_pos;
      return;
    };

    move_target = Vector2(path[path.size() - 2].x * CELL_DISTANCE, path[path.size() - 2].y * CELL_DISTANCE);
  }

  void update_movement_towards_target() {
    Vector2 delta = Vector2Subtract(pos, move_target);
    float total_dist = Vector2Distance(pos, move_target);
    float move_dist = ENEMY_SPEED * GetFrameTime();

    if (move_dist >= total_dist) {
      pos = move_target;
    } else {
      Vector2 delta_adjusted_to_move_dist = Vector2Divide(delta, {total_dist / move_dist, total_dist / move_dist});
      pos = Vector2Subtract(pos, delta_adjusted_to_move_dist);
    }
  }
};
