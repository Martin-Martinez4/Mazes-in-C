#ifndef H_CREATE_MAZE
#define H_CREATE_MAZE

#include "cell.h"
#include "maze_stats.h"
#include <SDL3/SDL_rect.h>

typedef enum { BACKTRACKING ,PRIMS, KRUSKALS } MazeGenAlgorithm;

Cell* createCells(MazeStats* mazeStats, MazeGenAlgorithm algo, double roomSaturation);

SDL_FRect* createSDLRects(MazeStats* mazeStats, Cell* cells, int* cellsCreated);

#endif