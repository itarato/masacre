#pragma once

#include <string>
#include <unordered_map>

#include "common.h"
#include "raylib.h"

#define ASSET_PLAYER_TEXTURE 0
#define ASSET_MAP_TEXTURE 1
#define ASSET_ENEMY_WHEEL_TEXTURE 2
#define ASSET_COLLECTIBLE_HEALTH_TEXTURE 3
#define ASSET_COLLECTIBLE_BULLET_TEXTURE 4
#define ASSET_ICON_HEALTH_TEXTURE 5
#define ASSET_ICON_BULLET_TEXTURE 6
#define ASSET_ICON_FPS_TEXTURE 7
#define ASSET_ENEMY_BARREL_TEXTURE 8
#define ASSET_BULLET_TEXTURE 9
#define ASSET_ICON_KILLS_TEXTURE 10

#define ASSET_MAP_IMAGE 0

struct AssetManager {
  std::unordered_map<int, Texture2D> textures{};
  std::unordered_map<int, Image> images{};

  ~AssetManager() {
    for (auto const& kv : textures) UnloadTexture(kv.second);
    for (auto const& kv : images) UnloadImage(kv.second);
  }

  void init() {
    textures[ASSET_PLAYER_TEXTURE] = LoadTexture("./assets/images/player_00.png");
    textures[ASSET_MAP_TEXTURE] = LoadTexture("./assets/images/map_texture.png");
    textures[ASSET_ENEMY_WHEEL_TEXTURE] = LoadTexture("./assets/images/enemy_wheel.png");
    textures[ASSET_ENEMY_BARREL_TEXTURE] = LoadTexture("./assets/images/enemy_barrel.png");
    textures[ASSET_COLLECTIBLE_HEALTH_TEXTURE] = LoadTexture("./assets/images/health_00.png");
    textures[ASSET_COLLECTIBLE_BULLET_TEXTURE] = LoadTexture("./assets/images/bullet_00.png");
    textures[ASSET_ICON_HEALTH_TEXTURE] = LoadTexture("./assets/images/icon_health.png");
    textures[ASSET_ICON_BULLET_TEXTURE] = LoadTexture("./assets/images/icon_bullet.png");
    textures[ASSET_ICON_FPS_TEXTURE] = LoadTexture("./assets/images/icon_fps.png");
    textures[ASSET_BULLET_TEXTURE] = LoadTexture("./assets/images/bullet.png");
    textures[ASSET_ICON_KILLS_TEXTURE] = LoadTexture("./assets/images/icon_kills.png");

    images[ASSET_MAP_IMAGE] = LoadImage("./assets/images/map_image.png");
  }
};

static AssetManager asset_manager{};
