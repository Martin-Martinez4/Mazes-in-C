#ifndef H_MAZE_GEN_MAZE_STATE
#define H_MAZE_GEN_MAZE_STATE

#include <stdint.h>

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

#endif