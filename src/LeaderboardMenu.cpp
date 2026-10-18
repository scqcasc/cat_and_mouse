#include "LeaderboardMenu.h"
#include <iostream>
#include "SoundManager.h"
#include <iomanip>
#include <sstream>

LeaderboardMenu::LeaderboardMenu() {
    fontTitle   = TTF_OpenFont("assets/fonts/static/Roboto-Regular.ttf", 36.0f);
    fontHeader  = TTF_OpenFont("assets/fonts/static/Roboto-Regular.ttf", 20.0f);
    fontRow     = TTF_OpenFont("assets/fonts/static/Roboto-Regular.ttf", 18.0f);
    fontSubtext = TTF_OpenFont("assets/fonts/static/Roboto-Regular.ttf", 16.0f);

    if (!fontTitle || !fontHeader || !fontRow || !fontSubtext) {
        std::cerr << "Failed to load LeaderboardMenu fonts: " << SDL_GetError() << std::endl;
    }
}

LeaderboardMenu::~LeaderboardMenu() {
    if (fontTitle)   TTF_CloseFont(fontTitle);
    if (fontHeader)  TTF_CloseFont(fontHeader);
    if (fontRow)     TTF_CloseFont(fontRow);
    if (fontSubtext) TTF_CloseFont(fontSubtext);
}


void LeaderboardMenu::HandleInput(const SDL_Event& event, GameState& currentState) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.key == SDLK_ESCAPE || event.key.key == SDLK_RETURN || event.key.key == SDLK_SPACE) {
            // Play exit sound effect when leaving the leaderboard
            SoundManager::Instance().Play(SoundEffect::Slingshot);
            currentState = GameState::MainMenu;
        }
    }
}

void LeaderboardMenu::RenderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, float x, float y, SDL_Color color, bool center) {
    if (!font || !text || text[0] == '\0') return;

    SDL_Surface* surface = TTF_RenderText_Blended(font, text, 0, color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        float dstX = center ? x - ((float)surface->w / 2.0f) : x;
        float dstY = y - ((float)surface->h / 2.0f);
        SDL_FRect dstRect{ dstX, dstY, (float)surface->w, (float)surface->h };

        SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
        SDL_DestroyTexture(texture);
    }
    SDL_DestroySurface(surface);
}

void LeaderboardMenu::Render(SDL_Renderer* renderer) {
    int windowWidth = 0, windowHeight = 0;
    SDL_GetRenderOutputSize(renderer, &windowWidth, &windowHeight);

    // 1. Semi-transparent backdrop plate
    SDL_FRect backdrop{ 80.0f, 40.0f, (float)windowWidth - 160.0f, (float)windowHeight - 80.0f };
    SDL_SetRenderDrawColor(renderer, 15, 15, 25, 240);
    SDL_RenderFillRect(renderer, &backdrop);

    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Gold border
    SDL_RenderRect(renderer, &backdrop);

    // 2. Title & Navigation Subtext
    SDL_Color gold  = { 255, 215, 0, 255 };
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color amber = { 235, 140, 20, 255 };
    SDL_Color gray  = { 180, 180, 190, 255 };

    RenderText(renderer, fontTitle, "HALL OF FAME", (float)windowWidth / 2.0f, backdrop.y + 40.0f, gold);
    RenderText(renderer, fontSubtext, "ESC / ENTER: Return to Main Menu", (float)windowWidth / 2.0f, backdrop.y + 75.0f, gray);

    // 3. Table Column Headers
    float startY = backdrop.y + 115.0f;
    float colRank   = backdrop.x + 40.0f;
    float colName   = backdrop.x + 140.0f;
    float colScore  = backdrop.x + 300.0f;
    float colRounds = backdrop.x + 480.0f;
    float colDate   = backdrop.x + 640.0f;

    RenderText(renderer, fontHeader, "RANK", colRank, startY, amber, false);
    RenderText(renderer, fontHeader, "NAME", colName, startY, amber, false);
    RenderText(renderer, fontHeader, "SCORE", colScore, startY, amber, false);
    RenderText(renderer, fontHeader, "ROUNDS", colRounds, startY, amber, false);
    RenderText(renderer, fontHeader, "DATE", colDate, startY, amber, false);

    // Divider Line
    SDL_SetRenderDrawColor(renderer, 100, 100, 120, 255);
    SDL_RenderLine(renderer, backdrop.x + 30.0f, startY + 18.0f, backdrop.x + backdrop.w - 30.0f, startY + 18.0f);

    // 4. Fetch Top 10 High Scores from SQLite DB
    auto topScores = ScoreDatabase::Instance().GetTopScores(10);
    float rowY = startY + 42.0f;
    float rowSpacing = 32.0f;

    if (topScores.empty()) {
        RenderText(renderer, fontRow, "NO HIGH SCORES YET!", (float)windowWidth / 2.0f, rowY + 60.0f, white);
        return;
    }

    for (size_t i = 0; i < topScores.size(); ++i) {
        const auto& entry = topScores[i];
        float currentY = rowY + (i * rowSpacing);

        // Highlight #1 Rank with Gold, Top 3 with Amber
        SDL_Color textColor = white;
        if (i == 0)      textColor = gold;
        else if (i < 3) textColor = amber;

        std::string rankStr = "#" + std::to_string(i + 1);
        std::string scoreStr = std::to_string(entry.score);
        std::string roundsStr = std::to_string(entry.levelsCompleted);

        RenderText(renderer, fontRow, rankStr.c_str(), colRank, currentY, textColor, false);
        RenderText(renderer, fontRow, entry.initials.c_str(), colName, currentY, textColor, false);
        RenderText(renderer, fontRow, scoreStr.c_str(), colScore, currentY, textColor, false);
        RenderText(renderer, fontRow, roundsStr.c_str(), colRounds, currentY, textColor, false);
        RenderText(renderer, fontRow, entry.dateTime.c_str(), colDate, currentY, textColor, false);
    }
}


