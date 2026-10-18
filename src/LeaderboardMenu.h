#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>
#include <string>
#include "Menu.h"
#include "ScoreDatabase.h"

class LeaderboardMenu {
public:
    LeaderboardMenu();
    ~LeaderboardMenu();

    void HandleInput(const SDL_Event& event, GameState& currentState);
    void Render(SDL_Renderer* renderer);

private:
    void RenderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, float x, float y, SDL_Color color, bool center = true);

    TTF_Font* fontTitle = nullptr;
    TTF_Font* fontHeader = nullptr;
    TTF_Font* fontRow = nullptr;
    TTF_Font* fontSubtext = nullptr;
};
