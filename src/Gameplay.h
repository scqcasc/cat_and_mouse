#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>
#include <map>
#include <string>
#include "GameTypes.h"
#include "ControlsMenu.h"

class Gameplay {
public:
    Gameplay();
    ~Gameplay();

    void InitializeHouse();
    void SwitchRoom(RoomID nextRoom, RoomID previousRoom);
    void HandleInput(const SDL_Event& event, GameState& currentState, const ControlsMenu& controls);
    void Update(float deltaTime, const ControlsMenu& controls);
    void Render(SDL_Renderer* renderer, TTF_Font* fontUI);

private:
    RoomID currentRoomID;
    std::map<RoomID, RoomState> house;

    int miceKilled = 0;
    int chancesRemaining = 3;
    
    // Player State
    SDL_FRect catRect{ 200.0f, 200.0f, 40.0f, 40.0f };
    float catSpeed = 250.0f;
    float stunTimer = 0.0f;

    // Movement Direction Flags
    bool moveLeftPressed = false;
    bool moveRightPressed = false;
    bool moveUpPressed = false;
    bool moveDownPressed = false;

    // AI & HUD Helpers (Declared only once)
    void UpdateMice(float deltaTime);
    void RenderMice(SDL_Renderer* renderer);
    void TriggerMeow();
    int GetTotalHouseMiceCount() const;
    void RenderHUD(SDL_Renderer* renderer, TTF_Font* fontUI);
};
