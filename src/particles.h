#pragma once

#include <cstdlib>
#include <list>
#include <memory>
#include <string>
#include <utility>

#include "map.h"
#include "raylib.h"

#define PARTICLE_SMOKE_SPEED 80.f

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
  TimedTask lifetime;
  float radius{2.0f};
  float jitter;
  float alpha{0.2f};

  explicit SmokeParticle(Vector2 const _pos) : Particle(_pos), lifetime(1.6) {
    jitter = static_cast<float>(rand() % 628) / 100.f;
  }
  ~SmokeParticle() override = default;

  void draw(Map const &map) const override {
    DrawCircleV(Vector2Add(pos, map.world_offset), radius, ColorAlpha(DARKGRAY, alpha));
  };
  void update() override {
    pos.y -= PARTICLE_SMOKE_SPEED * GetFrameTime();
    pos.x += sinf(GetTime() * 10.f + jitter) * 0.3f;
    radius += GetFrameTime() * 10.f;
    alpha -= GetFrameTime() * 0.1f;

    if (lifetime.is_completed()) is_dead = true;
  };
};

struct Explosion final : Particle {
  Vector2 v{};
  TimedTask lifetime;

  Explosion(Vector2 _pos, Vector2 _v) : Particle(_pos), v(_v), lifetime(2.0) {
  }

  void draw(Map const &map) const override {
    DrawCircleV(Vector2Add(pos, map.world_offset), 10.f, GOLD);
  };
  void update() override {
    pos.x += v.x;
    pos.y += v.y;

    v.x *= powf(0.99f, static_cast<float>(REFERENCE_FPS) / static_cast<float>(GetFPS()));
    v.y *= powf(0.99f, static_cast<float>(REFERENCE_FPS) / static_cast<float>(GetFPS()));

    if (lifetime.is_completed()) is_dead = true;
  }
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
