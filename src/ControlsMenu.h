#pragma once
#include <SDL3/SDL.h>
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

    // Save/Load methods
    void SaveBindings();
    void LoadBindings();

private:
    std::string GetConfigFilePath();

    std::vector<KeyBinding> bindings;
    int selectedIndex;
    bool isRebinding;

    const int startY = 180;
    const int buttonWidth = 460;
    const int buttonHeight = 50;
    const int shadowOffset = 5;
};
