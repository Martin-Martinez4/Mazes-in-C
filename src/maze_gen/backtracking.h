#ifndef H_BACKTRACKING
#define H_BACKTRACKING

#include "cell.h"
#include "maze_stats.h"
#include "rooms.h"
#include "maze_state.h"

void backtrack_region(Cell* cells, int rows, int cols, MazeState* maze_state);

#endif