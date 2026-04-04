#include "dead_end.h"
#include "grid_utils.h"
#include <stdio.h>

bool is_dead_end(Cell* cell) {
  uint8_t count = 0;
  uint8_t walls = cell->walls;

  for (int i = 0; i < cell->num_neighbors; i++) {
    if ((walls & cell->dirs[i]) == 0) {
      count++;
    }
  }
  return count == 1;
}

int get_open_end(Cell* cell) {
  uint8_t walls = cell->walls;

  for (int i = 0; i < cell->num_neighbors; i++) {
    if ((walls & cell->dirs[i]) == 0) {
      return i;
    }
  }

  return -1;
}

int prune_dead_ends(Cell* cells, int rows, int cols, int max_depth ) {
  int count = 0;
  // TOP, BOTTOM, LEFT, RIGHT
  const int neigh_coords[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  int neigh_coord;

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      int coords = matrix_coords_to_array_coords(row, col, cols);

      Cell* current_cell = &cells[coords];

      int current_row = row;
      int current_col = col;

      int depth = 0;
      while (is_dead_end(current_cell) && depth < max_depth) {

        
        int open_end = get_open_end(current_cell);
        current_cell->walls = ALL_WALLS;
        count++;
        cells[current_cell->neighbors[open_end]].walls |= current_cell->opposite_dirs[open_end];
        if (open_end == -1) {
          break;
        }

        int index = current_cell->neighbors[open_end];
        current_cell = &cells[index];
        depth++;
      }
    }
  }

  return count;
}