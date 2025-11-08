#ifndef H_BACKTRACKING
#define H_BACKTRACKING

#include "cell.h"
#include "maze_stats.h"
#include "rooms.h"

Cell* backtrackingCreateMaze(MazeStats* mazeStates, Rooms* rooms);
void backtrack(Cell* cells, bool* visited, int row, int column, int rows, int columns);

#endif