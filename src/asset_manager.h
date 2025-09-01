#pragma once

#include <unordered_map>

#include "raylib.h"

constexpr int ASSET_PLAYER_TEXTURE = 0;
constexpr int ASSET_MAP_TEXTURE = 1;
constexpr int ASSET_ENEMY_WHEEL_TEXTURE = 2;
constexpr int ASSET_COLLECTIBLE_HEALTH_TEXTURE = 3;
constexpr int ASSET_COLLECTIBLE_BULLET_TEXTURE = 4;
constexpr int ASSET_ICON_HEALTH_TEXTURE = 5;
constexpr int ASSET_ICON_BULLET_TEXTURE = 6;
constexpr int ASSET_ICON_FPS_TEXTURE = 7;
constexpr int ASSET_ENEMY_BARREL_TEXTURE = 8;
constexpr int ASSET_BULLET_TEXTURE = 9;
constexpr int ASSET_ICON_KILLS_TEXTURE = 10;
constexpr int ASSET_PLAYER_BROKEN_TEXTURE = 11;
constexpr int ASSET_ENEMY_BROKEN_TEXTURE = 12;
constexpr int ASSET_COLLECTIBLE_MINE_TEXTURE = 13;
constexpr int ASSET_ICON_MINE_TEXTURE = 14;
constexpr int ASSET_ENEMY_SPAWNER_TEXTURE = 15;

constexpr int ASSET_MAP_IMAGE = 0;

constexpr int ASSET_SOUND_PLAYER_SHOOT = 0;
constexpr int ASSET_SOUND_ENEMY_SHOOT = 1;
constexpr int ASSET_SOUND_PICKUP = 2;

constexpr int ASSET_MUSIC_ZAPPER = 0;

struct AssetManager {
  std::unordered_map<int, Texture2D> textures{};
  std::unordered_map<int, Image> images{};
  std::unordered_map<int, Sound> sounds{};
  std::unordered_map<int, Music> musics{};

  ~AssetManager() {
    for (auto const& kv : textures) UnloadTexture(kv.second);
    for (auto const& kv : images) UnloadImage(kv.second);
    for (auto const& kv : sounds) UnloadSound(kv.second);
  }

  void init() {
    textures[ASSET_PLAYER_TEXTURE] = LoadTexture("./assets/images/player.png");
    textures[ASSET_MAP_TEXTURE] = LoadTexture("./assets/images/map_texture.png");
    textures[ASSET_ENEMY_WHEEL_TEXTURE] = LoadTexture("./assets/images/enemy_wheel.png");
    textures[ASSET_ENEMY_BARREL_TEXTURE] = LoadTexture("./assets/images/enemy_barrel.png");
    textures[ASSET_COLLECTIBLE_HEALTH_TEXTURE] = LoadTexture("./assets/images/collectible_health.png");
    textures[ASSET_COLLECTIBLE_BULLET_TEXTURE] = LoadTexture("./assets/images/collectible_bullet.png");
    textures[ASSET_ICON_HEALTH_TEXTURE] = LoadTexture("./assets/images/icon_health.png");
    textures[ASSET_ICON_BULLET_TEXTURE] = LoadTexture("./assets/images/icon_bullet.png");
    textures[ASSET_ICON_FPS_TEXTURE] = LoadTexture("./assets/images/icon_fps.png");
    textures[ASSET_BULLET_TEXTURE] = LoadTexture("./assets/images/bullet.png");
    textures[ASSET_ICON_KILLS_TEXTURE] = LoadTexture("./assets/images/icon_kills.png");
    textures[ASSET_PLAYER_BROKEN_TEXTURE] = LoadTexture("./assets/images/player_broken.png");
    textures[ASSET_ENEMY_BROKEN_TEXTURE] = LoadTexture("./assets/images/enemy_broken.png");
    textures[ASSET_COLLECTIBLE_MINE_TEXTURE] = LoadTexture("./assets/images/collectible_mine.png");
    textures[ASSET_ICON_MINE_TEXTURE] = LoadTexture("./assets/images/icon_mine.png");
    textures[ASSET_ENEMY_SPAWNER_TEXTURE] = LoadTexture("./assets/images/enemy_spawner.png");

    images[ASSET_MAP_IMAGE] = LoadImage("./assets/images/map_image.png");

    sounds[ASSET_SOUND_PLAYER_SHOOT] = LoadSound("./assets/sounds/player_shoot.mp3");
    SetSoundVolume(sounds[ASSET_SOUND_PLAYER_SHOOT], 0.8f);
    sounds[ASSET_SOUND_ENEMY_SHOOT] = LoadSound("./assets/sounds/enemy_shoot.mp3");
    SetSoundVolume(sounds[ASSET_SOUND_ENEMY_SHOOT], 0.4f);
    sounds[ASSET_SOUND_PICKUP] = LoadSound("./assets/sounds/pickup.mp3");
    SetSoundVolume(sounds[ASSET_SOUND_PICKUP], 0.9f);

    musics[ASSET_MUSIC_ZAPPER] = LoadMusicStream("./assets/sounds/zapper.mp3");
    SetMusicVolume(musics[ASSET_MUSIC_ZAPPER], 1.0f);

    TraceLog(LOG_INFO, IsMusicValid(musics[ASSET_MUSIC_ZAPPER]) ? "VALID" : "NOT VALID");
  }
};

static AssetManager asset_manager{};
