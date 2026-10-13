#include "Menu.h"
#include <iostream>

Menu::Menu() : selectedIndex(0) {
    menuOptions = {
        "Start New Game",
        "Continue Existing Game",
        "Manage Controls",
        "Exit Game"
    };

    // Load custom TTF fonts at target sizes
    fontTitle = TTF_OpenFont("assets/fonts/static/Roboto-Regular.ttf", 36.0f);
    fontButton = TTF_OpenFont("assets/fonts/static/Roboto-Regular.ttf", 20.0f);

    if (!fontTitle || !fontButton) {
        std::cerr << "Failed to load TTF font: " << SDL_GetError() << std::endl;
    }
}

Menu::~Menu() {
    if (fontTitle) TTF_CloseFont(fontTitle);
    if (fontButton) TTF_CloseFont(fontButton);
}

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

// Helper to draw centered text on a target renderer
void RenderCenteredText(SDL_Renderer* renderer, TTF_Font* font, const char* text, float centerX, float centerY, SDL_Color color) {
    if (!font || !text) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font, text, 0, color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
        float dstX = centerX - ((float)surface->w / 2.0f);
        float dstY = centerY - ((float)surface->h / 2.0f);
        SDL_FRect dstRect{ dstX, dstY, (float)surface->w, (float)surface->h };

        SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
        SDL_DestroyTexture(texture);
    }
    SDL_DestroySurface(surface);
}

void Menu::Render(SDL_Renderer* renderer) {
    int windowWidth = 0, windowHeight = 0;
    SDL_GetRenderOutputSize(renderer, &windowWidth, &windowHeight);

    // 1. Draw Title
    SDL_Color white = { 255, 255, 255, 255 };
    RenderCenteredText(renderer, fontTitle, "VULKAN ENGINE MAIN MENU", 
                       (float)windowWidth / 2.0f, (float)windowHeight * 0.15f, white);

    // 2. Draw Menu Buttons
    const int itemSpacing = 80;
    float totalMenuHeight = (menuOptions.size() * itemSpacing);
    float startY = ((float)windowHeight / 2.0f) - (totalMenuHeight / 2.0f) + 20.0f;
    float centerX = ((float)windowWidth / 2.0f) - ((float)buttonWidth / 2.0f);

    for (size_t i = 0; i < menuOptions.size(); ++i) {
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
        RenderCenteredText(renderer, fontButton, menuOptions[i].c_str(), buttonCenterX, buttonCenterY, textColor);
    }
}
