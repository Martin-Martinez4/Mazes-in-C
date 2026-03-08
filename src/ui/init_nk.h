#ifndef INIT_NK_H
#define INIT_NK_H

#include <SDL3/SDL.h>



struct nk_context;

struct nk_context* initNk(SDL_Window* window, SDL_Renderer* renderer);

#endif