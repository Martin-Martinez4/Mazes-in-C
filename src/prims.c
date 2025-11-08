#include "prims.h"
#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "grid_utils.h"
#include "rand_utils.h"
#include "rooms.h"

void setUpCells_prims(Cell* cells, Rooms* rooms, int rows, int columns) {
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      cells[matrix_coords_to_array_coords(row, col, columns)] = create_walled_cell(row, col);
    }
  }
}

void setup_prims_cells(PrimCell* p_cells, Cell* cells, int rows, int columns) {
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      PrimCell p_c;
      p_c.cell        = &cells[matrix_coords_to_array_coords(row, col, columns)];
      p_c.visited     = false;
      p_c.in_frontier = false;
      p_cells[matrix_coords_to_array_coords(row, col, columns)] = p_c;
    }
  }
}

int add_neighbors_to_frontier(PrimCell* p_cells, int* frontier, int top_index, int row, int col,
                              int rows, int columns) {
  // TOP BOTTOM LEFT RIGHT
  const int d_coords[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

  for (int i = 0; i < 4; i++) {
    int neigh_row = row + d_coords[i][0];
    int neigh_col = col + d_coords[i][1];

    if (neigh_row < 0 || neigh_row >= rows || neigh_col < 0 || neigh_col >= columns) {
      continue;
    }
    int index = matrix_coords_to_array_coords(neigh_row, neigh_col, columns);
    if (!p_cells[index].visited && !p_cells[index].in_frontier) {
      frontier[++top_index]      = index;
      p_cells[index].in_frontier = true; // ensure not double-added
    }
  }
  return top_index;
}

void carve_to_visited_neighbor(Cell* cells, PrimCell* p_cells, int current_row, int current_col,
                               int rows, int columns) {
  // [row][column] - row-major order
  // TOP BOTTOM LEFT RIGHT
  const int d_coords[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

  int eligibles[4];
  int eligibles_index = -1;

  for (int i = 0; i < 4; i++) {
    int neigh_row = current_row + d_coords[i][0];
    int neigh_col = current_col + d_coords[i][1];

    if (neigh_row < 0 || neigh_row >= rows || neigh_col < 0 || neigh_col >= columns) {
      continue;
    }

    int neigh_index = matrix_coords_to_array_coords(neigh_row, neigh_col, columns);
    if (p_cells[neigh_index].visited && !p_cells[neigh_index].in_frontier) {
      switch (i) {
      case 0:
        eligibles[++eligibles_index] = TOP;
        break;

      case 1:
        eligibles[++eligibles_index] = BOTTOM;
        break;

      case 2:
        eligibles[++eligibles_index] = LEFT;
        break;

      case 3:
        eligibles[++eligibles_index] = RIGHT;
        break;

      default:
        break;
      }
    }
  }

  if (eligibles_index > -1) {
    Cell* current_cell = &cells[matrix_coords_to_array_coords(current_row, current_col, columns)];
    Cell* n_cell;
    int dir = eligibles[rand() % (eligibles_index + 1)];

    switch (dir) {
    case LEFT:
      current_cell->walls &= ~LEFT;
      n_cell = &cells[matrix_coords_to_array_coords(current_row, current_col - 1, columns)];
      n_cell->walls &= ~RIGHT;
      break;

    case RIGHT:
      current_cell->walls &= ~RIGHT;
      n_cell = &cells[matrix_coords_to_array_coords(current_row, current_col + 1, columns)];
      n_cell->walls &= ~LEFT;
      break;

    case TOP:
      current_cell->walls &= ~TOP;
      n_cell = &cells[matrix_coords_to_array_coords(current_row - 1, current_col, columns)];
      n_cell->walls &= ~BOTTOM;
      break;

    case BOTTOM:
      current_cell->walls &= ~BOTTOM;
      n_cell = &cells[matrix_coords_to_array_coords(current_row + 1, current_col, columns)];
      n_cell->walls &= ~TOP;
      break;

    default:
      break;
    }
  }
}

Cell* prims_create_maze(MazeStats* mazeStats, Rooms* rooms) {
  int rows          = mazeStats->rows;
  int columns       = mazeStats->columns;
  Cell* cells       = malloc(sizeof(Cell) * rows * columns);
  PrimCell* p_cells = malloc(sizeof(PrimCell) * rows * columns);

  if (!cells) {
    fprintf(stderr, "Error: malloc failed for cells in prims\n");
    return NULL;
  }

  setUpCells_prims(cells, rooms, rows, columns);
  setup_prims_cells(p_cells, cells, rows, columns);

  // setup array of neighbors (frontier)
  // for now frontier is going to be half the size of the grid
  int* frontier      = malloc(sizeof(int) * rows * columns);
  int frontier_index = -1;

  int current_col = rand_int(0, columns - 1);
  int current_row = rand_int(0, rows - 1);

  int start_idx              = matrix_coords_to_array_coords(current_row, current_col, columns);
  frontier[++frontier_index] = start_idx;

  do {
    // switch the top with a random frontier node
    // adds random selection of frontier
    int random_index  = rand() % (frontier_index + 1);
    int current_index = frontier[random_index];

    // swap top with rand; random selection from stack
    int temp                 = frontier[frontier_index];
    frontier[frontier_index] = frontier[random_index];
    frontier[random_index]   = temp;
    frontier_index--;

    array_coords_to_matrix_coords(current_index, columns, &current_row, &current_col);
    carve_to_visited_neighbor(cells, p_cells, current_row, current_col, rows, columns);

    p_cells[current_index].visited     = true;
    p_cells[current_index].in_frontier = false;

    frontier_index = add_neighbors_to_frontier(p_cells, frontier, frontier_index, current_row,
                                               current_col, rows, columns);

  } while (frontier_index >= 0);

  return cells;
}