#ifndef H_UI_INIT_FONT
#define H_UI_INIT_FONT

#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct {
    TTF_TextEngine* font_engine;
    TTF_Font** fonts;
} FontSystem;

FontSystem* init_SDLFont(SDL_Renderer* renderer);

#endif