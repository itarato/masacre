#pragma once

#include <cstdlib>
#include <list>
#include <memory>
#include <string>
#include <utility>

#include "map.h"
#include "raylib.h"

#define PARTICLE_SMOKE_SPEED 40.f

struct Particle {
  Vector2 pos{};
  bool is_dead{false};

  explicit Particle(Vector2 _pos) : pos(_pos) {
  }
  virtual ~Particle() {};

  virtual void draw(Map const &map) const = 0;
  virtual void update() = 0;
};

struct SmokeParticle final : Particle {
  double lifetime_end{};
  float radius{2.0f};

  explicit SmokeParticle(Vector2 const _pos) : Particle(_pos), lifetime_end(GetTime() + 2.0) {
  }
  ~SmokeParticle() override = default;

  void draw(Map const &map) const override {
    DrawCircleV(Vector2Add(pos, map.world_offset), radius, ColorAlpha(DARKGRAY, 0.1f));
  };
  void update() override {
    pos.y -= PARTICLE_SMOKE_SPEED * GetFrameTime();
    pos.x += sinf(GetTime() * 10.f) * 0.3f;
    radius += GetFrameTime() * 10.f;

    if (GetTime() > lifetime_end) is_dead = true;
  };
};

struct ParticleManager {
  std::list<std::unique_ptr<Particle>> particles{};

  void update() {
    for (auto &particle : particles) particle->update();
    std::erase_if(particles, [](auto const &e) { return e->is_dead; });
  }

  void draw(Map const &map) const {
    for (auto const &particle : particles) particle->draw(map);
  }

  void reset() {
    particles.clear();
  }
};
