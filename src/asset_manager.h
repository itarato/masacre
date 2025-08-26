#pragma once

#include <string>
#include <unordered_map>

#include "common.h"
#include "raylib.h"

struct AssetManager {
  std::unordered_map<int, Texture2D> textures{};
  std::unordered_map<int, Image> images{};

  ~AssetManager() {
    for (auto const& kv : textures) UnloadTexture(kv.second);
    for (auto const& kv : images) UnloadImage(kv.second);
  }

  void init() {
    textures[ASSET_PLAYER_TEXTURE] = LoadTexture("./assets/images/player_00.png");
    textures[ASSET_MAP_TEXTURE] = LoadTexture("./assets/images/map_0.png");
    textures[ASSET_ENEMY_TEXTURE] = LoadTexture("./assets/images/enemy_00.png");
    textures[ASSET_COLLECTIBLE_HEALTH_TEXTURE] = LoadTexture("./assets/images/health_00.png");
    textures[ASSET_COLLECTIBLE_BULLET_TEXTURE] = LoadTexture("./assets/images/bullet_00.png");
    textures[ASSET_ICON_HEALTH_TEXTURE] = LoadTexture("./assets/images/icon_health.png");
    textures[ASSET_ICON_BULLET_TEXTURE] = LoadTexture("./assets/images/icon_bullet.png");
    textures[ASSET_ICON_FPS_TEXTURE] = LoadTexture("./assets/images/icon_fps.png");

    images[ASSET_MAP_IMAGE] = LoadImage("./assets/images/map_0.png");
  }
};

static AssetManager asset_manager{};
