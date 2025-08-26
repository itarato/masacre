#pragma once

#include <cstdlib>
#include <list>
#include <string>

#include "map.h"
#include "raylib.h"

#define PARTICLE_SMOKE_SPEED 10.f

struct Particle {
  Vector2 pos{};
  bool is_dead{false};

  explicit Particle(Vector2 _pos) : pos(_pos) {
  }
  virtual ~Particle() = delete;

  virtual void draw(Map const &map) const;
  virtual void update();
};

struct SmokeParticle final : Particle {
  float lifetime_secs{2.f};

  explicit SmokeParticle(Vector2 const _pos) : Particle(_pos) {
  }
  ~SmokeParticle() override {};

  void draw(Map const &map) const override {
    DrawCircleV(pos, 4.f, ColorAlpha(DARKGRAY, 1.f));
  };
  void update() override {
    pos.y -= PARTICLE_SMOKE_SPEED * GetFrameTime();
  };
};

struct ParticleManager {
  std::list<Particle> particles{};

  void update() {
    for (auto &particle : particles) particle.update();
    std::erase_if(particles, [](auto const &e) { return e.is_dead; });
  }

  void draw(Map const &map) const {
    for (auto const &particle : particles) particle.draw(map);
  }
};
