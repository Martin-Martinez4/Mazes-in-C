#ifndef H_PRIMS
#define H_PRIMS

#include <stdbool.h>

#include "cell.h"
#include "maze_stats.h"
#include "rooms.h"

typedef struct PrimCell {
  Cell* cell;
  bool visited;
  bool in_frontier;
} PrimCell;

Cell* prims_create_maze(MazeStats* mazeStats, Rooms* rooms);

#endif