#ifndef H_MAZE_GEN_HYBRID
#define H_MAZE_GEN_HYBRID

#include "cell.h"
#include "maze_stats.h"
#include "rooms.h"

typedef union {
  Edge* edges;
  int* frontier;

} ScratchBuffer;

typedef struct MazeState {
  int current_index;
  bool* visited;
  int number_visited;
  int* sets;
  float* noise;
  int current_algo_index;

  uint8_t* parent_dirs_stack;
  int parent_dirs_stack_size;
  bool backstep_done;

  int* frontier;
  bool* in_frontier;
  int frontier_index;
} MazeState;

typedef void (*AlgoStepFunc)(Cell* cells, int rows, int cols, MazeState* maze_state);

void prim_step(Cell* cells, int rows, int cols, MazeState* maze_state);
void backtrack_region(Cell* cells, int rows, int cols, MazeState* maze_state);
void kruskals_region(Cell* cells, int rows, int cols, MazeState* maze_state);

Cell* create_maze_hybrid(MazeStats* mazeStats, float roomSaturation, AlgoStepFunc* algoStepFuncs,
                         int num_algos);

#endif