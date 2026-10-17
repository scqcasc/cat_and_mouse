#include "Gameplay.h"
#include "Helpers.h"
#include <iostream>
#include <random>
#include <cmath>
#include "SoundManager.h"

Gameplay::Gameplay() {
    InitializeHouse();
    currentRoomID = RoomID::LivingRoom;
}

Gameplay::~Gameplay() {}

void Gameplay::ProcessScoreDecay(float deltaTime) {
    // Only decay score if points have been scored and game is active
    if (score <= 0 || isGameOver) return;

    scoreDecayTimer += deltaTime;

    if (scoreDecayTimer >= scoreDecayRate) {
        scoreDecayTimer -= scoreDecayRate;
        score -= scoreDecayAmount;

        if (score < 0) {
            score = 0;
        }
    }
}

void Gameplay::DefineRoomTemplates() {
    roomTemplates.clear();

    // 1. Living Room Template
    RoomTemplate livingRoom;
    livingRoom.id = RoomID::LivingRoom;
    livingRoom.name = "Living Room";
    livingRoom.defaultItem = ItemType::Slingshot;
    livingRoom.baseItemAmmo = 5;
    livingRoom.doorConfigs = {
        { 1.0f, 0.5f, 20.0f, 80.0f, RoomID::Hallway, "To Hallway" },
        { 0.5f, 1.0f, 80.0f, 20.0f, RoomID::Kitchen, "To Kitchen" }
    };
    livingRoom.furniturePool = {
        { "Couch",        0.2f, 0.2f, 140.0f, 70.0f, FurnitureType::Moveable, 1.0f },
        { "Coffee Table", 0.3f, 0.45f, 80.0f,  40.0f, FurnitureType::Flippable, 0.8f },
        { "Bookcase",     0.05f, 0.25f, 40.0f, 100.0f, FurnitureType::Flippable, 0.7f },
        { "TV Stand",     0.55f, 0.05f, 100.0f, 35.0f, FurnitureType::Moveable,  0.9f },
        { "Armchair",     0.75f, 0.3f,  60.0f,  60.0f, FurnitureType::Moveable,  0.5f } // Dynamic extra
    };
    roomTemplates[RoomID::LivingRoom] = livingRoom;

    // 2. Kitchen Template
    RoomTemplate kitchen;
    kitchen.id = RoomID::Kitchen;
    kitchen.name = "Kitchen";
    kitchen.defaultItem = ItemType::ToyRobot;
    kitchen.baseItemAmmo = 2;
    kitchen.doorConfigs = {
        { 0.5f, 0.0f, 80.0f, 20.0f, RoomID::LivingRoom, "To Living Room" }
    };
    kitchen.furniturePool = {
        { "Stove",        0.05f, 0.05f, 70.0f, 70.0f, FurnitureType::Static,   1.0f },
        { "Refrigerator", 0.2f,  0.05f, 70.0f, 80.0f, FurnitureType::Static,   1.0f },
        { "Dining Table", 0.45f, 0.4f, 120.0f, 80.0f, FurnitureType::Flippable, 0.85f },
        { "Kitchen Island", 0.5f, 0.1f, 100.0f, 50.0f, FurnitureType::Static,   0.4f }
    };
    roomTemplates[RoomID::Kitchen] = kitchen;

    // 3. Hallway Template
    RoomTemplate hallway;
    hallway.id = RoomID::Hallway;
    hallway.name = "Hallway";
    hallway.doorConfigs = {
        { 0.0f, 0.5f, 20.0f, 80.0f, RoomID::LivingRoom, "To Living Room" },
        { 0.3f, 0.0f, 80.0f, 20.0f, RoomID::MasterBedroom, "To Master Bed" },
        { 0.7f, 0.0f, 80.0f, 20.0f, RoomID::GuestBedroom,  "To Guest Bed" }
    };
    hallway.furniturePool = {
        { "Lamp",        0.25f, 0.2f, 35.0f, 35.0f, FurnitureType::Moveable, 0.75f },
        { "Night Stand", 0.6f,  0.2f, 45.0f, 45.0f, FurnitureType::Moveable, 0.75f },
        { "Coat Rack",   0.85f, 0.2f, 30.0f, 30.0f, FurnitureType::Moveable, 0.5f }
    };
    roomTemplates[RoomID::Hallway] = hallway;

    // 4. Master Bedroom Template
    RoomTemplate masterBed;
    masterBed.id = RoomID::MasterBedroom;
    masterBed.name = "Master Bedroom";
    masterBed.defaultItem = ItemType::LaserBlaster;
    masterBed.baseItemAmmo = 3;
    masterBed.doorConfigs = {
        { 0.5f, 1.0f, 80.0f, 20.0f, RoomID::Hallway, "To Hallway" }
    };
    masterBed.furniturePool = {
        { "Bed",         0.2f, 0.2f, 120.0f, 150.0f, FurnitureType::Static,   1.0f },
        { "Night Stand", 0.08f, 0.2f, 45.0f,  45.0f,  FurnitureType::Moveable, 0.8f },
        { "Cabinet",     0.65f, 0.2f, 70.0f,  100.0f, FurnitureType::Moveable, 0.85f }
    };
    roomTemplates[RoomID::MasterBedroom] = masterBed;

    // 5. Guest Bedroom Template
    RoomTemplate guestBed;
    guestBed.id = RoomID::GuestBedroom;
    guestBed.name = "Guest Bedroom";
    guestBed.doorConfigs = {
        { 0.5f, 1.0f, 80.0f, 20.0f, RoomID::Hallway, "To Hallway" }
    };
    guestBed.furniturePool = {
        { "Bed",              0.3f, 0.2f, 120.0f, 150.0f, FurnitureType::Static,   1.0f },
        { "Chest of Drawers", 0.65f, 0.2f, 60.0f,  90.0f,  FurnitureType::Moveable, 0.8f }
    };
    roomTemplates[RoomID::GuestBedroom] = guestBed;
}

