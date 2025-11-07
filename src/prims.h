#ifndef H_PRIMS
#define H_PRIMS

#include "cell.h"
#include "maze_stats.h"
#include "rooms.h"

Cell* prims_create_maze(MazeStats* mazeStats, Rooms* rooms);

#endif