#pragma once

#include "common.h"
#include "map.h"
#include "raylib.h"

#define ENEMY_SPEED 200.f
#define ENEMY_TARGET_REACH_THRESHOLD 1.f

struct Enemy {
  Vector2 pos;
  Vector2 move_target{};

  Enemy(Vector2 _pos) : pos(_pos) {
  }

  void update(Vector2 const &player_pos) {
    move_target = player_pos;

    if (Vector2Distance(pos, move_target) <= ENEMY_TARGET_REACH_THRESHOLD) {
      update_move_target(player_pos);
    } else {
      update_movement_towards_target();
    }
  }

  void draw(Map const &map) const {
    DrawCircleV(Vector2Add(pos, map.world_offset), 30, RED);
  }

 private:
  void update_move_target(Vector2 const &player_pos) {
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
