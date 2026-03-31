#ifndef H_MAZE_GEN_HYBRID
#define H_MAZE_GEN_HYBRID

#include "cell.h"
#include "maze_stats.h"
#include "rooms.h"
#include "maze_state.h"

typedef union {
  Edge* edges;
  int* frontier;

} ScratchBuffer;



typedef void (*AlgoStepFunc)(Cell* cells, int rows, int cols, MazeState* maze_state);

void prim_region(Cell* cells, int rows, int cols, MazeState* maze_state);

Cell* create_maze_hybrid(MazeStats* mazeStats, float* noise_grid, float room_saturation,
                         AlgoStepFunc* algoStepFuncs, int num_algos, int prune_aggressiveness);

#endif