#include "Gameplay.h"
#include "Helpers.h"
#include <iostream>
#include <random>
#include <cmath>

Gameplay::Gameplay() {
    InitializeHouse();
    currentRoomID = RoomID::LivingRoom;
}

Gameplay::~Gameplay() {}

void Gameplay::InitializeHouse() {
    house.clear();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> humanChance(0, 1);
    std::uniform_int_distribution<int> miceChance(1, 3);
    std::uniform_real_distribution<float> posX(200.0f, 800.0f);
    std::uniform_real_distribution<float> posY(200.0f, 500.0f);

    auto PopulateMice = [&](int count) {
        std::vector<Mouse> miceList;
        for (int i = 0; i < count; ++i) {
            Mouse m;
            m.rect.x = posX(gen);
            m.rect.y = posY(gen);
            miceList.push_back(m);
        }
        return miceList;
    };

    // 1. Living Room
    RoomState livingRoom;
    livingRoom.id = RoomID::LivingRoom;
    livingRoom.name = "Living Room";
    livingRoom.humanPresent = (humanChance(gen) == 1);
    livingRoom.mice = PopulateMice(miceChance(gen));
    livingRoom.miceCount = (int)livingRoom.mice.size();
    livingRoom.furniture = {
        { "Couch",        { 300, 200, 160, 80 },  FurnitureType::Moveable },
        { "Coffee Table", { 340, 320, 80,  40 },  FurnitureType::Flippable },
        { "Bookcase",     { 100, 180, 40,  120 }, FurnitureType::Flippable },
        { "TV",           { 550, 120, 100, 40 },  FurnitureType::Moveable }
    };
    livingRoom.doors = {
        { { 1200, 300, 20, 100 }, RoomID::Hallway, "To Hallway" },
        { { 500, 650, 100, 20 },  RoomID::Kitchen, "To Kitchen" }
    };
    house[RoomID::LivingRoom] = livingRoom;

    // 2. Kitchen
    RoomState kitchen;
    kitchen.id = RoomID::Kitchen;
    kitchen.name = "Kitchen";
    kitchen.humanPresent = (humanChance(gen) == 1);
    kitchen.mice = PopulateMice(miceChance(gen));
    kitchen.miceCount = (int)kitchen.mice.size();
    kitchen.furniture = {
        { "Stove",        { 100, 120, 80, 80 },   FurnitureType::Static },
        { "Refrigerator", { 200, 120, 80, 90 },   FurnitureType::Static },
        { "Dining Table", { 400, 300, 140, 90 },  FurnitureType::Flippable }
    };
    kitchen.doors = {
        { { 500, 70, 100, 20 }, RoomID::LivingRoom, "To Living Room" }
    };
    house[RoomID::Kitchen] = kitchen;

    // 3. Hallway
    RoomState hallway;
    hallway.id = RoomID::Hallway;
    hallway.name = "Hallway";
    hallway.humanPresent = false;
    hallway.mice = PopulateMice(miceChance(gen));
    hallway.miceCount = (int)hallway.mice.size();
    hallway.furniture = {
        { "Lamp",       { 300, 200, 40, 40 }, FurnitureType::Moveable },
        { "Night Stand",{ 500, 200, 50, 50 }, FurnitureType::Moveable }
    };
    hallway.doors = {
        { { 60, 300, 20, 100 },   RoomID::LivingRoom,    "To Living Room" },
        { { 400, 70, 100, 20 },   RoomID::MasterBedroom, "To Master Bed" },
        { { 800, 70, 100, 20 },   RoomID::GuestBedroom,  "To Guest Bed" }
    };
    house[RoomID::Hallway] = hallway;

    // 4. Master Bedroom
    RoomState masterBed;
    masterBed.id = RoomID::MasterBedroom;
    masterBed.name = "Master Bedroom";
    masterBed.humanPresent = (humanChance(gen) == 1);
    masterBed.mice = PopulateMice(miceChance(gen));
    masterBed.miceCount = (int)masterBed.mice.size();
    masterBed.furniture = {
        { "Bed",         { 200, 180, 140, 180 }, FurnitureType::Static },
        { "Night Stand", { 140, 180, 50, 50 },   FurnitureType::Moveable },
        { "Cabinet",     { 500, 180, 80, 120 },  FurnitureType::Moveable }
    };
    masterBed.doors = {
        { { 400, 650, 100, 20 }, RoomID::Hallway, "To Hallway" }
    };
    house[RoomID::MasterBedroom] = masterBed;

    // 5. Guest Bedroom
    RoomState guestBed;
    guestBed.id = RoomID::GuestBedroom;
    guestBed.name = "Guest Bedroom";
    guestBed.humanPresent = (humanChance(gen) == 1);
    guestBed.mice = PopulateMice(miceChance(gen));
    guestBed.miceCount = (int)guestBed.mice.size();
    guestBed.furniture = {
        { "Bed",             { 300, 180, 140, 180 }, FurnitureType::Static },
        { "Chest of Drawers",{ 550, 180, 70, 110 },  FurnitureType::Moveable }
    };
    guestBed.doors = {
        { { 800, 650, 100, 20 }, RoomID::Hallway, "To Hallway" }
    };
    house[RoomID::GuestBedroom] = guestBed;
}

