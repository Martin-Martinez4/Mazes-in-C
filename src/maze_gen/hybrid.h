#ifndef H_MAZE_GEN_HYBRID
#define H_MAZE_GEN_HYBRID

#include "cell.h"
#include "maze_stats.h"
#include "rooms.h"

typedef union {
  Edge* edges;
  int* frontier;

} ScratchBuffer;
typedef void (*AlgoStepFunc)(ScratchBuffer* scratch, Cell* cells, float* noise, int* sets, int row,
                             int column, int columns, int rows);

int prim_step(ScratchBuffer* scratch, Cell* cells, float* noise, int* sets, int row, int column,
              int columns, int rows);

Cell* create_maze_hybrid(MazeStats* mazeStats, float roomSaturation, AlgoStepFunc** algoStepFuncs,
                         int size);


#endif