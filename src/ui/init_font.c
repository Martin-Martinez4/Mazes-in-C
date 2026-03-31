#include "init_font.h"
#include <stdlib.h>

FontSystem* init_SDLFont(SDL_Renderer* renderer) {
    if (TTF_Init() < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_Init failed: %s", SDL_GetError());
        return NULL;
    }

    FontSystem* fs = SDL_malloc(sizeof(FontSystem));
    if (!fs) return NULL;

    fs->font_engine = TTF_CreateRendererTextEngine(renderer);
    if (!fs->font_engine) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create text engine: %s", SDL_GetError());
        free(fs);
        return NULL;
    }

    fs->fonts = SDL_calloc(1, sizeof(TTF_Font*));
    if (!fs->fonts) {
        TTF_DestroyRendererTextEngine(fs->font_engine);
        free(fs);
        return NULL;
    }

    fs->fonts[0] = TTF_OpenFont("resources/Roboto-Regular.ttf", 24);
    if (!fs->fonts[0]) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font: %s", SDL_GetError());
        SDL_free(fs->fonts);
        TTF_DestroyRendererTextEngine(fs->font_engine);
        free(fs);
        return NULL;
    }

    return fs;
}