void Gameplay::TriggerMeow() {
    std::cout << "MEOW! Scared nearby mice!" << std::endl;
    RoomState& currentRoom = house[currentRoomID];
    
    for (auto& mouse : currentRoom.mice) {
        if (mouse.state != MouseState::Stunned) {
            mouse.state = MouseState::Flee;
        }
    }
}

void Gameplay::UpdateMice(float deltaTime) {
    RoomState& currentRoom = house[currentRoomID];
    std::vector<size_t> caughtMiceIndices;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dirDist(-1.0f, 1.0f);
    std::uniform_real_distribution<float> timerDist(1.5f, 4.0f);

    for (size_t i = 0; i < currentRoom.mice.size(); ++i) {
        Mouse& m = currentRoom.mice[i];

        if (m.state == MouseState::Stunned) {
            m.stunTimer -= deltaTime;
            if (m.stunTimer <= 0.0f) {
                m.state = MouseState::Wander;
            }
        } else {
            // Distance check to Cat
            float centerX = m.rect.x + (m.rect.w / 2.0f);
            float centerY = m.rect.y + (m.rect.h / 2.0f);
            float catCenterX = catRect.x + (catRect.w / 2.0f);
            float catCenterY = catRect.y + (catRect.h / 2.0f);

            float dx = centerX - catCenterX;
            float dy = centerY - catCenterY;
            float distance = std::sqrt(dx * dx + dy * dy);

            // Trigger Flee state if Cat gets close (within 200 pixels)
            if (distance < 200.0f) {
                m.state = MouseState::Flee;
            }

            if (m.state == MouseState::Flee) {
                // Run directly away from Cat
                if (distance > 0.0001f) {
                    m.dirX = dx / distance;
                    m.dirY = dy / distance;
                }
                m.speed = 180.0f; // Flee speed

                if (distance > 350.0f) {
                    m.state = MouseState::Wander;
                }
            } 
            else if (m.state == MouseState::Wander) {
                m.speed = 90.0f; // Wander speed
                m.changeDirTimer -= deltaTime;

                if (m.changeDirTimer <= 0.0f) {
                    m.dirX = dirDist(gen);
                    m.dirY = dirDist(gen);
                    m.changeDirTimer = timerDist(gen);
                }
            }

            // Move mouse
            m.rect.x += m.dirX * m.speed * deltaTime;
            m.rect.y += m.dirY * m.speed * deltaTime;

            // Keep mouse bounded in playable screen region
            if (m.rect.x < 50.0f)   { m.rect.x = 50.0f;   m.dirX *= -1.0f; }
            if (m.rect.x > 1200.0f) { m.rect.x = 1200.0f; m.dirX *= -1.0f; }
            if (m.rect.y < 80.0f)   { m.rect.y = 80.0f;   m.dirY *= -1.0f; }
            if (m.rect.y > 650.0f)  { m.rect.y = 650.0f;  m.dirY *= -1.0f; }
        }

        // Cat Catches Mouse Check
        if (SDL_HasRectIntersectionFloat(&catRect, &m.rect)) {
            caughtMiceIndices.push_back(i);
        }
    }

    // Process caught mice (iterate backwards to safely erase)
    for (int i = (int)caughtMiceIndices.size() - 1; i >= 0; --i) {
        size_t index = caughtMiceIndices[i];
        currentRoom.mice.erase(currentRoom.mice.begin() + index);
        miceKilled++;
        currentRoom.miceCount = (int)currentRoom.mice.size();
        std::cout << "MICE CAUGHT! Total Score: " << miceKilled << std::endl;
    }
}

