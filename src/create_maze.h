#ifndef H_CREATE_MAZE
#define H_CREATE_MAZE

#include "cell.h"
#include "maze_stats.h"
#include <SDL3/SDL_rect.h>
#include "SDL3/SDL_render.h"

typedef enum { BACKTRACKING ,PRIMS, KRUSKALS, HYBRID } MazeGenAlgorithm;

Cell* createCells(MazeStats* mazeStats, float* noise_grid, MazeGenAlgorithm algo, double roomSaturation, int prune_aggressiveness);

SDL_FRect* createSDLRects(MazeStats* mazeStats, Cell* cells, int* cellsCreated);
void updateNoiseTexture(SDL_Texture* texture, float* noiseGrid, int columns, int rows);
#endif