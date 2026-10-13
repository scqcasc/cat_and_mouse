#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <string>

// Enum tracking what screen the player is currently viewing
enum class GameState {
    MainMenu,
    Gameplay,
    ControlsMenu,
    Exiting
};

int main(int argc, char* argv[]) {
    // 1. Initialize SDL Video Subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // 2. Create the Window (Configured to accept an modern GPU backend)
    SDL_Window* window = SDL_CreateWindow(
        "Proton-Optimized Game Framework",
        1280, 720,
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // 3. Initialize the SDL GPU Device (Defaults to Vulkan under Proton/Linux)
    // This automates creating the Vulkan Instance, Device, and Swapchain wrapper
    SDL_GPUDevice* gpuDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!gpuDevice) {
        std::cerr << "GPU Subsystem failed to initialize: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Claim the window for our Vulkan-backed GPU device context
    if (!SDL_ClaimWindowForGPUDevice(gpuDevice, window)) {
        std::cerr << "Failed to claim window context: " << SDL_GetError() << std::endl;
        SDL_DestroyGPUDevice(gpuDevice);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // --- Menu Configuration System ---
    GameState currentState = GameState::MainMenu;
    int selectedIndex = 0;
    const std::vector<std::string> menuOptions = {
        "Start New Game",
        "Continue Existing Game",
        "Manage Controls",
        "Exit Game"
    };

    bool running = true;
    SDL_Event event;

    std::cout << "--- MAIN MENU INITIALIZED ---" << std::endl;
    std::cout << "Use UP/DOWN Arrow keys to navigate. Press ENTER to select." << std::endl;
    std::cout << "Current Selection: " << menuOptions[selectedIndex] << std::endl;

    // --- Main Application Loop ---
    while (running && currentState != GameState::Exiting) {
        
        // 4. Handle Input and Menu Navigation Events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                currentState = GameState::Exiting;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (currentState == GameState::MainMenu) {
                    if (event.key.key == SDLK_DOWN) {
                        selectedIndex = (selectedIndex + 1) % menuOptions.size();
                        std::cout << "Current Selection: " << menuOptions[selectedIndex] << std::endl;
                    }
                    else if (event.key.key == SDLK_UP) {
                        selectedIndex = (selectedIndex - 1 + menuOptions.size()) % menuOptions.size();
                        std::cout << "Current Selection: " << menuOptions[selectedIndex] << std::endl;
                    }
                    else if (event.key.key == SDLK_RETURN || event.key.key == SDLK_RETURN2) {
                        std::cout << "\n[Selected Action]: " << menuOptions[selectedIndex] << std::endl;
                        
                        // Process menu interactions
                        if (selectedIndex == 0)      currentState = GameState::Gameplay;
                        else if (selectedIndex == 1) std::cout << "-> Error: No existing save file found!" << std::endl;
                        else if (selectedIndex == 2) currentState = GameState::ControlsMenu;
                        else if (selectedIndex == 3) currentState = GameState::Exiting;
                    }
                }
                else if (currentState == GameState::Gameplay || currentState == GameState::ControlsMenu) {
                    // Press ESCAPE to return to the Main Menu from any active submenu
                    if (event.key.key == SDLK_ESCAPE) {
                        std::cout << "\nReturning to Main Menu..." << std::endl;
                        currentState = GameState::MainMenu;
                        selectedIndex = 0;
                    }
                }
            }
        }

        // 5. Hardware Rendering Block (Acquire Vulkan Command Buffers)
        SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(gpuDevice);
        if (cmdBuffer) {
            uint32_t w, h;
            SDL_GPUTexture* swapchainTexture = nullptr;

            // Acquire the next target texture from the Vulkan swapchain
            if (SDL_AcquireGPUSwapchainTexture(cmdBuffer, window, &swapchainTexture, &w, &h)) {
                if (swapchainTexture != nullptr) {
                    
                    // Define our hardware clear configuration
                    SDL_GPUColorTargetInfo colorTargetInfo{};
                    colorTargetInfo.texture = swapchainTexture;
                    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;    // Clear the screen buffer on loop start
                    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;  // Save results for presentation
                    
                    // Choose background color based on the system state (Normalized 0.0f - 1.0f)
                    if (currentState == GameState::MainMenu) {
                        // Deep Slate Grey/Blue for Menu Background
                        colorTargetInfo.clear_color = SDL_FColor{ 0.12f, 0.15f, 0.18f, 1.0f };
                    } 
                    else if (currentState == GameState::Gameplay) {
                        // Forest Green when the gameplay starts
                        colorTargetInfo.clear_color = SDL_FColor{ 0.15f, 0.40f, 0.20f, 1.0f };
                    } 
                    else if (currentState == GameState::ControlsMenu) {
                        // Deep Crimson for the configuration menu
                        colorTargetInfo.clear_color = SDL_FColor{ 0.35f, 0.10f, 0.15f, 1.0f };
                    }

                    // Start the render pass (Applies the hardware background clear)
                    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTargetInfo, 1, nullptr);
                    
                    // (Future visual vertex geometry/text drawing calls go right here!)
                    
                    SDL_EndGPURenderPass(renderPass);
                }
            }
            
            // Push compiled frame buffers directly down to the Vulkan graphics queue
            SDL_SubmitGPUCommandBuffer(cmdBuffer);
        }

        SDL_Delay(16); // Target ~60Hz ticks to lower CPU usage
    }

    std::cout << "\nExiting game framework engine cleanly..." << std::endl;

    // 6. Tear Down Hardware Resources
    SDL_ReleaseWindowFromGPUDevice(gpuDevice, window);
    SDL_DestroyGPUDevice(gpuDevice);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
