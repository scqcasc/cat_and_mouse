#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>
#include <string>

enum class GameState {
    MainMenu,
    Gameplay,
    ControlsMenu,
    Exiting
};

class Menu {
public:
    Menu();
    ~Menu();

    // Process key presses specific to menu navigation
    void HandleInput(const SDL_Event& event, GameState& currentState);
    
    // Draw the 3D buttons and debug text labels
    void Render(SDL_Renderer* renderer);

private:
    TTF_Font* fontTitle = nullptr; 
    TTF_Font* fontButton = nullptr;

    int selectedIndex;
    std::vector<std::string> menuOptions;
    
    // UI Visual constants
    const int startY = 220;
    const int buttonWidth = 400;
    const int buttonHeight = 60;
    const int shadowOffset = 6;
};
