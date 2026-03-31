#ifndef H_PRIMS
#define H_PRIMS

#include <stdbool.h>

#include "cell.h"
#include "maze_stats.h"
#include "rooms.h"
#include "maze_state.h"


void prim_region(Cell* cells, int rows, int cols, MazeState* maze_state);

#endif