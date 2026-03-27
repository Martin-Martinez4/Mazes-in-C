#ifndef H_MAZE_GEN_GAME_MAP
#define H_MAZE_GEN_GAME_MAP

#include "cell.h"
#include "maze_stats.h"
#include "hybrid.h"

Cell* create_game_map(MazeStats* mazeStats, Rooms* rooms, float* noise_grid, AlgoStepFunc* algoStepFuncs);

#endif