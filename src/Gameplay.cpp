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

    // 1. Living Room (Contains Slingshot)
    RoomState livingRoom;
    livingRoom.id = RoomID::LivingRoom;
    livingRoom.name = "Living Room";
    livingRoom.humanPresent = true;
    livingRoom.mice = PopulateMice(miceChance(gen));
    livingRoom.miceCount = (int)livingRoom.mice.size();
    livingRoom.items = { { { 700.0f, 350.0f, 22.0f, 22.0f }, ItemType::Slingshot, 5, true } };
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

    // 2. Kitchen (Contains Toy Robot)
    RoomState kitchen;
    kitchen.id = RoomID::Kitchen;
    kitchen.name = "Kitchen";
    kitchen.mice = PopulateMice(miceChance(gen));
    kitchen.miceCount = (int)kitchen.mice.size();
    kitchen.items = { { { 300.0f, 450.0f, 22.0f, 22.0f }, ItemType::ToyRobot, 2, true } };
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

    // 4. Master Bedroom (Contains Laser Blaster)
    RoomState masterBed;
    masterBed.id = RoomID::MasterBedroom;
    masterBed.name = "Master Bedroom";
    masterBed.mice = PopulateMice(miceChance(gen));
    masterBed.miceCount = (int)masterBed.mice.size();
    masterBed.items = { { { 600.0f, 220.0f, 22.0f, 22.0f }, ItemType::LaserBlaster, 3, true } };
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

void Gameplay::UseEquippedItem() {
    if (equippedItem == ItemType::None || itemAmmo <= 0) return;

    itemAmmo--;
    std::cout << "Used Item! Ammo remaining: " << itemAmmo << std::endl;

    Projectile p;
    p.rect.x = catRect.x + (catRect.w / 2.0f) - 4.0f;
    p.rect.y = catRect.y + (catRect.h / 2.0f) - 4.0f;
    p.dirX = lastFacingX;
    p.dirY = lastFacingY;
    p.type = equippedItem;

    if (equippedItem == ItemType::Slingshot) {
        p.speed = 550.0f;
    } else if (equippedItem == ItemType::LaserBlaster) {
        p.speed = 850.0f;
    }

    activeProjectiles.push_back(p);

    if (itemAmmo <= 0) {
        equippedItem = ItemType::None;
    }
}

void Gameplay::UpdateProjectiles(float deltaTime) {
    RoomState& currentRoom = house[currentRoomID];
    std::vector<size_t> expiredProjectiles;

    for (size_t i = 0; i < activeProjectiles.size(); ++i) {
        Projectile& p = activeProjectiles[i];
        p.lifetime -= deltaTime;

        p.rect.x += p.dirX * p.speed * deltaTime;
        p.rect.y += p.dirY * p.speed * deltaTime;

        if (p.lifetime <= 0.0f) {
            expiredProjectiles.push_back(i);
            continue;
        }

        // Check collision with mice
        for (auto& mouse : currentRoom.mice) {
            if (SDL_HasRectIntersectionFloat(&p.rect, &mouse.rect)) {
                mouse.state = MouseState::Stunned;
                mouse.stunTimer = 4.0f; // Stun mouse for 4 seconds
                std::cout << "MOUSE STUNNED BY PROJECTILE!" << std::endl;
                expiredProjectiles.push_back(i);
                break;
            }
        }
    }

    // Erase expired/hit projectiles backwards
    for (int i = (int)expiredProjectiles.size() - 1; i >= 0; --i) {
        size_t index = expiredProjectiles[i];
        if (index < activeProjectiles.size()) {
            activeProjectiles.erase(activeProjectiles.begin() + index);
        }
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

        if (event.key.key == controls.GetKeyForAction(4)) { // Action / Use Item
            UseEquippedItem();
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
    if (isGameOver) return;

    if (roundBannerTimer > 0.0f) {
        roundBannerTimer -= deltaTime;
    }

    if (outsidePenaltyTimer > 0.0f) {
        outsidePenaltyTimer -= deltaTime;
        if (outsidePenaltyTimer <= 0.0f) {
            isOutside = false;
        }
    }

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

    if (moveX != 0.0f || moveY != 0.0f) {
        lastFacingX = moveX;
        lastFacingY = moveY;
    }

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

    // Item Pickups
    for (auto& item : currentRoom.items) {
        if (item.spawned && SDL_HasRectIntersectionFloat(&catRect, &item.rect)) {
            item.spawned = false;
            equippedItem = item.type;
            itemAmmo = item.ammo;
            std::cout << "PICKED UP ITEM! Equipped Ammo: " << itemAmmo << std::endl;
        }
    }

    // Cat Touches Human Check (-50% Anger Soothe)
    if (currentRoom.humanPresent && human.touchCooldown <= 0.0f) {
        if (SDL_HasRectIntersectionFloat(&catRect, &human.rect)) {
            if (human.irritation > 0.0f) {
                human.irritation -= 50.0f;
                if (human.irritation < 0.0f) human.irritation = 0.0f;
                std::cout << "*CAT RUBBED HUMAN! Anger decreased by 50%! Current: " 
                          << human.irritation << "%*" << std::endl;
            }
            human.touchCooldown = 1.5f;
        }
    }

    // Update Subsystems
    UpdateHuman(deltaTime);
    UpdateMice(deltaTime);
    UpdateProjectiles(deltaTime);
}

void Gameplay::RenderItems(SDL_Renderer* renderer) {
    const RoomState& currentRoom = house[currentRoomID];

    for (const auto& item : currentRoom.items) {
        if (!item.spawned) continue;

        if (item.type == ItemType::Slingshot) {
            SDL_SetRenderDrawColor(renderer, 220, 60, 220, 255); // Magenta Slingshot
        } else if (item.type == ItemType::LaserBlaster) {
            SDL_SetRenderDrawColor(renderer, 60, 220, 240, 255); // Cyan Laser
        } else if (item.type == ItemType::ToyRobot) {
            SDL_SetRenderDrawColor(renderer, 60, 220, 60, 255);  // Lime Robot
        }
        SDL_RenderFillRect(renderer, &item.rect);
    }
}

void Gameplay::RenderProjectiles(SDL_Renderer* renderer) {
    for (const auto& p : activeProjectiles) {
        if (p.type == ItemType::Slingshot) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White Pellets
        } else if (p.type == ItemType::LaserBlaster) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);   // Cyan Beams
        }
        SDL_RenderFillRect(renderer, &p.rect);
    }
}

