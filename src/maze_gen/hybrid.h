#ifndef H_MAZE_GEN_HYBRID
#define H_MAZE_GEN_HYBRID

#include "cell.h"
#include "maze_stats.h"
#include "rooms.h"

typedef void (*AlgoStepFunc)(Cell* cells);

int prim_step();

Cell* create_maze_hybrid(MazeStats* mazeStats, Rooms* rooms, AlgoStepFunc** algoStepFuncs,
                         int size);

typedef union {
  Edge* edges;
  int* frontier;

} ScratchBuffer;

#endif