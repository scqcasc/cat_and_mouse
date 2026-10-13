#include "Menu.h"
#include <iostream>

Menu::Menu() : selectedIndex(0) {
    menuOptions = {
        "Start New Game",
        "Continue Existing Game",
        "Manage Controls",
        "Exit Game"
    };
}

Menu::~Menu() {}

void Menu::HandleInput(const SDL_Event& event, GameState& currentState) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.key == SDLK_DOWN) {
            selectedIndex = (selectedIndex + 1) % menuOptions.size();
        }
        else if (event.key.key == SDLK_UP) {
            selectedIndex = (selectedIndex - 1 + menuOptions.size()) % menuOptions.size();
        }
        else if (event.key.key == SDLK_RETURN || event.key.key == SDLK_RETURN2) {
            if (selectedIndex == 0)      currentState = GameState::Gameplay;
            else if (selectedIndex == 1) std::cout << "Save file slot empty!" << std::endl;
            else if (selectedIndex == 2) currentState = GameState::ControlsMenu;
            else if (selectedIndex == 3) currentState = GameState::Exiting;
        }
    }
}

void Menu::Render(SDL_Renderer* renderer) {
    // Get current target dimensions dynamically
    int windowWidth = 0, windowHeight = 0;
    SDL_GetRenderOutputSize(renderer, &windowWidth, &windowHeight);

    // Dynamic Title Centering
    const char* titleText = "VULKAN ENGINE MAIN MENU";
    int titleWidth = (int)strlen(titleText) * 8;
    float titleX = ((float)windowWidth / 2.0f) - ((float)titleWidth / 2.0f);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, titleX, (float)windowHeight * 0.15f, titleText);

    // Vertical layout calculations
    const int itemSpacing = 80;
    float totalMenuHeight = (menuOptions.size() * itemSpacing);
    float startY = ((float)windowHeight / 2.0f) - (totalMenuHeight / 2.0f) + 20.0f;
    float centerX = ((float)windowWidth / 2.0f) - ((float)buttonWidth / 2.0f);

    for (size_t i = 0; i < menuOptions.size(); ++i) {
        float currentY = startY + (i * itemSpacing);

        // 1. Drop Shadow
        SDL_FRect shadowRect{ 
            centerX + shadowOffset, 
            currentY + shadowOffset, 
            (float)buttonWidth, 
            (float)buttonHeight 
        };
        SDL_SetRenderDrawColor(renderer, 10, 15, 20, 200);
        SDL_RenderFillRect(renderer, &shadowRect);

        // 2. Main Plate
        SDL_FRect buttonRect{ centerX, currentY, (float)buttonWidth, (float)buttonHeight };

        if ((int)i == selectedIndex) {
            SDL_SetRenderDrawColor(renderer, 235, 140, 20, 255); // Amber
            SDL_RenderFillRect(renderer, &buttonRect);
            SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);   // Charcoal Text
        } else {
            SDL_SetRenderDrawColor(renderer, 55, 68, 82, 255);  // Slate Grey
            SDL_RenderFillRect(renderer, &buttonRect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White Text
        }

        // 3. Center Text
        int textWidth = (int)menuOptions[i].length() * 8;
        int textHeight = 8;
        float textX = centerX + ((float)buttonWidth / 2.0f) - ((float)textWidth / 2.0f);
        float textY = currentY + ((float)buttonHeight / 2.0f) - ((float)textHeight / 2.0f);

        SDL_RenderDebugText(renderer, textX, textY, menuOptions[i].c_str());
    }
}
