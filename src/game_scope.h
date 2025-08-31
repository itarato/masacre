#pragma once

#include <algorithm>
#include <list>

#include "bullet.h"
#include "map.h"
#include "particles.h"
#include "player.h"
#include "raylib.h"

struct GameScope {
  std::list<Bullet> enemy_bullets{};
  ParticleManager particle_manager{};

  void reset() {
    enemy_bullets.clear();
    particle_manager.reset();
  }

  void update(Map const &map, Player &player) {
    for (auto &bullet : enemy_bullets) bullet.update(map);
    std::erase_if(enemy_bullets, [](auto e) { return e.should_be_deleted; });

    for (auto &bullet : enemy_bullets) {
      if (CheckCollisionPointCircle(bullet.pos, *player.pos, player.circle_frame_radius)) {
        bullet.kill();
        player.hurt(bullet.attack_damage());
      }
    }

    particle_manager.update();
  }

  void draw(Map const &map) const {
    for (auto const &bullet : enemy_bullets) bullet.draw(map);
    particle_manager.draw(map);
  }
};

static GameScope game_scope{};
