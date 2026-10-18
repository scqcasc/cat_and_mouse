#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <map>

enum class RoomID {
    LivingRoom,
    MasterBedroom,
    GuestBedroom,
    Kitchen,
    Hallway,
    Outside,
    None
};

enum class MouseState {
    Wander,
    Flee,
    Stunned
};

enum class ItemType {
    None,
    Slingshot,
    LaserBlaster,
    ToyRobot,
    PowerUpSnack,   
    PowerDownSnack
};

enum class FurnitureType {
    Static,
    Moveable,
    Flippable
};

struct Furniture {
    std::string name;
    SDL_FRect rect;
    FurnitureType type;
    bool isFlipped = false;
};

struct Door {
    SDL_FRect rect;
    RoomID targetRoom;
    std::string label;
};


struct SpeedEffect {
    float modifier = 1.0f; // Multiplier (e.g., 1.5f for +50%, 0.6f for -40%)
    float duration = 0.0f; // Active time remaining in seconds
};

struct Mouse {
    SDL_FRect rect{ 0.0f, 0.0f, 20.0f, 20.0f };
    MouseState state = MouseState::Wander;
    float dirX = 0.0f;
    float dirY = 0.0f;
    float changeDirTimer = 0.0f;
    float squeakTimer = 5.0f;
    float stunTimer = 0.0f;
    float speed = 120.0f;

    // Armed Mouse Attributes
    ItemType equippedItem = ItemType::None;
    int ammo = 0;
    float shootCooldown = 0.0f;
    SpeedEffect speedEffect;
};

struct Human {
    SDL_FRect rect{ 600.0f, 250.0f, 45.0f, 75.0f };
    float moveTimer = 14.0f;
    float irritation = 0.0f;
    float touchCooldown = 0.0f;
    float decayDelayTimer = 0.0f;

    float targetX = 600.0f;
    float targetY = 250.0f;
    float wanderSpeed = 60.0f;
    float pauseTimer = 0.0f;

    bool isHeadingToDoor = false;
    RoomID nextRoomID = RoomID::LivingRoom;
    RoomID currentRoom = RoomID::LivingRoom;
};

struct Item {
    SDL_FRect rect{ 0.0f, 0.0f, 22.0f, 22.0f };
    ItemType type = ItemType::None;
    int ammo = 3;
    bool spawned = true;
};

struct Projectile {
    SDL_FRect rect{ 0.0f, 0.0f, 8.0f, 8.0f };
    float dirX = 0.0f;
    float dirY = 0.0f;
    float speed = 500.0f;
    float lifetime = 2.0f;
    ItemType type = ItemType::Slingshot;
    bool isFiredByMouse = false; // Distinguishes friendly vs hostile projectiles
};

struct RoomState {
    RoomID id;
    std::string name;
    std::vector<Furniture> furniture;
    std::vector<Door> doors;
    std::vector<Mouse> mice;
    std::vector<Item> items;
    bool humanPresent = false;
    int miceCount = 0;
};

// Struct representing relative door configurations (0.0 to 1.0 wall ratios)
struct DoorConfig {
    float relX;      // 0.0 = Left edge, 1.0 = Right edge
    float relY;      // 0.0 = Top edge, 1.0 = Bottom edge
    float width;
    float height;
    RoomID targetRoom;
    std::string label;
};

// Struct representing potential furniture spawn choices
struct FurnitureSpawnOption {
    std::string name;
    float relX;       // Relative offset (0.0 to 1.0 inside room bounds)
    float relY;
    float width;
    float height;
    FurnitureType type;
    float spawnChance; // 0.0 to 1.0 probability
};

// Master Template for generating room instances
struct RoomTemplate {
    RoomID id;
    std::string name;
    std::vector<DoorConfig> doorConfigs;
    std::vector<FurnitureSpawnOption> furniturePool;
    ItemType defaultItem = ItemType::None;
    int baseItemAmmo = 3;
};