void Gameplay::HandleInput(const SDL_Event& event, GameState& currentState, const ControlsMenu& controls) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.key == SDLK_ESCAPE) {
            currentState = GameState::MainMenu;
            return;
        }

        if (event.key.key == controls.GetKeyForAction(0)) moveLeftPressed = true;
        if (event.key.key == controls.GetKeyForAction(1)) moveRightPressed = true;
        if (event.key.key == controls.GetKeyForAction(2)) moveUpPressed = true;
        if (event.key.key == controls.GetKeyForAction(3)) moveDownPressed = true;

        if (event.key.key == controls.GetKeyForAction(4)) {
            std::cout << "Cat interact in " << house[currentRoomID].name << std::endl;
        }
        if (event.key.key == controls.GetKeyForAction(5)) { // Meow Key
            TriggerMeow();
        }
    }
    else if (event.type == SDL_EVENT_KEY_UP) {
        if (event.key.key == controls.GetKeyForAction(0)) moveLeftPressed = false;
        if (event.key.key == controls.GetKeyForAction(1)) moveRightPressed = false;
        if (event.key.key == controls.GetKeyForAction(2)) moveUpPressed = false;
        if (event.key.key == controls.GetKeyForAction(3)) moveDownPressed = false;
    }
}

void Gameplay::Update(float deltaTime, const ControlsMenu& controls) {
    if (stunTimer > 0.0f) {
        stunTimer -= deltaTime;
        return;
    }

    float moveX = 0.0f;
    float moveY = 0.0f;

    if (moveLeftPressed)  moveX -= 1.0f;
    if (moveRightPressed) moveX += 1.0f;
    if (moveUpPressed)    moveY -= 1.0f;
    if (moveDownPressed)  moveY += 1.0f;

    if (moveX != 0.0f && moveY != 0.0f) {
        moveX *= 0.7071f;
        moveY *= 0.7071f;
    }

    RoomState& currentRoom = house[currentRoomID];

    // Cat Movement & Furniture Collision
    catRect.x += moveX * catSpeed * deltaTime;
    for (auto& item : currentRoom.furniture) {
        if (SDL_HasRectIntersectionFloat(&catRect, &item.rect)) {
            if (item.type == FurnitureType::Static) {
                if (moveX > 0) catRect.x = item.rect.x - catRect.w;
                else if (moveX < 0) catRect.x = item.rect.x + item.rect.w;
            } else {
                item.rect.x += moveX * (catSpeed * 0.4f) * deltaTime;
            }
        }
    }

    catRect.y += moveY * catSpeed * deltaTime;
    for (auto& item : currentRoom.furniture) {
        if (SDL_HasRectIntersectionFloat(&catRect, &item.rect)) {
            if (item.type == FurnitureType::Static) {
                if (moveY > 0) catRect.y = item.rect.y - catRect.h;
                else if (moveY < 0) catRect.y = item.rect.y + item.rect.h;
            } else {
                item.rect.y += moveY * (catSpeed * 0.4f) * deltaTime;
            }
        }
    }

    // Door Transitions
    for (const auto& door : currentRoom.doors) {
        if (SDL_HasRectIntersectionFloat(&catRect, &door.rect)) {
            moveLeftPressed = moveRightPressed = moveUpPressed = moveDownPressed = false;
            SwitchRoom(door.targetRoom, currentRoomID);
            break;
        }
    }

    // Update Mouse AI Logic
    UpdateMice(deltaTime);
}

