#include "dead_end.h"
#include "grid_utils.h"


bool is_dead_end(Cell* cell) {
  uint8_t count = 0;
  uint8_t walls = cell->walls;

  uint8_t wall_dirs[4] = {TOP, RIGHT, LEFT, BOTTOM};

  for (int i = 0; i < 4; i++) {
    if ((walls & wall_dirs[i]) == 0) {
      count++;
    }
  }
  return count == 1;
}

uint8_t get_open_end(Cell* cell) {
  uint8_t count = 0;
  uint8_t walls = cell->walls;

  uint8_t wall_dirs[4] = {TOP, RIGHT, LEFT, BOTTOM};

  for (int i = 0; i < 4; i++) {
    if ((walls & wall_dirs[i]) == 0) {
      return wall_dirs[i];
    }
  }

  return 0;
}

int prune_dead_ends(Cell* cells, int rows, int cols) {
  int count = 0;
  // TOP, BOTTOM, LEFT, RIGHT
  const int neigh_coords[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  const int neigh_dirs[4]      = {BOTTOM, TOP, RIGHT, LEFT};
  int neigh_coord;

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      int coords = matrix_coords_to_array_coords(row, col, cols);

      Cell* current_cell = &cells[coords];

      int current_row = row;
      int current_col = col;

      while (is_dead_end(current_cell)) {

        cells[coords].walls = ALL_WALLS;
        count++;

        for (int i = 0; i < 4; i++) {
          int new_row = row + neigh_coords[i][0];
          int new_col = col + neigh_coords[i][1];

          // Bounds check
          if (new_row < 0 || new_row >= rows || new_col < 0 || new_col >= cols) {
            continue;
          }

          int neigh_coord = matrix_coords_to_array_coords(new_row, new_col, cols);
          cells[neigh_coord].walls |= neigh_dirs[i];
        }

        int8_t open_end = get_open_end(current_cell);
        if (open_end == -1) {
          break;
        }

        switch (open_end) {
        case TOP:
          current_row -= 1;
          break;
        case RIGHT:
          current_col += 1;
          break;
        case BOTTOM:
          current_row += 1;
          break;
        case LEFT:
          current_col -= 1;
          break;
        default:
          break;
        }
        coords       = matrix_coords_to_array_coords(current_row, current_col, cols);
        current_cell = &cells[coords];
      }
    }
  }

  return count;
}