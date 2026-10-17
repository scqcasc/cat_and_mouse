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


void Gameplay::InitializeHouse(float windowWidth, float windowHeight) {
    house.clear();

    // Fallback safety if 0 or uninitialized dimensions are passed
    if (windowWidth <= 0.0f) windowWidth = 1280.0f;
    if (windowHeight <= 0.0f) windowHeight = 720.0f;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> miceChance(1, 3);

    // Compute exact room bounds based on live window size
    SDL_FRect roomBounds = GetRoomBounds(RoomID::LivingRoom, windowWidth, windowHeight);
    SDL_FRect hallwayBounds = GetRoomBounds(RoomID::Hallway, windowWidth, windowHeight);

    auto PopulateMice = [&](int count, const SDL_FRect& bounds) {
        std::vector<Mouse> miceList;
        std::uniform_real_distribution<float> posX(bounds.x + 40.0f, bounds.x + bounds.w - 60.0f);
        std::uniform_real_distribution<float> posY(bounds.y + 40.0f, bounds.y + bounds.h - 60.0f);
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
    livingRoom.humanPresent = true;
    livingRoom.mice = PopulateMice(miceChance(gen), roomBounds);
    livingRoom.items = { { { roomBounds.x + roomBounds.w - 80.0f, roomBounds.y + 80.0f, 22.0f, 22.0f }, ItemType::Slingshot, 5, true } };
    livingRoom.furniture = {
        { "Couch",        { roomBounds.x + 100.0f, roomBounds.y + 80.0f,  140.0f, 70.0f }, FurnitureType::Moveable },
        { "Coffee Table", { roomBounds.x + 140.0f, roomBounds.y + 180.0f, 80.0f,  40.0f }, FurnitureType::Flippable },
        { "Bookcase",     { roomBounds.x + 20.0f,  roomBounds.y + 100.0f, 40.0f, 100.0f }, FurnitureType::Flippable },
        { "TV",           { roomBounds.x + 280.0f, roomBounds.y + 20.0f,  100.0f, 35.0f }, FurnitureType::Moveable }
    };
    livingRoom.doors = {
        { { roomBounds.x + roomBounds.w - 20.0f, roomBounds.y + (roomBounds.h / 2.0f) - 40.0f, 20.0f, 80.0f }, RoomID::Hallway, "To Hallway" },
        { { roomBounds.x + (roomBounds.w / 2.0f) - 40.0f, roomBounds.y + roomBounds.h - 20.0f, 80.0f, 20.0f },  RoomID::Kitchen, "To Kitchen" }
    };
    house[RoomID::LivingRoom] = livingRoom;

    // 2. Kitchen
    RoomState kitchen;
    kitchen.id = RoomID::Kitchen;
    kitchen.name = "Kitchen";
    kitchen.mice = PopulateMice(miceChance(gen), roomBounds);
    kitchen.items = { { { roomBounds.x + 80.0f, roomBounds.y + roomBounds.h - 80.0f, 22.0f, 22.0f }, ItemType::ToyRobot, 2, true } };
    kitchen.furniture = {
        { "Stove",        { roomBounds.x + 30.0f,  roomBounds.y + 30.0f,  70.0f, 70.0f }, FurnitureType::Static },
        { "Refrigerator", { roomBounds.x + 120.0f, roomBounds.y + 30.0f,  70.0f, 80.0f }, FurnitureType::Static },
        { "Dining Table", { roomBounds.x + 260.0f, roomBounds.y + 180.0f, 120.0f, 80.0f }, FurnitureType::Flippable }
    };
    kitchen.doors = {
        { { roomBounds.x + (roomBounds.w / 2.0f) - 40.0f, roomBounds.y, 80.0f, 20.0f }, RoomID::LivingRoom, "To Living Room" }
    };
    house[RoomID::Kitchen] = kitchen;

    // 3. Hallway (Narrow Viewport)
    RoomState hallway;
    hallway.id = RoomID::Hallway;
    hallway.name = "Hallway";
    hallway.mice = PopulateMice(miceChance(gen), hallwayBounds);
    hallway.furniture = {
        { "Lamp",       { hallwayBounds.x + 180.0f, hallwayBounds.y + 40.0f, 35.0f, 35.0f }, FurnitureType::Moveable },
        { "Night Stand",{ hallwayBounds.x + 420.0f, hallwayBounds.y + 40.0f, 45.0f, 45.0f }, FurnitureType::Moveable }
    };
    hallway.doors = {
        { { hallwayBounds.x, hallwayBounds.y + (hallwayBounds.h / 2.0f) - 40.0f, 20.0f, 80.0f }, RoomID::LivingRoom, "To Living Room" },
        { { hallwayBounds.x + 180.0f, hallwayBounds.y, 80.0f, 20.0f }, RoomID::MasterBedroom, "To Master Bed" },
        { { hallwayBounds.x + 500.0f, hallwayBounds.y, 80.0f, 20.0f }, RoomID::GuestBedroom,  "To Guest Bed" }
    };
    house[RoomID::Hallway] = hallway;

    // 4. Master Bedroom
    RoomState masterBed;
    masterBed.id = RoomID::MasterBedroom;
    masterBed.name = "Master Bedroom";
    masterBed.mice = PopulateMice(miceChance(gen), roomBounds);
    masterBed.items = { { { roomBounds.x + 380.0f, roomBounds.y + 100.0f, 22.0f, 22.0f }, ItemType::LaserBlaster, 3, true } };
    masterBed.furniture = {
        { "Bed",         { roomBounds.x + 100.0f, roomBounds.y + 80.0f, 120.0f, 150.0f }, FurnitureType::Static },
        { "Night Stand", { roomBounds.x + 40.0f,  roomBounds.y + 80.0f, 45.0f, 45.0f },   FurnitureType::Moveable },
        { "Cabinet",     { roomBounds.x + 350.0f, roomBounds.y + 80.0f, 70.0f, 100.0f },  FurnitureType::Moveable }
    };
    masterBed.doors = {
        { { roomBounds.x + (roomBounds.w / 2.0f) - 40.0f, roomBounds.y + roomBounds.h - 20.0f, 80.0f, 20.0f }, RoomID::Hallway, "To Hallway" }
    };
    house[RoomID::MasterBedroom] = masterBed;

    // 5. Guest Bedroom
    RoomState guestBed;
    guestBed.id = RoomID::GuestBedroom;
    guestBed.name = "Guest Bedroom";
    guestBed.mice = PopulateMice(miceChance(gen), roomBounds);
    guestBed.furniture = {
        { "Bed",             { roomBounds.x + 180.0f, roomBounds.y + 80.0f, 120.0f, 150.0f }, FurnitureType::Static },
        { "Chest of Drawers",{ roomBounds.x + 380.0f, roomBounds.y + 80.0f, 60.0f, 90.0f },   FurnitureType::Moveable }
    };
    guestBed.doors = {
        { { roomBounds.x + (roomBounds.w / 2.0f) - 40.0f, roomBounds.y + roomBounds.h - 20.0f, 80.0f, 20.0f }, RoomID::Hallway, "To Hallway" }
    };
    house[RoomID::GuestBedroom] = guestBed;

    // Place Cat inside initial Living Room bounds
    catRect.x = roomBounds.x + 100.0f;
    catRect.y = roomBounds.y + 200.0f;
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
    if (isGameOver || !isHouseInitialized) return;

    if (roundBannerTimer > 0.0f) {
        roundBannerTimer -= deltaTime;
    }

    if (outsidePenaltyTimer > 0.0f) {
        outsidePenaltyTimer -= deltaTime;
        if (outsidePenaltyTimer <= 0.0f) {
            isOutside = false;
        }
    }

    RoomState& currentRoom = house[currentRoomID];
    SDL_FRect bounds = GetRoomBounds(currentRoomID, windowWidth, windowHeight);

    // --- Only Process Cat Movement & Input if NOT Stunned ---
    if (stunTimer > 0.0f) {
        stunTimer -= deltaTime; // Countdown stun clock while game world runs!
    } else {
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

        // 1. Cat Movement & Furniture Collision with Wall Clamping
        catRect.x += moveX * catSpeed * deltaTime;
        for (auto& item : currentRoom.furniture) {
            if (SDL_HasRectIntersectionFloat(&catRect, &item.rect)) {
                if (item.type == FurnitureType::Static) {
                    if (moveX > 0) catRect.x = item.rect.x - catRect.w;
                    else if (moveX < 0) catRect.x = item.rect.x + item.rect.w;
                } else {
                    item.rect.x += moveX * (catSpeed * 0.4f) * deltaTime;
                    if (item.rect.x < bounds.x) {
                        item.rect.x = bounds.x;
                        catRect.x = item.rect.x + item.rect.w;
                    }
                    else if (item.rect.x + item.rect.w > bounds.x + bounds.w) {
                        item.rect.x = bounds.x + bounds.w - item.rect.w;
                        catRect.x = item.rect.x - catRect.w;
                    }
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
                    if (item.rect.y < bounds.y) {
                        item.rect.y = bounds.y;
                        catRect.y = item.rect.y + item.rect.h;
                    }
                    else if (item.rect.y + item.rect.h > bounds.y + bounds.h) {
                        item.rect.y = bounds.y + bounds.h - item.rect.h;
                        catRect.y = item.rect.y - catRect.h;
                    }
                }
            }
        }

        // 2. Door Transitions
        bool isPassingThroughDoor = false;
        for (const auto& door : currentRoom.doors) {
            if (SDL_HasRectIntersectionFloat(&catRect, &door.rect)) {
                isPassingThroughDoor = true;
                moveLeftPressed = moveRightPressed = moveUpPressed = moveDownPressed = false;
                SwitchRoom(door.targetRoom, currentRoomID);
                break;
            }
        }

        // 3. Cat Wall Clamping
        if (!isPassingThroughDoor) {
            if (catRect.x < bounds.x) catRect.x = bounds.x;
            if (catRect.x + catRect.w > bounds.x + bounds.w) catRect.x = bounds.x + bounds.w - catRect.w;
            if (catRect.y < bounds.y) catRect.y = bounds.y;
            if (catRect.y + catRect.h > bounds.y + bounds.h) catRect.y = bounds.y + bounds.h - catRect.h;
        }

        // 4. Cat Item Pickups
        SDL_FRect catPickupBounds{ catRect.x - 5.0f, catRect.y - 5.0f, catRect.w + 10.0f, catRect.h + 10.0f };
        for (auto& item : currentRoom.items) {
            if (item.spawned && SDL_HasRectIntersectionFloat(&catPickupBounds, &item.rect)) {
                item.spawned = false;
                equippedItem = item.type;
                itemAmmo = item.ammo;
                std::cout << "*CAT PICKED UP ITEM! Equipped Ammo: " << itemAmmo << "*" << std::endl;
            }
        }

        // 5. Cat Touches Human Check (-50% Anger Soothe)
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
    }

    // --- Subsystems Update Continually (Even when Cat is Stunned) ---
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
        if (p.isFiredByMouse) {
            SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255); // Hostile Yellow Shots
        } else if (p.type == ItemType::Slingshot) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Friendly White Pellets
        } else if (p.type == ItemType::LaserBlaster) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);   // Friendly Cyan Beams
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
    int w = 0, h = 0;
    SDL_GetRenderOutputSize(renderer, &w, &h);

    if (w > 0 && h > 0) {
        windowWidth = (float)w;
        windowHeight = (float)h;
    }

    // Initialize house layout once we know the exact renderer output size
    if (!isHouseInitialized) {
        InitializeHouse(windowWidth, windowHeight);
        isHouseInitialized = true;
    }

    SDL_FRect bounds = GetRoomBounds(currentRoomID, windowWidth, windowHeight);

    // 1. Draw Outer Border Wall
    SDL_FRect wallBorder{ bounds.x - 8.0f, bounds.y - 8.0f, bounds.w + 16.0f, bounds.h + 16.0f };
    SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
    SDL_RenderFillRect(renderer, &wallBorder);

    // 2. Draw Green Carpet Floor
    SDL_SetRenderDrawColor(renderer, 38, 102, 51, 255);
    SDL_RenderFillRect(renderer, &bounds);

    // 3. Render Room Contents
    const RoomState& currentRoom = house[currentRoomID];

    SDL_SetRenderDrawColor(renderer, 140, 80, 30, 255);
    for (const auto& door : currentRoom.doors) {
        SDL_RenderFillRect(renderer, &door.rect);
    }

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

    RenderItems(renderer);
    RenderProjectiles(renderer);
    RenderHuman(renderer);
    RenderMice(renderer);

    // Render Cat
    SDL_SetRenderDrawColor(renderer, 240, 130, 40, 255);
    SDL_RenderFillRect(renderer, &catRect);

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

    SDL_FRect newBounds = GetRoomBounds(currentRoomID, windowWidth, windowHeight);

    bool doorFound = false;
    for (const auto& door : house[currentRoomID].doors) {
        if (door.targetRoom == previousRoom) {
            doorFound = true;
            catRect.x = door.rect.x + (door.rect.w / 2.0f) - (catRect.w / 2.0f);
            catRect.y = door.rect.y + (door.rect.h / 2.0f) - (catRect.h / 2.0f);

            if (door.rect.w > door.rect.h) { // Top or Bottom door
                if (door.rect.y <= newBounds.y + 20.0f) {
                    catRect.y = newBounds.y + 25.0f;
                } else {
                    catRect.y = newBounds.y + newBounds.h - catRect.h - 25.0f;
                }
            } else { // Left or Right door
                if (door.rect.x <= newBounds.x + 20.0f) {
                    catRect.x = newBounds.x + 25.0f;
                } else {
                    catRect.x = newBounds.x + newBounds.w - catRect.w - 25.0f;
                }
            }
            break;
        }
    }

    if (!doorFound) {
        catRect.x = newBounds.x + (newBounds.w / 2.0f) - (catRect.w / 2.0f);
        catRect.y = newBounds.y + (newBounds.h / 2.0f) - (catRect.h / 2.0f);
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
    SDL_FRect bounds = GetRoomBounds(humanRoomID, 1280.0f, 720.0f);

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
                    // Pick wander targets strictly inside active room bounds
                    std::uniform_real_distribution<float> posX(bounds.x + 40.0f, bounds.x + bounds.w - 60.0f);
                    std::uniform_real_distribution<float> posY(bounds.y + 40.0f, bounds.y + bounds.h - 80.0f);
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
    SDL_FRect bounds = GetRoomBounds(currentRoomID, windowWidth, windowHeight);
    std::vector<size_t> caughtMiceIndices;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dirDist(-1.0f, 1.0f);
    std::uniform_real_distribution<float> timerDist(1.5f, 4.0f);

    for (size_t i = 0; i < currentRoom.mice.size(); ++i) {
        Mouse& m = currentRoom.mice[i];

        // Item Scavenging Check
        if (m.state != MouseState::Stunned && m.equippedItem == ItemType::None) {
            for (auto& item : currentRoom.items) {
                if (item.spawned && SDL_HasRectIntersectionFloat(&m.rect, &item.rect)) {
                    item.spawned = false;
                    m.equippedItem = item.type;
                    m.ammo = item.ammo;
                    m.shootCooldown = 1.0f;
                    std::cout << "*MOUSE PICKED UP AN ITEM! IT'S ARMED!*" << std::endl;
                    break;
                }
            }
        }

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

            // Vector math to Cat
            float centerX = m.rect.x + (m.rect.w / 2.0f);
            float centerY = m.rect.y + (m.rect.h / 2.0f);
            float catCenterX = catRect.x + (catRect.w / 2.0f);
            float catCenterY = catRect.y + (catRect.h / 2.0f);

            float dx = catCenterX - centerX;
            float dy = catCenterY - centerY;
            float distance = std::sqrt(dx * dx + dy * dy);

            // Armed Mouse Firing Logic
            if (m.equippedItem != ItemType::None && m.ammo > 0) {
                m.shootCooldown -= deltaTime;
                if (distance < 350.0f && m.shootCooldown <= 0.0f) {
                    m.shootCooldown = 2.0f;
                    m.ammo--;

                    Projectile p;
                    p.rect.x = centerX - 4.0f;
                    p.rect.y = centerY - 4.0f;
                    p.dirX = dx / distance;
                    p.dirY = dy / distance;
                    p.speed = (m.equippedItem == ItemType::LaserBlaster) ? 750.0f : 450.0f;
                    p.type = m.equippedItem;
                    p.isFiredByMouse = true;

                    activeProjectiles.push_back(p);
                    std::cout << "*ARMED MOUSE SHOT AT THE CAT!*" << std::endl;

                    if (m.ammo <= 0) {
                        m.equippedItem = ItemType::None;
                    }
                }
            }

            // Flee / Wander logic
            if (distance < 200.0f) {
                m.state = MouseState::Flee;
            }

            if (m.state == MouseState::Flee) {
                if (distance > 0.0001f) {
                    m.dirX = -dx / distance;
                    m.dirY = -dy / distance;
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

            // Room Boundary Clamping
            if (m.rect.x < bounds.x) { m.rect.x = bounds.x; m.dirX *= -1.0f; }
            if (m.rect.x + m.rect.w > bounds.x + bounds.w) { m.rect.x = bounds.x + bounds.w - m.rect.w; m.dirX *= -1.0f; }
            if (m.rect.y < bounds.y) { m.rect.y = bounds.y; m.dirY *= -1.0f; }
            if (m.rect.y + m.rect.h > bounds.y + bounds.h) { m.rect.y = bounds.y + bounds.h - m.rect.h; m.dirY *= -1.0f; }
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

        if (p.isFiredByMouse) {
            // Mouse projectile hits Cat -> Stun Cat!
            if (SDL_HasRectIntersectionFloat(&p.rect, &catRect)) {
                stunTimer = 3.0f; // Cat stunned for 3 seconds
                std::cout << "*CAT WAS STUNNED BY MOUSE PROJECTILE!*" << std::endl;
                expiredProjectiles.push_back(i);
            }
        } else {
            // Cat projectile hits Mouse -> Stun Mouse!
            for (auto& mouse : currentRoom.mice) {
                if (SDL_HasRectIntersectionFloat(&p.rect, &mouse.rect)) {
                    mouse.state = MouseState::Stunned;
                    mouse.stunTimer = 4.0f;
                    std::cout << "MOUSE STUNNED BY CAT PROJECTILE!" << std::endl;
                    expiredProjectiles.push_back(i);
                    break;
                }
            }
        }
    }

    for (int i = (int)expiredProjectiles.size() - 1; i >= 0; --i) {
        size_t index = expiredProjectiles[i];
        if (index < activeProjectiles.size()) {
            activeProjectiles.erase(activeProjectiles.begin() + index);
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
            SDL_SetRenderDrawColor(renderer, 230, 60, 60, 255); // Red
        } else if (mouse.state == MouseState::Stunned) {
            SDL_SetRenderDrawColor(renderer, 220, 220, 60, 255); // Yellow
        } else if (mouse.equippedItem != ItemType::None) {
            SDL_SetRenderDrawColor(renderer, 255, 128, 0, 255);  // Bright Orange for Armed Mice
        } else {
            SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255); // Light Grey
        }
        SDL_RenderFillRect(renderer, &mouse.rect);
    }
}

SDL_FRect Gameplay::GetRoomBounds(RoomID roomID, float windowWidth, float windowHeight) const {
    // If window size hasn't been retrieved properly yet, fallback to 1280x720 baseline
    if (windowWidth <= 0.0f) windowWidth = 1280.0f;
    if (windowHeight <= 0.0f) windowHeight = 720.0f;

    float maxRoomW = windowWidth * (2.0f / 3.0f);
    float maxRoomH = windowHeight * (2.0f / 3.0f);
    float startX = (windowWidth - maxRoomW) / 2.0f;
    float startY = (windowHeight - maxRoomH) / 2.0f + 25.0f; // Offset 25px down for top HUD bar

    if (roomID == RoomID::Hallway) {
        float hallwayH = maxRoomH * 0.5f;
        float hallwayY = startY + (maxRoomH - hallwayH) / 2.0f;
        return SDL_FRect{ startX, hallwayY, maxRoomW, hallwayH };
    }

    return SDL_FRect{ startX, startY, maxRoomW, maxRoomH };
}
