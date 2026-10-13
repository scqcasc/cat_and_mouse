#include "ControlsMenu.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

ControlsMenu::ControlsMenu() : selectedIndex(0), isRebinding(false) {
    // Default Fallback Bindings
    bindings = {
        { "Move Left",    SDLK_A },
        { "Move Right",   SDLK_D },
        { "Move Up",      SDLK_W },
        { "Move Down",    SDLK_S },
        { "Pick Up Item", SDLK_G },
        { "Use Item",     SDLK_H },
        { "Jump",         SDLK_SPACE }
    };

    // Load saved keybindings if they exist
    LoadBindings();
}

ControlsMenu::~ControlsMenu() {}

std::string ControlsMenu::GetConfigFilePath() {
    // SDL_GetPrefPath returns an OS-appropriate writable directory for user settings.
    // Replace "MyCompany" and "MyVulkanGame" with your actual game/developer naming.
    char* prefPath = SDL_GetPrefPath("MyCompany", "MyVulkanGame");
    if (!prefPath) {
        return "controls.cfg"; // Fallback to working directory if prefPath fails
    }

    std::string filePath = std::string(prefPath) + "controls.cfg";
    SDL_free(prefPath); // Always free the string returned by SDL_GetPrefPath
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
        // Save as "ActionName:Keycode"
        outFile << binding.actionName << "=" << static_cast<int>(binding.keycode) << "\n";
    }

    outFile.close();
    std::cout << "Saved controls configuration to: " << path << std::endl;
}

void ControlsMenu::LoadBindings() {
    std::string path = GetConfigFilePath();
    std::ifstream inFile(path);

    if (!inFile.is_open()) {
        // First boot or no config saved yet; default settings remain active
        return;
    }

    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream ss(line);
        std::string actionName;
        std::string keycodeStr;

        if (std::getline(ss, actionName, '=') && std::getline(ss, keycodeStr)) {
            SDL_Keycode code = static_cast<SDL_Keycode>(std::stoi(keycodeStr));

            // Map loaded keycode back to corresponding action
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

        // Persist change immediately upon setting a new key
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

void ControlsMenu::Render(SDL_Renderer* renderer) {
    int windowWidth = 0, windowHeight = 0;
    SDL_GetRenderOutputSize(renderer, &windowWidth, &windowHeight);

    const char* titleText = "CONTROLS CONFIGURATION";
    const char* subtext = "UP/DOWN: Navigate | ENTER: Rebind | ESC: Back";

    float titleX = ((float)windowWidth / 2.0f) - (((float)strlen(titleText) * 8.0f) / 2.0f);
    float subtextX = ((float)windowWidth / 2.0f) - (((float)strlen(subtext) * 8.0f) / 2.0f);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, titleX, (float)windowHeight * 0.08f, titleText);
    SDL_RenderDebugText(renderer, subtextX, (float)windowHeight * 0.13f, subtext);

    const int itemSpacing = 60;
    float totalListHeight = (bindings.size() * itemSpacing);
    float startY = ((float)windowHeight / 2.0f) - (totalListHeight / 2.0f) + 30.0f;
    float centerX = ((float)windowWidth / 2.0f) - ((float)buttonWidth / 2.0f);

    for (size_t i = 0; i < bindings.size(); ++i) {
        float currentY = startY + (i * itemSpacing);

        SDL_FRect shadowRect{ 
            centerX + shadowOffset, 
            currentY + shadowOffset, 
            (float)buttonWidth, 
            (float)buttonHeight 
        };
        SDL_SetRenderDrawColor(renderer, 10, 15, 20, 200);
        SDL_RenderFillRect(renderer, &shadowRect);

        SDL_FRect buttonRect{ centerX, currentY, (float)buttonWidth, (float)buttonHeight };

        if ((int)i == selectedIndex) {
            if (isRebinding) {
                SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 235, 140, 20, 255);
            }
            SDL_RenderFillRect(renderer, &buttonRect);
            SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 55, 68, 82, 255);
            SDL_RenderFillRect(renderer, &buttonRect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }

        std::string keyName = SDL_GetKeyName(bindings[i].keycode);
        if (keyName.empty()) keyName = "UNKNOWN";

        std::string displayText;
        if ((int)i == selectedIndex && isRebinding) {
            displayText = bindings[i].actionName + ": [ PRESS ANY KEY ]";
        } else {
            displayText = bindings[i].actionName + ": " + keyName;
        }

        int textWidth = (int)displayText.length() * 8;
        int textHeight = 8;
        float textX = centerX + ((float)buttonWidth / 2.0f) - ((float)textWidth / 2.0f);
        float textY = currentY + ((float)buttonHeight / 2.0f) - ((float)textHeight / 2.0f);

        SDL_RenderDebugText(renderer, textX, textY, displayText.c_str());
    }
}
