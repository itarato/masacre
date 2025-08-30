#pragma once

#include <cstdlib>
#include <list>
#include <memory>
#include <string>
#include <utility>

#include "map.h"
#include "raylib.h"

constexpr float PARTICLE_SMOKE_SPEED = 80.f;

struct Particle {
  Vector2 pos{};
  bool should_be_deleted{false};

  Particle() = default;
  explicit Particle(Vector2 _pos) : pos(_pos) {
  }
  virtual ~Particle() = default;

  virtual void draw(Map const &map) const = 0;
  virtual void update() = 0;
};

struct ParticleManager {
  std::list<std::unique_ptr<Particle>> particles{};

  void update() {
    for (auto &particle : particles) particle->update();
    std::erase_if(particles, [](auto const &e) { return e->should_be_deleted; });
  }

  void draw(Map const &map) const {
    for (auto const &particle : particles) particle->draw(map);
  }

  void reset() {
    particles.clear();
  }
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

    if (lifetime.is_completed()) should_be_deleted = true;
  };
};

struct ExplosionParticle final : Particle {
  Vector2 v{};
  TimedTask lifetime;
  float size_jitter;

  ExplosionParticle(Vector2 _pos, Vector2 _v) : Particle(_pos), v(_v), lifetime(1.5) {
    size_jitter = static_cast<float>(rand() % 100) / 200.f + 0.75f;
  }

  void draw(Map const &map) const override {
    DrawCircleV(Vector2Add(pos, map.world_offset), 6.f * size_jitter, GOLD);
  };

  void update() override {
    pos.x += v.x;
    pos.y += v.y;

    fps_independent_multiply(&v.x, 0.99f);
    fps_independent_multiply(&v.y, 0.99f);

    if (lifetime.is_completed()) should_be_deleted = true;
  }
};

struct StraightLineParticle final : Particle {
  float angle_rad{};
  float speed{};
  TimedTask lifetime;
  float size{6.f};
  float speed_multiplier{1.0};
  Color color{GOLD};
  float pos_jitter{40.f};

  StraightLineParticle(Vector2 _pos, float _angle_rad, float _speed, double _lifetime)
      : Particle(_pos), angle_rad(_angle_rad), speed(_speed), lifetime(_lifetime) {
    pos.x += randf() * pos_jitter - pos_jitter / 2.f;
    pos.y += randf() * pos_jitter - pos_jitter / 2.f;
  }

  void draw(Map const &map) const override {
    DrawCircleV(Vector2Add(pos, map.world_offset), size, color);
  };

  void update() override {
    pos.x += cosf(angle_rad) * speed * GetFrameTime();
    pos.y += sinf(angle_rad) * speed * GetFrameTime();

    if (speed_multiplier < 1.0f) fps_independent_multiply(&speed, speed_multiplier);

    if (lifetime.is_completed()) should_be_deleted = true;
  }
};

struct TraceParticle final : Particle {
  float angle_deg;
  float alpha{0.3};

  TimedTask lifetime{1.0};

  TraceParticle(Vector2 _pos, float _angle_deg) : Particle(_pos), angle_deg(_angle_deg) {
  }

  void draw(Map const &map) const override {
    DrawRectanglePro(Rectangle{pos.x + map.world_offset.x, pos.y + map.world_offset.y, 10.f, 10.f},
                     Vector2{20.f, -20.f}, angle_deg, ColorAlpha(DARKBROWN, alpha));

    DrawRectanglePro(Rectangle{pos.x + map.world_offset.x, pos.y + map.world_offset.y, 10.f, 10.f},
                     Vector2{-10.f, -20.f}, angle_deg, ColorAlpha(DARKBROWN, alpha));
  }

  void update() override {
    if (lifetime.is_completed()) should_be_deleted = true;
    alpha -= GetFrameTime() * 0.4f;
  }
};

struct BurnParticleGroup final : Particle {
  std::shared_ptr<Vector2> player_pos;
  TimedTask lifetime{1.0};
  RepeatedTask particle_repeater{0.02};
  ParticleManager particle_manager{};

  explicit BurnParticleGroup(std::shared_ptr<Vector2> _player_pos) : Particle(), player_pos(std::move(_player_pos)) {
  }

  void update() override {
    if (lifetime.is_completed()) should_be_deleted = true;

    particle_manager.update();
    particle_repeater.update();

    if (particle_repeater.did_tick) {
      float angle_rad = (230 + rand() % 80) * DEG2RAD;
      auto particle = std::make_unique<StraightLineParticle>(*player_pos, angle_rad, 400.f, 0.5);
      particle->speed_multiplier = 0.95f;
      particle->size = 2.f;
      switch (rand() % 5) {
        case 0:
          particle->color = YELLOW;
          break;
        case 1:
          particle->color = ORANGE;
          break;
        case 2:
          particle->color = DARKGRAY;
          break;
        default:
          particle->color = RED;
          break;
      }
      particle_manager.particles.push_back(std::move(particle));
    }
  }

  void draw(Map const &map) const override {
    particle_manager.draw(map);
  }
};
