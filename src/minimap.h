#pragma once

#include <raylib.h>

#include <list>

#include "enemy.h"
#include "map.h"
#include "player.h"

#define REL_POS(full, absolute) (absolute * MINIMAP_SIZE / full)

constexpr int MINIMAP_SIZE = 100;
constexpr int MINIMAP_PIXEL_SIZE = 2;

struct Minimap {
  void draw(Map const& map, Player const& player, std::list<Enemy> const& enemies) const {
    const float w{static_cast<float>(map.width())};
    const float h{static_cast<float>(map.height())};

    DrawRectangle(4, 4, MINIMAP_SIZE + MINIMAP_PIXEL_SIZE - 1, MINIMAP_SIZE + MINIMAP_PIXEL_SIZE - 1,
                  ColorAlpha(DARKGRAY, 0.8f));

    DrawRectangle(REL_POS(w, player.pos->x) + 4, REL_POS(h, player.pos->y) + 4, MINIMAP_PIXEL_SIZE, MINIMAP_PIXEL_SIZE,
                  WHITE);

    for (auto const& enemy : enemies) {
      DrawRectangle(REL_POS(w, enemy.pos.x) + 4, REL_POS(h, enemy.pos.y) + 4, MINIMAP_PIXEL_SIZE, MINIMAP_PIXEL_SIZE,
                    RED);
    }
  }
};
