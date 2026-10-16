#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>

enum class RoomID {
    LivingRoom,
    MasterBedroom,
    GuestBedroom,
    Kitchen,
    Hallway
};

enum class MouseState {
    Wander,
    Flee,
    Stunned
};

enum class EntityType {
    Cat,
    Mouse,
    Human
};

enum class ItemType {
    None,
    Slingshot,
    LaserBlaster,
    ToyRobot
};

enum class FurnitureType {
    Static,      // Stove, Counter, Bed, Refrigerator
    Moveable,    // Lamp, Night Stand, Cabinet, TV
    Flippable    // Coffee Table, Chair, Bookcase, Dining Table
};

struct Furniture {
    std::string name;
    SDL_FRect rect;
    FurnitureType type;
    bool isFlipped = false;
    
    // Direction vector for nudging/moving
    float moveX = 0.0f;
    float moveY = 0.0f;
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
    float squeakTimer = 0.0f;
    float stunTimer = 0.0f;
    float speed = 120.0f;
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
