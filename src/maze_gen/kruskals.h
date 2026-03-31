#ifndef H_KRUSKALS
#define H_KRUSKALS

#include "rooms.h"
#include "cell.h"
#include "maze_stats.h"
#include "maze_state.h"


void kruskals_region(Cell* cells, int rows, int cols, MazeState* maze_state);

#endif