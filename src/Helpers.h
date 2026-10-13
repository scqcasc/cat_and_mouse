#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace Helpers {
    void RenderCenteredText(SDL_Renderer* renderer, TTF_Font* font, const char* text, float centerX, float centerY, SDL_Color color);
}
