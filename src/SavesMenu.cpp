#include "SavesMenu.h"
#include "Helpers.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>

SavesMenu::SavesMenu() : selectedIndex(0){
  saveOptions = {
    "Add Save",
    "Delete Save"
  };
    // Load custom TTF fonts at target sizes
    fontTitle = TTF_OpenFont("assets/fonts/static/Roboto-Regular.ttf", 36.0f);
    fontButton = TTF_OpenFont("assets/fonts/static/Roboto-Regular.ttf", 20.0f);

    if (!fontTitle || !fontButton) {
        std::cerr << "Failed to load TTF font: " << SDL_GetError() << std::endl;
    }
  }

  SavesMenu::~SavesMenu() {
    if (fontTitle) TTF_CloseFont(fontTitle);
    if (fontButton) TTF_CloseFont(fontButton);
  }

void SavesMenu::HandleInput(const SDL_Event& event, GameState& currentState) {
    if (event.type != SDL_EVENT_KEY_DOWN) return;


    if (event.key.key == SDLK_ESCAPE) {
        currentState = GameState::MainMenu;
    }
    else if (event.key.key == SDLK_DOWN) {
        selectedIndex = (selectedIndex + 1) % saveOptions.size();
    }
    else if (event.key.key == SDLK_UP) {
        selectedIndex = (selectedIndex - 1 + saveOptions.size()) % saveOptions.size();
    }
}

void SavesMenu::Render(SDL_Renderer* renderer) {
    int windowWidth = 0, windowHeight = 0;
    SDL_GetRenderOutputSize(renderer, &windowWidth, &windowHeight);

    // 1. Draw Title
    SDL_Color white = { 255, 255, 255, 255 };
    Helpers::RenderCenteredText(renderer, fontTitle, "Save Options", 
                       (float)windowWidth / 2.0f, (float)windowHeight * 0.15f, white);

    // 2. Draw Menu Buttons
    const int itemSpacing = 80;
    float totalMenuHeight = (saveOptions.size() * itemSpacing);
    float startY = ((float)windowHeight / 2.0f) - (totalMenuHeight / 2.0f) + 20.0f;
    float centerX = ((float)windowWidth / 2.0f) - ((float)buttonWidth / 2.0f);

    for (size_t i = 0; i < saveOptions.size(); ++i) {
        float currentY = startY + (i * itemSpacing);

        // Button Shadow
        SDL_FRect shadowRect{ 
            centerX + shadowOffset, currentY + shadowOffset, 
            (float)buttonWidth, (float)buttonHeight 
        };
        SDL_SetRenderDrawColor(renderer, 10, 15, 20, 200);
        SDL_RenderFillRect(renderer, &shadowRect);

        // Button Plate
        SDL_FRect buttonRect{ centerX, currentY, (float)buttonWidth, (float)buttonHeight };
        SDL_Color textColor;

        if ((int)i == selectedIndex) {
            SDL_SetRenderDrawColor(renderer, 235, 140, 20, 255); // Amber
            textColor = { 20, 20, 20, 255 };                    // Charcoal Text
        } else {
            SDL_SetRenderDrawColor(renderer, 55, 68, 82, 255);   // Slate Grey
            textColor = { 255, 255, 255, 255 };                 // White Text
        }
        SDL_RenderFillRect(renderer, &buttonRect);

        // Draw Centered Button Text
        float buttonCenterX = centerX + ((float)buttonWidth / 2.0f);
        float buttonCenterY = currentY + ((float)buttonHeight / 2.0f);
        Helpers::RenderCenteredText(renderer, fontButton, saveOptions[i].c_str(), buttonCenterX, buttonCenterY, textColor);
    }
}
