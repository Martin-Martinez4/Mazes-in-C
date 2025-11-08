#ifndef H_KRUSKALS
#define H_KRUSKALS

#include "rooms.h"
#include "cell.h"
#include "maze_stats.h"

Cell* kruskalsCreateMaze(MazeStats* mazeStates, Rooms* rooms);

#endif