RoomState Gameplay::InstantiateRoomFromTemplate(const RoomTemplate& tmpl, float windowW, float windowH, int roundNum) {
    RoomState room;
    room.id = tmpl.id;
    room.name = tmpl.name;

    SDL_FRect bounds = GetRoomBounds(tmpl.id, windowW, windowH);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> chanceDist(0.0f, 1.0f);

    // 1. Instantiate Doors
    for (const auto& dConfig : tmpl.doorConfigs) {
        Door door;
        door.targetRoom = dConfig.targetRoom;
        door.label = dConfig.label;
        door.rect.w = dConfig.width;
        door.rect.h = dConfig.height;

        // Position flush to wall edges using relative placement ratios
        if (dConfig.relX == 0.0f)      door.rect.x = bounds.x;
        else if (dConfig.relX == 1.0f) door.rect.x = bounds.x + bounds.w - dConfig.width;
        else                           door.rect.x = bounds.x + (bounds.w * dConfig.relX) - (dConfig.width / 2.0f);

        if (dConfig.relY == 0.0f)      door.rect.y = bounds.y;
        else if (dConfig.relY == 1.0f) door.rect.y = bounds.y + bounds.h - dConfig.height;
        else                           door.rect.y = bounds.y + (bounds.h * dConfig.relY) - (dConfig.height / 2.0f);

        room.doors.push_back(door);
    }

    // 2. Instantiate Furniture based on spawn probabilities
    for (const auto& option : tmpl.furniturePool) {
        if (chanceDist(gen) <= option.spawnChance) {
            Furniture f;
            f.name = option.name;
            f.type = option.type;
            f.rect.w = option.width;
            f.rect.h = option.height;
            f.rect.x = bounds.x + (bounds.w * option.relX);
            f.rect.y = bounds.y + (bounds.h * option.relY);

            room.furniture.push_back(f);
        }
    }

    // 3. Instantiate Items
    if (tmpl.defaultItem != ItemType::None) {
        std::uniform_real_distribution<float> posX(bounds.x + 60.0f, bounds.x + bounds.w - 80.0f);
        std::uniform_real_distribution<float> posY(bounds.y + 60.0f, bounds.y + bounds.h - 80.0f);

        Item item;
        item.type = tmpl.defaultItem;
        item.ammo = tmpl.baseItemAmmo;
        item.rect = { posX(gen), posY(gen), 22.0f, 22.0f };
        item.spawned = true;

        room.items.push_back(item);
    }

    // 4. Instantiate Mice
    std::uniform_int_distribution<int> miceChance(1 + (roundNum / 2), 3 + (roundNum / 2));
    int mouseCount = miceChance(gen);

    std::uniform_real_distribution<float> mouseX(bounds.x + 40.0f, bounds.x + bounds.w - 60.0f);
    std::uniform_real_distribution<float> mouseY(bounds.y + 40.0f, bounds.y + bounds.h - 60.0f);

    for (int i = 0; i < mouseCount; ++i) {
        Mouse m;
        m.rect.x = mouseX(gen);
        m.rect.y = mouseY(gen);
        m.speed = 120.0f + (roundNum * 10.0f);
        room.mice.push_back(m);
    }
    room.miceCount = (int)room.mice.size();

    return room;
}

