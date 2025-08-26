#pragma once

#include <algorithm>
#include <list>
#include <unordered_map>

#include "bullet.h"
#include "map.h"

struct GameScope {
  std::list<Bullet> enemy_bullets{};

  void reset() {
    enemy_bullets.clear();
  }

  void update(Map const &map) {
    for (auto &bullet : enemy_bullets) bullet.update(map);
    std::erase_if(enemy_bullets, [](auto e) { return e.should_be_deleted; });
  }

  void draw(Map const &map) const {
    for (auto const &bullet : enemy_bullets) bullet.draw(map);
  }
};

static GameScope game_scope{};
