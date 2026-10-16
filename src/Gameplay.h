#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>
#include <string>
#include <map>
#include "GameTypes.h"
#include "ControlsMenu.h"

// enum class RoomID {
//     LivingRoom,
//     MasterBedroom,
//     GuestBedroom,
//     Kitchen,
//     Hallway
// };

class Gameplay {
public:
    Gameplay();
    ~Gameplay();

    void InitializeHouse();
    void SwitchRoom(RoomID nextRoom, RoomID previousRoom);
    void LoadRoom(RoomID room);
    void HandleInput(const SDL_Event& event, GameState& currentState, const ControlsMenu& controls);
    void Update(float deltaTime, const ControlsMenu& controls);
    void Render(SDL_Renderer* renderer, TTF_Font* fontUI);

private:
    // Movement Direction Flags
    bool moveLeftPressed = false;
    bool moveRightPressed = false;
    bool moveUpPressed = false;
    bool moveDownPressed = false;

    RoomID currentRoomID;
    std::map<RoomID, RoomState> house;
    int miceKilled = 0;
    int chancesRemaining = 3;
    
    // Player State
    SDL_FRect catRect{ 200.0f, 200.0f, 40.0f, 40.0f };
    float catSpeed = 250.0f;
    float stunTimer = 0.0f;
    ItemType heldItem = ItemType::None;

    // Room Entities
    std::vector<Furniture> roomFurniture;
    bool humanPresent = false;
    int miceInCurrentRoom = 0;

    int GetTotalHouseMiceCount() const;
    void RenderHUD(SDL_Renderer* renderer, TTF_Font* fontUI);
};
