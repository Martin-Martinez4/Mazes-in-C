#ifndef H_MAZE_GEN_DEAD_END
#define H_MAZE_GEN_DEAD_END

#include "cell.h"
#include <stdbool.h>

bool is_dead_end(Cell* cell);
int get_open_end(Cell* cell);
int prune_dead_ends(Cell* cells, int rows, int cols, int max_depth);

#endif