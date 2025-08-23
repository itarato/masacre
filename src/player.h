#pragma once

#include <algorithm>
#include <vector>

#include "bullet.h"
#include "common.h"
#include "map.h"
#include "raylib.h"
#include "raymath.h"

struct Player {
  Vector2 pos{};
  Vector2 frame{};
  float angle;
  Texture2D player_body_texture;
  std::vector<Bullet> bullets{};

  Player() {
  }

  ~Player() {
    UnloadTexture(player_body_texture);
  }

  void init() {
    player_body_texture = LoadTexture("./assets/images/player_body.png");
  }

  void reset() {
    frame = Vector2(player_body_texture.width, player_body_texture.height);
    angle = 0.f;
    bullets.clear();
    pos.x = 0.f;
    pos.y = 0.f;
  }

  void update(Map const &map) {
    Vector2 old_pos = pos;

    if (IsKeyDown(KEY_UP)) move_to_relative_direction(0.f);
    if (IsKeyDown(KEY_DOWN)) move_to_relative_direction(PI);

    if (IsKeyDown(KEY_A)) move_to_relative_direction(-PI / 2.f);
    if (IsKeyDown(KEY_D)) move_to_relative_direction(PI / 2.f);

    if (map.is_hit(pos)) {
      pos = old_pos;
    }

    if (IsKeyDown(KEY_LEFT)) angle -= PLAYER_ANGLE_SPEED * GetFrameTime();
    if (IsKeyDown(KEY_RIGHT)) angle += PLAYER_ANGLE_SPEED * GetFrameTime();

    if (IsKeyPressed(KEY_LEFT_CONTROL)) {
      float bullet_angle = angle * DEG2RAD - PI / 2.f;
      Vector2 bullet_v{cosf(bullet_angle) * BULLET_SPEED * GetFrameTime(),
                       sinf(bullet_angle) * BULLET_SPEED * GetFrameTime()};
      bullets.emplace_back(pos, bullet_v);
    }

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [&map](const auto &e) { return e.is_dead(map.world_offset); }),
                  bullets.end());

    for (auto &bullet : bullets) bullet.update();
  }

  void draw(Map const &map) const {
    Vector2 rel_pos = screen_relative_center(map.world_offset);
    DrawTexturePro(
            player_body_texture,
            {0.f, 0.f, (float) player_body_texture.width, (float) player_body_texture.height},
            {rel_pos.x, rel_pos.y, (float) player_body_texture.width, (float) player_body_texture.height},
            Vector2Divide(frame, {2.f, 2.f}),
            angle,
            WHITE);

    for (auto const &bullet : bullets) bullet.draw(map.world_offset);

    DrawText(TextFormat("Player: %.2f:%.2f", pos.x, pos.y), 10, 70, 20, ORANGE);
    DrawText(map.is_hit(pos) ? "hit" : "miss", 10, 30, 20, ORANGE);
  }

  Vector2 screen_relative_center(Vector2 world_offset) const {
    return Vector2Add(pos, world_offset);
  }

  void move_to_relative_direction(float rel_rad_angle) {
    float abs_angle = (angle * DEG2RAD) + rel_rad_angle - PI / 2.f;
    pos.x += cosf(abs_angle) * PLAYER_SPEED * GetFrameTime();
    pos.y += sinf(abs_angle) * PLAYER_SPEED * GetFrameTime();
  }
};
