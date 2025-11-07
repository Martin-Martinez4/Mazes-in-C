#include "prims.h"
#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "grid_utils.h"
#include "rooms.h"

void setUpCells_prims(Cell* cells, Rooms* rooms, int rows, int columns) {
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      cells[matrix_coords_to_array_coords(row, col, columns)] = create_walled_cell(row, col);
    }
  }
}

// array of the size of the matrix of bools to keep track of visited status

Cell* prims_create_maze(MazeStats* mazeStats, Rooms* rooms) {
  int rows    = mazeStats->rows;
  int columns = mazeStats->columns;
  Cell* cells = malloc(sizeof(Cell) * rows * columns);

  if (!cells) {
    fprintf(stderr, "Error: malloc failed for cells in prims\n");
    return NULL;
  }

  setUpCells_prims(cells, rooms, rows, columns);

  int start_col = rand_int(0, columns - 1);
  int start_row = rand_int(0, rows - 1);
  // setup array of neighbors (frontier)
}