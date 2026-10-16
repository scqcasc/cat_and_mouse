#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>

enum class RoomID {
    LivingRoom,
    MasterBedroom,
    GuestBedroom,
    Kitchen,
    Hallway,
    Outside // Backyard/Front porch when thrown out
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
    ToyRobot
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

struct Mouse {
    SDL_FRect rect{ 0.0f, 0.0f, 20.0f, 20.0f };
    MouseState state = MouseState::Wander;
    float dirX = 0.0f;
    float dirY = 0.0f;
    float changeDirTimer = 0.0f;
    float squeakTimer = 2.0f; // Countdown to squeaking
    float stunTimer = 0.0f;
    float speed = 120.0f;
};

struct Human {
    SDL_FRect rect{ 600.0f, 250.0f, 45.0f, 75.0f };
    float moveTimer = 14.0f;
    float irritation = 0.0f;       // 0% to 100%
    float touchCooldown = 0.0f;    // Prevents multi-touch spam
    float decayDelayTimer = 0.0f;  // Delay before irritation begins dropping
};

struct RoomState {
    RoomID id;
    std::string name;
    std::vector<Furniture> furniture;
    std::vector<Door> doors;
    std::vector<Mouse> mice;
    bool humanPresent = false;
    int miceCount = 0;
};
