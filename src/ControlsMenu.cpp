#include "ControlsMenu.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

// These are the default keybindings
ControlsMenu::ControlsMenu() : selectedIndex(0), isRebinding(false) {
    bindings = {
        { "Move Left",    SDLK_A },
        { "Move Right",   SDLK_D },
        { "Move Up",      SDLK_W },
        { "Move Down",    SDLK_S },
        { "Pick Up Item", SDLK_G },
        { "Use Item",     SDLK_H },
        { "Drop Item",    SDLK_J },
        { "Meow",         SDLK_SPACE }
    };

    // Load custom TTF fonts
    fontTitle   = TTF_OpenFont("assets/fonts/static/Roboto-Regular.ttf", 32.0f);
    fontSubtext = TTF_OpenFont("assets/fonts/static/Roboto-Regular.ttf", 18.0f);
    fontButton  = TTF_OpenFont("assets/fonts/static/Roboto-Regular.ttf", 20.0f);

    if (!fontTitle || !fontSubtext || !fontButton) {
        std::cerr << "Failed to load ControlsMenu fonts: " << SDL_GetError() << std::endl;
    }

    // Load existing keybindings if present
    LoadBindings();
}

ControlsMenu::~ControlsMenu() {
    if (fontTitle)   TTF_CloseFont(fontTitle);
    if (fontSubtext) TTF_CloseFont(fontSubtext);
    if (fontButton)  TTF_CloseFont(fontButton);
}

std::string ControlsMenu::GetConfigFilePath() {
    char* prefPath = SDL_GetPrefPath("MyCompany", "MyVulkanGame");
    if (!prefPath) {
        return "controls.cfg";
    }

    std::string filePath = std::string(prefPath) + "controls.cfg";
    SDL_free(prefPath);
    return filePath;
}

void ControlsMenu::SaveBindings() {
    std::string path = GetConfigFilePath();
    std::ofstream outFile(path);

    if (!outFile.is_open()) {
        std::cerr << "Failed to open config file for writing: " << path << std::endl;
        return;
    }

    for (const auto& binding : bindings) {
        outFile << binding.actionName << "=" << static_cast<int>(binding.keycode) << "\n";
    }

    outFile.close();
    std::cout << "Saved controls configuration to: " << path << std::endl;
}

void ControlsMenu::LoadBindings() {
    std::string path = GetConfigFilePath();
    std::ifstream inFile(path);

    if (!inFile.is_open()) {
        return; // First launch, keep defaults
    }

    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream ss(line);
        std::string actionName;
        std::string keycodeStr;

        if (std::getline(ss, actionName, '=') && std::getline(ss, keycodeStr)) {
            SDL_Keycode code = static_cast<SDL_Keycode>(std::stoi(keycodeStr));

            for (auto& binding : bindings) {
                if (binding.actionName == actionName) {
                    binding.keycode = code;
                    break;
                }
            }
        }
    }

    inFile.close();
    std::cout << "Loaded controls configuration from: " << path << std::endl;
}

SDL_Keycode ControlsMenu::GetKeyForAction(size_t index) const {
    if (index < bindings.size()) {
        return bindings[index].keycode;
    }
    return SDLK_UNKNOWN;
}

void ControlsMenu::HandleInput(const SDL_Event& event, GameState& currentState) {
    if (event.type != SDL_EVENT_KEY_DOWN) return;

    if (isRebinding) {
        bindings[selectedIndex].keycode = event.key.key;
        isRebinding = false;

        // Persist change upon rebind
        SaveBindings();
        return;
    }

    if (event.key.key == SDLK_ESCAPE) {
        currentState = GameState::MainMenu;
    }
    else if (event.key.key == SDLK_DOWN) {
        selectedIndex = (selectedIndex + 1) % bindings.size();
    }
    else if (event.key.key == SDLK_UP) {
        selectedIndex = (selectedIndex - 1 + bindings.size()) % bindings.size();
    }
    else if (event.key.key == SDLK_RETURN || event.key.key == SDLK_RETURN2) {
        isRebinding = true;
    }
}

void ControlsMenu::RenderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, float centerX, float centerY, SDL_Color color) {
    if (!font || !text) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font, text, 0, color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        float dstX = centerX - ((float)surface->w / 2.0f);
        float dstY = centerY - ((float)surface->h / 2.0f);
        SDL_FRect dstRect{ dstX, dstY, (float)surface->w, (float)surface->h };

        SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
        SDL_DestroyTexture(texture);
    }
    SDL_DestroySurface(surface);
}

void ControlsMenu::Render(SDL_Renderer* renderer) {
    int windowWidth = 0, windowHeight = 0;
    SDL_GetRenderOutputSize(renderer, &windowWidth, &windowHeight);

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color subtextColor = { 200, 200, 200, 255 };

    // 1. Title & Navigation Subtext
    RenderText(renderer, fontTitle, "CONTROLS CONFIGURATION", 
               (float)windowWidth / 2.0f, (float)windowHeight * 0.08f, white);

    RenderText(renderer, fontSubtext, "UP/DOWN: Navigate | ENTER: Rebind | ESC: Back", 
               (float)windowWidth / 2.0f, (float)windowHeight * 0.13f, subtextColor);

    // 2. Button Items Layout Calculations
    const int itemSpacing = 60;
    float totalListHeight = (bindings.size() * itemSpacing);
    float startY = ((float)windowHeight / 2.0f) - (totalListHeight / 2.0f) + 30.0f;
    float centerX = ((float)windowWidth / 2.0f) - ((float)buttonWidth / 2.0f);

    for (size_t i = 0; i < bindings.size(); ++i) {
        float currentY = startY + (i * itemSpacing);

        // Shadow Plate
        SDL_FRect shadowRect{ 
            centerX + shadowOffset, 
            currentY + shadowOffset, 
            (float)buttonWidth, 
            (float)buttonHeight 
        };
        SDL_SetRenderDrawColor(renderer, 10, 15, 20, 200);
        SDL_RenderFillRect(renderer, &shadowRect);

        // Main Button Plate & Text Color
        SDL_FRect buttonRect{ centerX, currentY, (float)buttonWidth, (float)buttonHeight };
        SDL_Color textColor;

        if ((int)i == selectedIndex) {
            if (isRebinding) {
                SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255); // Red highlight during active rebinding
            } else {
                SDL_SetRenderDrawColor(renderer, 235, 140, 20, 255); // Amber highlight
            }
            textColor = { 20, 20, 20, 255 };
        } else {
            SDL_SetRenderDrawColor(renderer, 55, 68, 82, 255);
            textColor = { 255, 255, 255, 255 };
        }
        SDL_RenderFillRect(renderer, &buttonRect);

        // Button Keybinding Text
        std::string keyName = SDL_GetKeyName(bindings[i].keycode);
        if (keyName.empty()) keyName = "UNKNOWN";

        std::string displayText;
        if ((int)i == selectedIndex && isRebinding) {
            displayText = bindings[i].actionName + ": [ PRESS ANY KEY ]";
        } else {
            displayText = bindings[i].actionName + ": " + keyName;
        }

        float buttonCenterX = centerX + ((float)buttonWidth / 2.0f);
        float buttonCenterY = currentY + ((float)buttonHeight / 2.0f);

        RenderText(renderer, fontButton, displayText.c_str(), buttonCenterX, buttonCenterY, textColor);
    }
}