void Gameplay::RenderHUD(SDL_Renderer* renderer, TTF_Font* fontUI) {
    int windowWidth = 0, windowHeight = 0;
    SDL_GetRenderOutputSize(renderer, &windowWidth, &windowHeight);

    SDL_FRect hudBar{ 0.0f, 0.0f, (float)windowWidth, 50.0f };
    SDL_SetRenderDrawColor(renderer, 20, 25, 30, 230);
    SDL_RenderFillRect(renderer, &hudBar);

    SDL_Color white  = { 255, 255, 255, 255 };
    SDL_Color cyan   = { 80, 210, 240, 255 };
    SDL_Color yellow = { 240, 200, 40, 255 };
    SDL_Color red    = { 240, 60, 60, 255 };

    if (roundBannerTimer > 0.0f && fontUI) {
        std::string roundText = "ROUND " + std::to_string(roundNumber) + " START!";
        Helpers::RenderCenteredText(renderer, fontUI, roundText.c_str(), 
                                   (float)windowWidth / 2.0f, (float)windowHeight / 2.0f, yellow);
    }

    if (!fontUI) return;

    if (isGameOver) {
        Helpers::RenderCenteredText(renderer, fontUI, "GAME OVER! TAKEN TO THE POUND!", 
                                   (float)windowWidth / 2.0f, 25.0f, red);
        return;
    }

    const RoomState& activeRoom = house[currentRoomID];

    // Stats Text
    std::string roomText = "Room: " + activeRoom.name;
    std::string miceText = "Mice: " + std::to_string(activeRoom.mice.size()) + 
                           " (Total: " + std::to_string(GetTotalHouseMiceCount()) + ")";
    std::string livesText = "Chances: " + std::to_string(chancesRemaining);

    std::string itemText = "Item: ";
    if (equippedItem == ItemType::Slingshot) itemText += "Slingshot (" + std::to_string(itemAmmo) + ")";
    else if (equippedItem == ItemType::LaserBlaster) itemText += "Laser (" + std::to_string(itemAmmo) + ")";
    else if (equippedItem == ItemType::ToyRobot) itemText += "Robot (" + std::to_string(itemAmmo) + ")";
    else itemText += "None";

    Helpers::RenderCenteredText(renderer, fontUI, roomText.c_str(), 100.0f, 25.0f, cyan);
    Helpers::RenderCenteredText(renderer, fontUI, miceText.c_str(), (float)windowWidth * 0.32f, 25.0f, white);
    Helpers::RenderCenteredText(renderer, fontUI, itemText.c_str(), (float)windowWidth * 0.52f, 25.0f, yellow);

    // Human Anger Progress Bar
    float barWidth = 120.0f;
    float barHeight = 18.0f;
    float barX = (float)windowWidth * 0.76f - (barWidth / 2.0f);
    float barY = 16.0f;

    Helpers::RenderCenteredText(renderer, fontUI, "Anger:", barX - 35.0f, 25.0f, white);

    SDL_FRect bgBarRect{ barX, barY, barWidth, barHeight };
    SDL_SetRenderDrawColor(renderer, 50, 20, 20, 255);
    SDL_RenderFillRect(renderer, &bgBarRect);

    float fillWidth = (human.irritation / 100.0f) * barWidth;
    SDL_FRect fillBarRect{ barX, barY, fillWidth, barHeight };

    if (human.irritation >= 70.0f) {
        SDL_SetRenderDrawColor(renderer, 240, 60, 60, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 240, 180, 40, 255);
    }
    SDL_RenderFillRect(renderer, &fillBarRect);

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderRect(renderer, &bgBarRect);

    Helpers::RenderCenteredText(renderer, fontUI, livesText.c_str(), (float)windowWidth - 70.0f, 25.0f, white);
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

    // 3. Render Items
    RenderItems(renderer);

    // 4. Render Projectiles
    RenderProjectiles(renderer);

    // 5. Render Human
    RenderHuman(renderer);

    // 6. Render Mice
    RenderMice(renderer);

    // 7. Render Cat
    SDL_SetRenderDrawColor(renderer, 240, 130, 40, 255);
    SDL_RenderFillRect(renderer, &catRect);

    // 8. Render HUD
    RenderHUD(renderer, fontUI);
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

void Gameplay::SwitchRoom(RoomID nextRoom, RoomID previousRoom) {
    currentRoomID = nextRoom;

    for (const auto& door : house[currentRoomID].doors) {
        if (door.targetRoom == previousRoom) {
            catRect.x = door.rect.x;
            catRect.y = door.rect.y;

            if (door.rect.w > door.rect.h) {
                catRect.y += (door.rect.y < 200.0f) ? 50.0f : -50.0f;
            } else {
                catRect.x += (door.rect.x < 200.0f) ? 50.0f : -50.0f;
            }
            break;
        }
    }
}

void Gameplay::ThrowCatOutside() {
    chancesRemaining--;
    std::cout << "HUMAN THREW THE CAT OUTSIDE! Chances left: " << chancesRemaining << std::endl;

    if (chancesRemaining <= 0) {
        isGameOver = true;
        std::cout << "GAME OVER! The cat was taken to the pound." << std::endl;
        return;
    }

    isOutside = true;
    outsidePenaltyTimer = 5.0f;
    currentRoomID = RoomID::LivingRoom;
    catRect.x = 100.0f;
    catRect.y = 100.0f;
}

void Gameplay::IncreaseHumanIrritation(float amount) {
    if (!house[currentRoomID].humanPresent || isOutside) return;

    human.irritation += amount;
    if (human.irritation > 100.0f) human.irritation = 100.0f;

    human.decayDelayTimer = 3.0f;

    std::cout << "Human Irritation increased to: " << human.irritation << "%" << std::endl;

    if (human.irritation >= 100.0f) {
        human.irritation = 0.0f;
        ThrowCatOutside();
    }
}

void Gameplay::AdvanceToNextRound() {
    roundNumber++;
    roundBannerTimer = 3.0f;
    std::cout << "ROUND COMPLETED! Starting Round " << roundNumber << std::endl;

    human.irritation = 0.0f;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> miceChance(1 + (roundNumber / 2), 3 + (roundNumber / 2));
    std::uniform_real_distribution<float> posX(200.0f, 800.0f);
    std::uniform_real_distribution<float> posY(200.0f, 500.0f);

    for (auto& pair : house) {
        RoomState& room = pair.second;
        room.mice.clear();

        int count = miceChance(gen);
        for (int i = 0; i < count; ++i) {
            Mouse m;
            m.rect.x = posX(gen);
            m.rect.y = posY(gen);
            m.speed = 120.0f + (roundNumber * 10.0f);
            room.mice.push_back(m);
        }
        room.miceCount = (int)room.mice.size();
    }
}

void Gameplay::UpdateHuman(float deltaTime) {
    if (human.touchCooldown > 0.0f) {
        human.touchCooldown -= deltaTime;
    }

    // Irritation Decay Logic
    if (human.decayDelayTimer > 0.0f) {
        human.decayDelayTimer -= deltaTime;
    } 
    else if (human.irritation > 0.0f) {
        human.irritation -= 15.0f * deltaTime;
        if (human.irritation < 0.0f) human.irritation = 0.0f;
    }

    RoomState& currentRoom = house[humanRoomID];

    // Room Exit Trigger
    if (!human.isHeadingToDoor) {
        human.moveTimer -= deltaTime;

        if (human.moveTimer <= 0.0f && !currentRoom.doors.empty()) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<size_t> doorDist(0, currentRoom.doors.size() - 1);

            size_t doorIdx = doorDist(gen);
            const Door& exitDoor = currentRoom.doors[doorIdx];

            human.targetX = exitDoor.rect.x + (exitDoor.rect.w / 2.0f) - (human.rect.w / 2.0f);
            human.targetY = exitDoor.rect.y + (exitDoor.rect.h / 2.0f) - (human.rect.h / 2.0f);
            human.nextRoomID = exitDoor.targetRoom;
            human.isHeadingToDoor = true;
        }
    }

    // Human Movement Executions
    if (currentRoom.humanPresent) {
        float dx = human.targetX - human.rect.x;
        float dy = human.targetY - human.rect.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (human.isHeadingToDoor) {
            if (distance < 10.0f) {
                house[humanRoomID].humanPresent = false;
                humanRoomID = human.nextRoomID;
                house[humanRoomID].humanPresent = true;

                for (const auto& door : house[humanRoomID].doors) {
                    if (door.targetRoom == currentRoom.id) {
                        human.rect.x = door.rect.x;
                        human.rect.y = door.rect.y;
                        break;
                    }
                }

                human.isHeadingToDoor = false;
                human.targetX = human.rect.x;
                human.targetY = human.rect.y;
                human.moveTimer = 14.0f;
                human.irritation = 0.0f;
                human.decayDelayTimer = 0.0f;
            } else {
                human.rect.x += (dx / distance) * (human.wanderSpeed * 1.2f) * deltaTime;
                human.rect.y += (dy / distance) * (human.wanderSpeed * 1.2f) * deltaTime;
            }
        } 
        else {
            if (human.pauseTimer > 0.0f) {
                human.pauseTimer -= deltaTime;
            } else {
                if (distance < 5.0f) {
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_real_distribution<float> posX(150.0f, 1050.0f);
                    std::uniform_real_distribution<float> posY(150.0f, 550.0f);
                    std::uniform_real_distribution<float> pauseDist(1.0f, 3.0f);

                    human.targetX = posX(gen);
                    human.targetY = posY(gen);
                    human.pauseTimer = pauseDist(gen);
                } else {
                    human.rect.x += (dx / distance) * human.wanderSpeed * deltaTime;
                    human.rect.y += (dy / distance) * human.wanderSpeed * deltaTime;
                }
            }
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
            // Squeak Logic (+10% Irritation)
            m.squeakTimer -= deltaTime;
            if (m.squeakTimer <= 0.0f) {
                m.squeakTimer = 5.0f;
                if (currentRoom.humanPresent) {
                    IncreaseHumanIrritation(10.0f);
                }
            }

            // Distance check to Cat
            float centerX = m.rect.x + (m.rect.w / 2.0f);
            float centerY = m.rect.y + (m.rect.h / 2.0f);
            float catCenterX = catRect.x + (catRect.w / 2.0f);
            float catCenterY = catRect.y + (catRect.h / 2.0f);

            float dx = centerX - catCenterX;
            float dy = centerY - catCenterY;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < 200.0f) {
                m.state = MouseState::Flee;
            }

            if (m.state == MouseState::Flee) {
                if (distance > 0.0001f) {
                    m.dirX = dx / distance;
                    m.dirY = dy / distance;
                }
                m.speed = 180.0f;

                if (distance > 350.0f) {
                    m.state = MouseState::Wander;
                }
            } 
            else if (m.state == MouseState::Wander) {
                m.speed = 90.0f;
                m.changeDirTimer -= deltaTime;

                if (m.changeDirTimer <= 0.0f) {
                    m.dirX = dirDist(gen);
                    m.dirY = dirDist(gen);
                    m.changeDirTimer = timerDist(gen);
                }
            }

            m.rect.x += m.dirX * m.speed * deltaTime;
            m.rect.y += m.dirY * m.speed * deltaTime;

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

    for (int i = (int)caughtMiceIndices.size() - 1; i >= 0; --i) {
        size_t index = caughtMiceIndices[i];
        currentRoom.mice.erase(currentRoom.mice.begin() + index);
        miceKilled++;
        currentRoom.miceCount = (int)currentRoom.mice.size();
    }

    if (GetTotalHouseMiceCount() == 0) {
        AdvanceToNextRound();
    }
}

int Gameplay::GetTotalHouseMiceCount() const {
    int total = 0;
    for (const auto& pair : house) {
        total += (int)pair.second.mice.size();
    }
    return total;
}

void Gameplay::RenderHuman(SDL_Renderer* renderer) {
    if (house[currentRoomID].humanPresent) {
        SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255);
        SDL_RenderFillRect(renderer, &human.rect);
    }
}

void Gameplay::RenderMice(SDL_Renderer* renderer) {
    const RoomState& currentRoom = house[currentRoomID];

    for (const auto& mouse : currentRoom.mice) {
        if (mouse.state == MouseState::Flee) {
            SDL_SetRenderDrawColor(renderer, 230, 60, 60, 255);
        } else if (mouse.state == MouseState::Stunned) {
            SDL_SetRenderDrawColor(renderer, 220, 220, 60, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
        }
        SDL_RenderFillRect(renderer, &mouse.rect);
    }
}
