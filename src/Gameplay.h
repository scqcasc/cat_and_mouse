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

    void InitializeHouse(float windowWidth = 1280.0f, float windowHeight = 720.0f);
    void SwitchRoom(RoomID nextRoom, RoomID previousRoom);
    void HandleInput(const SDL_Event& event, GameState& currentState, const ControlsMenu& controls);
    void Update(float deltaTime, const ControlsMenu& controls);
    void Render(SDL_Renderer* renderer, TTF_Font* fontUI);

private:
    RoomID previousHumanRoomID = RoomID::None;
    std::map<RoomID, RoomTemplate> roomTemplates;
    bool wasStunSoundPlayed = false;
    float scoreDecayTimer = 0.0f;
    float scoreDecayRate = 1.0f; // Loses points every 1.0 second
    int scoreDecayAmount = 10;   // Points lost per interval

    int score = 0;
    int highScore = 0;
    
    float windowWidth = 1280.0f;
    float windowHeight = 720.0f;
    bool isHouseInitialized = false;

    RoomID currentRoomID;
    std::map<RoomID, RoomState> house;

    int miceKilled = 0;
    int chancesRemaining = 3;
    int roundNumber = 1;
    float roundBannerTimer = 0.0f;
    bool isGameOver = false;

    // Player State
    SDL_FRect catRect{ 200.0f, 200.0f, 40.0f, 40.0f };
    float catSpeed = 250.0f;
    float stunTimer = 0.0f;
    bool isOutside = false;
    float outsidePenaltyTimer = 0.0f;

    // Player Inventory & Projectiles
    ItemType equippedItem = ItemType::None;
    int itemAmmo = 0;
    std::vector<Projectile> activeProjectiles;
    float lastFacingX = 1.0f; // Track facing direction for shooting
    float lastFacingY = 0.0f;

    // Human State
    Human human;
    RoomID humanRoomID = RoomID::LivingRoom;

    // Movement Direction Flags
    bool moveLeftPressed = false;
    bool moveRightPressed = false;
    bool moveUpPressed = false;
    bool moveDownPressed = false;

    SpeedEffect catSpeedEffect;
    SpeedEffect speedEffect;
    // Helper Methods
    void RenderSpeedEffectHUD(SDL_Renderer* renderer);
    void MoveHumanToRoom(RoomID newRoomID);
    void ApplySnackEffect(ItemType snackType, SpeedEffect& effect);
    void UpdateSpeedEffects(float deltaTime);
    void RespawnSnacksInRoom(RoomID roomID);
    void ProcessScoreDecay(float deltaTime);
    void DefineRoomTemplates();
    RoomState InstantiateRoomFromTemplate(const RoomTemplate& tmpl, float windowW, float windowH, int roundNum);
    SDL_FRect GetRoomBounds(RoomID roomID, float windowWidth, float windowHeight) const;
    void ProcessCatMovement(float deltaTime);
    void CheckDoorTransitions();
    void ClampCatToBounds(const SDL_FRect& bounds);
    void CheckItemPickups();
    void CheckCatHumanCollision();
    void AddScore(int points);
    void UpdateMice(float deltaTime);
    void UpdateHuman(float deltaTime);
    void UpdateProjectiles(float deltaTime);
    void IncreaseHumanIrritation(float amount);
    void UseEquippedItem();
    void ThrowCatOutside();
    void AdvanceToNextRound();

    void RenderMice(SDL_Renderer* renderer);
    void RenderHuman(SDL_Renderer* renderer);
    void RenderItems(SDL_Renderer* renderer);
    void RenderProjectiles(SDL_Renderer* renderer);
    void TriggerMeow();
    int GetTotalHouseMiceCount() const;
    void RenderHUD(SDL_Renderer* renderer, TTF_Font* fontUI);
};