void Gameplay::RenderMice(SDL_Renderer* renderer) {
    const RoomState& currentRoom = house[currentRoomID];

    for (const auto& mouse : currentRoom.mice) {
        if (mouse.state == MouseState::Flee) {
            SDL_SetRenderDrawColor(renderer, 230, 60, 60, 255); // Red when fleeing
        } else if (mouse.state == MouseState::Stunned) {
            SDL_SetRenderDrawColor(renderer, 220, 220, 60, 255); // Yellow when stunned
        } else {
            SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255); // Light Grey wandering
        }
        SDL_RenderFillRect(renderer, &mouse.rect);
    }
}

void Gameplay::Render(SDL_Renderer* renderer, TTF_Font* fontUI) {
    const RoomState& currentRoom = house[currentRoomID];

    // 1. Render Doors
    SDL_SetRenderDrawColor(renderer, 120, 70, 30, 255);
    for (const auto& door : currentRoom.doors) {
        SDL_RenderFillRect(renderer, &door.rect);
    }

    // 2. Render Furniture
    for (const auto& item : currentRoom.furniture) {
        if (item.type == FurnitureType::Static) {
            SDL_SetRenderDrawColor(renderer, 100, 110, 120, 255);
        } else if (item.type == FurnitureType::Moveable) {
            SDL_SetRenderDrawColor(renderer, 160, 100, 60, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 180, 140, 80, 255);
        }
        SDL_RenderFillRect(renderer, &item.rect);
    }

    // 3. Render Mice Entities
    RenderMice(renderer);

    // 4. Render Cat
    SDL_SetRenderDrawColor(renderer, 240, 130, 40, 255);
    SDL_RenderFillRect(renderer, &catRect);

    // 5. Render HUD Overlay
    RenderHUD(renderer, fontUI);
}

void Gameplay::SwitchRoom(RoomID nextRoom, RoomID previousRoom) {
    currentRoomID = nextRoom;

    // Reposition Cat near the entering door of the new room
    for (const auto& door : house[currentRoomID].doors) {
        if (door.targetRoom == previousRoom) {
            catRect.x = door.rect.x;
            catRect.y = door.rect.y;

            if (door.rect.w > door.rect.h) { // Top/Bottom Door
                catRect.y += (door.rect.y < 200.0f) ? 50.0f : -50.0f;
            } else { // Left/Right Door
                catRect.x += (door.rect.x < 200.0f) ? 50.0f : -50.0f;
            }
            break;
        }
    }
}

int Gameplay::GetTotalHouseMiceCount() const {
    int total = 0;
    for (const auto& pair : house) {
        total += (int)pair.second.mice.size();
    }
    return total;
}

void Gameplay::RenderHUD(SDL_Renderer* renderer, TTF_Font* fontUI) {
    int windowWidth = 0, windowHeight = 0;
    SDL_GetRenderOutputSize(renderer, &windowWidth, &windowHeight);

    // Top HUD Bar Background
    SDL_FRect hudBar{ 0.0f, 0.0f, (float)windowWidth, 50.0f };
    SDL_SetRenderDrawColor(renderer, 20, 25, 30, 230);
    SDL_RenderFillRect(renderer, &hudBar);

    if (!fontUI) return;

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color yellow = { 240, 200, 40, 255 };
    SDL_Color cyan = { 80, 210, 240, 255 };

    const RoomState& activeRoom = house[currentRoomID];

    std::string roomText = "Room: " + activeRoom.name;
    std::string miceText = "Mice in Room: " + std::to_string(activeRoom.mice.size()) + 
                           " (Total House: " + std::to_string(GetTotalHouseMiceCount()) + ")";
    std::string humanText = std::string("Human Present: ") + (activeRoom.humanPresent ? "YES!" : "No");
    std::string livesText = "Chances: " + std::to_string(chancesRemaining);

    Helpers::RenderCenteredText(renderer, fontUI, roomText.c_str(), 140.0f, 25.0f, cyan);
    Helpers::RenderCenteredText(renderer, fontUI, miceText.c_str(), (float)windowWidth * 0.42f, 25.0f, white);
    Helpers::RenderCenteredText(renderer, fontUI, humanText.c_str(), (float)windowWidth * 0.75f, 25.0f, yellow);
    Helpers::RenderCenteredText(renderer, fontUI, livesText.c_str(), (float)windowWidth - 100.0f, 25.0f, white);
}
