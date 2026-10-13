#include <SDL3/SDL.h>
#include "Menu.h"
#include "ControlsMenu.h"
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>

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

    Menu mainMenu;
    ControlsMenu controlsMenu;
    GameState currentState = GameState::MainMenu;
    SDL_Event event;

    while (currentState != GameState::Exiting) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                currentState = GameState::Exiting;
            }
            
            // Route inputs dynamically based on state
            if (currentState == GameState::MainMenu) {
                mainMenu.HandleInput(event, currentState);
            }
            else if (currentState == GameState::ControlsMenu) {
                controlsMenu.HandleInput(event, currentState);
            }
            else if (currentState == GameState::Gameplay) {
                if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                    currentState = GameState::MainMenu;
                }
            }
        }

        // --- Render Execution ---
        if (currentState == GameState::MainMenu) {
            SDL_SetRenderDrawColor(renderer, 30, 38, 46, 255);
            SDL_RenderClear(renderer);
            
            mainMenu.Render(renderer);
        } 
        else if (currentState == GameState::Gameplay) {
            SDL_SetRenderDrawColor(renderer, 38, 102, 51, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDebugText(renderer, 540, 340, "GAMEPLAY ACTIVE - PRESS ESC TO RETURN");
        } 
        else if (currentState == GameState::ControlsMenu) {
            SDL_SetRenderDrawColor(renderer, 30, 38, 46, 255);
            SDL_RenderClear(renderer);
            
            controlsMenu.Render(renderer);
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