void Gameplay::InitializeHouse(float windowWidth, float windowHeight) {
    house.clear();

    if (roomTemplates.empty()) {
        DefineRoomTemplates();
    }

    for (const auto& pair : roomTemplates) {
        house[pair.first] = InstantiateRoomFromTemplate(pair.second, windowWidth, windowHeight, roundNumber);
    }

    house[RoomID::LivingRoom].humanPresent = true;

    SDL_FRect roomBounds = GetRoomBounds(RoomID::LivingRoom, windowWidth, windowHeight);
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
        SoundManager::Instance().Play(SoundEffect::Slingshot);
        p.speed = 550.0f;
    } else if (equippedItem == ItemType::LaserBlaster) {
        SoundManager::Instance().Play(SoundEffect::Laser);
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

    // ... round banner & outside penalty timer updates ...

    ProcessScoreDecay(deltaTime);

    // --- Cat Stun Handling ---
    if (stunTimer > 0.0f) {
        stunTimer -= deltaTime;

        // Play stun sound ONCE at the start of the stun
        if (!wasStunSoundPlayed) {
            SoundManager::Instance().Play(SoundEffect::Stun);
            wasStunSoundPlayed = true;
        }

        // When stun expires, clear audio stream and reset flag
        if (stunTimer <= 0.0f) {
            stunTimer = 0.0f;
            wasStunSoundPlayed = false;
            SoundManager::Instance().StopAll(); // Flush lingering audio!
        }
    } else {
        wasStunSoundPlayed = false;

        ProcessCatMovement(deltaTime);
        CheckDoorTransitions();

        SDL_FRect bounds = GetRoomBounds(currentRoomID, windowWidth, windowHeight);
        ClampCatToBounds(bounds);

        CheckItemPickups();
        CheckCatHumanCollision();
    }

    // --- Subsystems ---
    UpdateHuman(deltaTime);
    UpdateMice(deltaTime);
    UpdateProjectiles(deltaTime);
}

void Gameplay::ProcessCatMovement(float deltaTime) {
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
    SDL_FRect bounds = GetRoomBounds(currentRoomID, windowWidth, windowHeight);

    // X Axis Movement & Furniture Push
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
                } else if (item.rect.x + item.rect.w > bounds.x + bounds.w) {
                    item.rect.x = bounds.x + bounds.w - item.rect.w;
                    catRect.x = item.rect.x - catRect.w;
                }
            }
        }
    }

    // Y Axis Movement & Furniture Push
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
                } else if (item.rect.y + item.rect.h > bounds.y + bounds.h) {
                    item.rect.y = bounds.y + bounds.h - item.rect.h;
                    catRect.y = item.rect.y - catRect.h;
                }
            }
        }
    }
}

void Gameplay::CheckDoorTransitions() {
    RoomState& currentRoom = house[currentRoomID];
    for (const auto& door : currentRoom.doors) {
        if (SDL_HasRectIntersectionFloat(&catRect, &door.rect)) {
            moveLeftPressed = moveRightPressed = moveUpPressed = moveDownPressed = false;
            SwitchRoom(door.targetRoom, currentRoomID);
            break;
        }
    }
}

