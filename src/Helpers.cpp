#include "Helpers.h"

namespace Helpers {
    void RenderCenteredText(SDL_Renderer* renderer, TTF_Font* font, const char* text, float centerX, float centerY, SDL_Color color) {
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
}
