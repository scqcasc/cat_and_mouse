#include <SDL3/SDL.h>
#include "Menu.h"
#include "ControlsMenu.h"
#include "SavesMenu.h"
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include "Gameplay.h"
#include "SoundManager.h"
#include "SoundGenerator.h"
#include "LeaderboardMenu.h"

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return -1;
    }

    if (!TTF_Init()) {
        std::cerr << "Failed to initialize SDL_ttf: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Proton-Optimized Game Framework", 0, 0, SDL_WINDOW_FULLSCREEN);
    if (!window) {
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // In main.cpp after SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)
    // Generate audio assets on launch if they don't exist yet
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    SoundGenerator::GenerateAllAssets("assets/audio/");
    if (SoundManager::Instance().Initialize()) {
        SoundManager::Instance().LoadWAV(SoundEffect::Meow, "assets/audio/meow.wav");
        SoundManager::Instance().LoadWAV(SoundEffect::Squeak, "assets/audio/squeak.wav");
        SoundManager::Instance().LoadWAV(SoundEffect::Slingshot, "assets/audio/slingshot.wav");
        SoundManager::Instance().LoadWAV(SoundEffect::Laser, "assets/audio/laser.wav");
        SoundManager::Instance().LoadWAV(SoundEffect::Stun, "assets/audio/stun.wav");
        SoundManager::Instance().LoadWAV(SoundEffect::RoundClear, "assets/audio/round_clear.wav");
        SoundManager::Instance().LoadWAV(SoundEffect::PowerUpSnack, "assets/audio/powerup_snack.wav");
        SoundManager::Instance().LoadWAV(SoundEffect::PowerDownSnack, "assets/audio/powerdown_snack.wav");
    }
    // Load a font for the gameplay HUD
    TTF_Font* fontUI = TTF_OpenFont("assets/fonts/static/Roboto-Regular.ttf", 18.0f);

    if (!fontUI) {
        std::cerr << "Failed to load gameplay fontUI: " << SDL_GetError() << std::endl;
    }

    Menu mainMenu;
    ControlsMenu controlsMenu;
    SavesMenu savesMenu;
    GameState currentState = GameState::MainMenu;
    SDL_Event event;
    Gameplay gameplay;

    uint64_t lastTime = SDL_GetTicksNS();

    // Initialize Database
    ScoreDatabase::Instance().Initialize("highscores.db");

    LeaderboardMenu leaderboardMenu;

while (currentState != GameState::Exiting) {
        // Calculate Delta Time
        uint64_t currentTime = SDL_GetTicksNS();
        float deltaTime = static_cast<float>(currentTime - lastTime) / 1000000000.0f;
        lastTime = currentTime;

        // --- 1. EVENT POLLING LOOP ---
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                currentState = GameState::Exiting;
            }
            
            // Route input events to the current active state
            if (currentState == GameState::MainMenu) {
                mainMenu.HandleInput(event, currentState);
            }
            else if (currentState == GameState::ControlsMenu) {
                controlsMenu.HandleInput(event, currentState);
            }
            else if (currentState == GameState::SavesMenu) {
                savesMenu.HandleInput(event, currentState);
            }
            else if (currentState == GameState::Leaderboard) {
                leaderboardMenu.HandleInput(event, currentState);
            }
            else if (currentState == GameState::Gameplay) {
                // Route KEY_DOWN and KEY_UP events to gameplay input handler
                gameplay.HandleInput(event, currentState, controlsMenu);
            }
        }

        // --- 2. GAME STATE UPDATES (EVERY FRAME) ---
        if (currentState == GameState::Gameplay) {
            gameplay.Update(deltaTime, controlsMenu);
        }

        // --- 3. RENDER EXECUTION (EVERY FRAME) ---
        if (currentState == GameState::MainMenu) {
            SDL_SetRenderDrawColor(renderer, 30, 38, 46, 255);
            SDL_RenderClear(renderer);
            mainMenu.Render(renderer);
        } 
        else if (currentState == GameState::Gameplay) {
            SDL_SetRenderDrawColor(renderer, 38, 102, 51, 255); // Green Carpet
            SDL_RenderClear(renderer);
            
            gameplay.Render(renderer, fontUI); // Render world and HUD
        } 
        else if (currentState == GameState::ControlsMenu) {
            SDL_SetRenderDrawColor(renderer, 30, 38, 46, 255);
            SDL_RenderClear(renderer);
            controlsMenu.Render(renderer);
        }
        else if (currentState == GameState::SavesMenu) {
            SDL_SetRenderDrawColor(renderer, 30, 38, 46, 255);
            SDL_RenderClear(renderer);
            savesMenu.Render(renderer);
        }
        else if (currentState == GameState::Leaderboard) {
            SDL_SetRenderDrawColor(renderer, 30, 38, 46, 255);
            SDL_RenderClear(renderer);
            leaderboardMenu.Render(renderer);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); 
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
