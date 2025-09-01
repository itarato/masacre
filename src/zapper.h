#pragma once

#include "common.h"
#include "map.h"
#include "raylib.h"

constexpr int ZAPPER_BREAKPOINTS = 4;

struct Zapper : AttackDamage {
  bool visible{false};
  Vector2 start{};
  Vector2 end{};

  Zapper() : AttackDamage() {
  }

  void draw(Map const& map) const {
    if (visible) {
      const float diffx = end.x - start.x;
      const float diffy = end.y - start.y;
      const float diffx_unit = diffx / ZAPPER_BREAKPOINTS;
      const float diffy_unit = diffy / ZAPPER_BREAKPOINTS;

      float prev_jitterx{0.f};
      float prev_jittery{0.f};
      float jitterx{0.f};
      float jittery{0.f};

      const float dist = Vector2Distance(start, end);
      const float jitter_size = (dist / ZAPPER_BREAKPOINTS) / 3.f;

      for (int i = 0; i < ZAPPER_BREAKPOINTS; i++) {
        if (i < ZAPPER_BREAKPOINTS - 1) {
          jitterx = randf_balanced(0.f, jitter_size);
          jittery = randf_balanced(0.f, jitter_size);
        } else {
          jitterx = 0.f;
          jittery = 0.f;
        }

        DrawLineEx(Vector2{start.x + map.world_offset.x + diffx_unit * i + prev_jitterx,
                           start.y + map.world_offset.y + diffy_unit * i + prev_jittery},
                   Vector2{start.x + map.world_offset.x + diffx_unit * (i + 1) + jitterx,
                           start.y + map.world_offset.y + diffy_unit * (i + 1) + jittery},
                   6, SKYBLUE);
        DrawLineEx(Vector2{start.x + map.world_offset.x + diffx_unit * i + prev_jitterx,
                           start.y + map.world_offset.y + diffy_unit * i + prev_jittery},
                   Vector2{start.x + map.world_offset.x + diffx_unit * (i + 1) + jitterx,
                           start.y + map.world_offset.y + diffy_unit * (i + 1) + jittery},
                   2, WHITE);

        prev_jitterx = jitterx;
        prev_jittery = jittery;
      }
    }
  }

  [[nodiscard]] float get_attack_damage() const override {
    return 30.f * GetFrameTime();
  }
};
