#pragma once

#include "asset_manager.h"
#include "common.h"
#include "map.h"
#include "raylib.h"

#define ENEMY_SPEED 200.f
#define ENEMY_TARGET_REACH_THRESHOLD 1.f

struct Enemy {
  Vector2 pos;
  Vector2 move_target{};
  bool is_dead{false};
  float circle_frame_radius{};
  float angle{};

  Enemy(Vector2 _pos) : pos(_pos), move_target(_pos) {
    circle_frame_radius = asset_manager.textures[ASSET_ENEMY_TEXTURE].width / 2.f;
  }

  ~Enemy() {
  }

  void update(Vector2 const &player_pos, Map const &map) {
    if (Vector2Distance(pos, move_target) <= ENEMY_TARGET_REACH_THRESHOLD) {
      update_move_target(player_pos, map);
    } else {
      update_movement_towards_target();
    }
  }

  void draw(Map const &map) const {
    draw_texture(asset_manager.textures[ASSET_ENEMY_TEXTURE], Vector2Add(pos, map.world_offset), angle);
  }

  void kill() {
    is_dead = true;
  }

  float attack_damage() const {
    return 10;
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

    Vector2 old_pos{pos};

    if (move_dist >= total_dist) {
      pos = move_target;
    } else {
      Vector2 delta_adjusted_to_move_dist = Vector2Divide(delta, {total_dist / move_dist, total_dist / move_dist});
      pos = Vector2Subtract(pos, delta_adjusted_to_move_dist);
    }

    angle = abs_angle_of_points(old_pos, pos) * RAD2DEG;
  }
};
