#include "init_font.h"
#include <SDL3_ttf/SDL_ttf.h>

int init_SDLFont(SDL_Renderer* renderer) {
  if (!TTF_Init()) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_Init failed: %s", SDL_GetError());
    return -1;
  }

  TTF_TextEngine* textEngine = TTF_CreateRendererTextEngine(renderer);
  if (!textEngine) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create text engine: %s", SDL_GetError());
    return -1;
  }

  // load font
  TTF_Font* font = TTF_OpenFont("resources/Roboto-Regular.ttf", 24);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font: %s", SDL_GetError());
    return -1;
  }

  TTF_Font** fonts = SDL_calloc(1, sizeof(TTF_Font*));
  if (!fonts) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Out of memory!");
    return -1;
  }

  // Load a font and assign it to the first slot
  fonts[0] = TTF_OpenFont("resources/Roboto-Regular.ttf", 24);
  if (!fonts[0]) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font: %s", SDL_GetError());
    return -1;
  }
}