void Gameplay::ClampCatToBounds(const SDL_FRect& bounds) {
    if (catRect.x < bounds.x) catRect.x = bounds.x;
    if (catRect.x + catRect.w > bounds.x + bounds.w) catRect.x = bounds.x + bounds.w - catRect.w;
    if (catRect.y < bounds.y) catRect.y = bounds.y;
    if (catRect.y + catRect.h > bounds.y + bounds.h) catRect.y = bounds.y + bounds.h - catRect.h;
}

void Gameplay::CheckItemPickups() {
    RoomState& currentRoom = house[currentRoomID];
    SDL_FRect catPickupBounds{ catRect.x - 5.0f, catRect.y - 5.0f, catRect.w + 10.0f, catRect.h + 10.0f };

    for (auto& item : currentRoom.items) {
        if (item.spawned && SDL_HasRectIntersectionFloat(&catPickupBounds, &item.rect)) {
            item.spawned = false;
            equippedItem = item.type;
            itemAmmo = item.ammo;
            std::cout << "*CAT PICKED UP ITEM! Equipped Ammo: " << itemAmmo << "*" << std::endl;
        }
    }
}

void Gameplay::CheckCatHumanCollision() {
    RoomState& currentRoom = house[currentRoomID];
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
    int w = 0, h = 0;
    SDL_GetRenderOutputSize(renderer, &w, &h);
    float winWidth = (float)w;

    // Top HUD Bar Background
    SDL_FRect hudBar{ 0.0f, 0.0f, winWidth, 50.0f };
    SDL_SetRenderDrawColor(renderer, 20, 25, 30, 230);
    SDL_RenderFillRect(renderer, &hudBar);

    SDL_Color white  = { 255, 255, 255, 255 };
    SDL_Color cyan   = { 80, 210, 240, 255 };
    SDL_Color yellow = { 240, 200, 40, 255 };
    SDL_Color green  = { 80, 220, 100, 255 };
    SDL_Color red    = { 240, 60, 60, 255 };

    if (roundBannerTimer > 0.0f && fontUI) {
        std::string roundText = "ROUND " + std::to_string(roundNumber) + " START!";
        Helpers::RenderCenteredText(renderer, fontUI, roundText.c_str(), 
                                   winWidth / 2.0f, (float)h / 2.0f, yellow);
    }

    if (!fontUI) return;

    if (isGameOver) {
        std::string gameOverText = "GAME OVER! TAKEN TO THE POUND! FINAL SCORE: " + std::to_string(score);
        Helpers::RenderCenteredText(renderer, fontUI, gameOverText.c_str(), 
                                   winWidth / 2.0f, 25.0f, red);
        return;
    }

    const RoomState& activeRoom = house[currentRoomID];

    // Format Arcade HUD strings
    std::string scoreText = "SCORE: " + std::to_string(score);
    std::string hiScoreText = "HI: " + std::to_string(highScore);
    std::string roomText = "Room: " + activeRoom.name;
    std::string miceText = "Mice: " + std::to_string(activeRoom.mice.size()) + 
                           " (" + std::to_string(GetTotalHouseMiceCount()) + ")";

    std::string itemText = "Item: ";
    if (equippedItem == ItemType::Slingshot) itemText += "Slingshot (" + std::to_string(itemAmmo) + ")";
    else if (equippedItem == ItemType::LaserBlaster) itemText += "Laser (" + std::to_string(itemAmmo) + ")";
    else if (equippedItem == ItemType::ToyRobot) itemText += "Robot (" + std::to_string(itemAmmo) + ")";
    else itemText += "None";

    std::string livesText = "Chances: " + std::to_string(chancesRemaining);

    // Render Stats horizontally across HUD bar
    Helpers::RenderCenteredText(renderer, fontUI, scoreText.c_str(), winWidth * 0.08f, 25.0f, green);
    Helpers::RenderCenteredText(renderer, fontUI, hiScoreText.c_str(), winWidth * 0.18f, 25.0f, yellow);
    Helpers::RenderCenteredText(renderer, fontUI, roomText.c_str(), winWidth * 0.32f, 25.0f, cyan);
    Helpers::RenderCenteredText(renderer, fontUI, miceText.c_str(), winWidth * 0.48f, 25.0f, white);
    Helpers::RenderCenteredText(renderer, fontUI, itemText.c_str(), winWidth * 0.64f, 25.0f, yellow);

    // Human Anger Meter
    float barWidth = 100.0f;
    float barHeight = 16.0f;
    float barX = winWidth * 0.82f - (barWidth / 2.0f);
    float barY = 17.0f;

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

    // Remaining Lives
    Helpers::RenderCenteredText(renderer, fontUI, livesText.c_str(), winWidth - 60.0f, 25.0f, white);
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
    SoundManager::Instance().Play(SoundEffect::Meow);
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

void Gameplay::AddScore(int points) {
    score += points;
    if (score > highScore) {
        highScore = score;
    }
}

void Gameplay::AdvanceToNextRound() {
    SoundManager::Instance().Play(SoundEffect::RoundClear);
    // Award round completion bonus
    AddScore(500 * roundNumber);

    roundNumber++;
    roundBannerTimer = 3.0f;
    std::cout << "ROUND COMPLETED! Starting Round " << roundNumber << " | Current Score: " << score << std::endl;

    human.irritation = 0.0f;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> miceChance(1 + (roundNumber / 2), 3 + (roundNumber / 2));
    std::uniform_int_distribution<int> itemTypeDist(1, 3);

    for (auto& pair : house) {
        RoomState& room = pair.second;
        room.mice.clear();
        room.items.clear();

        SDL_FRect roomBounds = GetRoomBounds(room.id, windowWidth, windowHeight);

        int mouseCount = miceChance(gen);
        std::uniform_real_distribution<float> posX(roomBounds.x + 40.0f, roomBounds.x + roomBounds.w - 60.0f);
        std::uniform_real_distribution<float> posY(roomBounds.y + 40.0f, roomBounds.y + roomBounds.h - 60.0f);

        for (int i = 0; i < mouseCount; ++i) {
            Mouse m;
            m.rect.x = posX(gen);
            m.rect.y = posY(gen);
            m.speed = 120.0f + (roundNumber * 10.0f);
            room.mice.push_back(m);
        }
        room.miceCount = (int)room.mice.size();

        int itemCount = 1 + (roundNumber / 2);
        for (int i = 0; i < itemCount; ++i) {
            Item newItem;
            newItem.rect.x = posX(gen);
            newItem.rect.y = posY(gen);
            newItem.rect.w = 22.0f;
            newItem.rect.h = 22.0f;
            newItem.type = static_cast<ItemType>(itemTypeDist(gen));
            newItem.ammo = 3 + roundNumber;
            newItem.spawned = true;

            room.items.push_back(newItem);
        }
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
            SoundManager::Instance().Play(SoundEffect::Stun);
            m.stunTimer -= deltaTime;
            if (m.stunTimer <= 0.0f) {
                m.state = MouseState::Wander;
            }
        } else {
            // Squeak Logic (+10% Irritation)
            m.squeakTimer -= deltaTime;
            if (m.squeakTimer <= 0.0f) {
                m.squeakTimer = 5.0f;
                SoundManager::Instance().Play(SoundEffect::Squeak);
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
        // Bonus points for taking down an armed mouse
        if (currentRoom.mice[index].equippedItem != ItemType::None) {
            AddScore(250);
        } else {
            AddScore(100);
        }
        currentRoom.mice.erase(currentRoom.mice.begin() + index);
        miceKilled++;
        currentRoom.miceCount = (int)currentRoom.mice.size();

        // Flush sound queue when stunned mouse is defeated
        SoundManager::Instance().StopAll();
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
                SoundManager::Instance().Play(SoundEffect::Stun);
                std::cout << "*CAT WAS STUNNED BY MOUSE PROJECTILE!*" << std::endl;
                expiredProjectiles.push_back(i);
            }
        } else {
            // Cat projectile hits Mouse -> Stun Mouse!
            for (auto& mouse : currentRoom.mice) {
                if (SDL_HasRectIntersectionFloat(&p.rect, &mouse.rect)) {
                    mouse.state = MouseState::Stunned;
                    mouse.stunTimer = 4.0f;
                    SoundManager::Instance().Play(SoundEffect::Stun);
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
