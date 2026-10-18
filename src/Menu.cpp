#include "Menu.h"
#include "Helpers.h"
#include "ScoreDatabase.h"
#include <iostream>
#include "SoundManager.h"

Menu::Menu() : selectedIndex(0) {
    menuOptions = {
        "Start New Game",
        "Continue Existing Game",
        "Manage Controls",
        "Leader Board",
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
            if (selectedIndex == 0)      {
                SoundManager::Instance().Play(SoundEffect::RoundClear);
                std::cout << "Transitioning to Gameplay..." << std::endl; // Debug log
                currentState = GameState::Gameplay;
            }
            else if (selectedIndex == 1) currentState = GameState::SavesMenu;
            else if (selectedIndex == 2) currentState = GameState::ControlsMenu;
            else if (selectedIndex == 3) {
                    SoundManager::Instance().Play(SoundEffect::RoundClear);
                    currentState = GameState::Leaderboard;
                }
            else if (selectedIndex == 4) currentState = GameState::Exiting;
        }
    }
}


void Menu::Render(SDL_Renderer* renderer) {
    int windowWidth = 0, windowHeight = 0;
    SDL_GetRenderOutputSize(renderer, &windowWidth, &windowHeight);

    // 1. Draw Title
    SDL_Color white = { 255, 255, 255, 255 };
    Helpers::RenderCenteredText(renderer, fontTitle, "CAT AND MOUSE GAME", 
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
        Helpers::RenderCenteredText(renderer, fontButton, menuOptions[i].c_str(), buttonCenterX, buttonCenterY, textColor);
    }
}

void RenderLeaderboardOverlay(SDL_Renderer* renderer, float windowWidth, float windowHeight) {
    // 1. Semi-transparent backdrop
    SDL_FRect backdrop{ 100.0f, 60.0f, windowWidth - 200.0f, windowHeight - 120.0f };
    SDL_SetRenderDrawColor(renderer, 15, 15, 25, 240);
    SDL_RenderFillRect(renderer, &backdrop);

    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Gold border
    SDL_RenderRect(renderer, &backdrop);

    // 2. Fetch Top Scores from SQLite DB
    auto topScores = ScoreDatabase::Instance().GetTopScores(10);

    // Table Header offsets
    float startY = 120.0f;
    float rowHeight = 35.0f;

    // You can iterate over topScores and render initials, score, levels, and date string
    for (size_t i = 0; i < topScores.size(); ++i) {
        const auto& entry = topScores[i];
        float y = startY + (i * rowHeight);

        // Format string: e.g. "1.   AAA    12,450 pts   Lvl 4   2026-10-18 10:45"
        std::cout << (i + 1) << ". " << entry.initials << " - " << entry.score 
                  << " pts (Rounds: " << entry.levelsCompleted << ") [" << entry.dateTime << "]" << std::endl;
    }
}
