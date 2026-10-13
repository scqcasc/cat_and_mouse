#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>
#include <string>
#include "Menu.h"

struct KeyBinding {
    std::string actionName;
    SDL_Keycode keycode;
};

class ControlsMenu {
public:
    ControlsMenu();
    ~ControlsMenu();

    void HandleInput(const SDL_Event& event, GameState& currentState);
    void Render(SDL_Renderer* renderer);

    SDL_Keycode GetKeyForAction(size_t index) const;

    void SaveBindings();
    void LoadBindings();

private:
    std::string GetConfigFilePath();
    void RenderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, float centerX, float centerY, SDL_Color color);

    TTF_Font* fontTitle = nullptr;
    TTF_Font* fontSubtext = nullptr;
    TTF_Font* fontButton = nullptr;

    std::vector<KeyBinding> bindings;
    int selectedIndex;
    bool isRebinding;

    const int startY = 180;
    const int buttonWidth = 460;
    const int buttonHeight = 50;
    const int shadowOffset = 5;
};
