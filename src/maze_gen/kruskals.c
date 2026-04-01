#include "kruskals.h"

#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "grid_utils.h"
#include "rooms.h"
#include "sets.h"
#include "rand_utils.h"

static void setUpRegions(Cell* cells, int* sets, Edge* edges, int rows, int cols) {

  int dirs[2]      = {RIGHT, BOTTOM};
  int neighbors[2] = {-1, -1};

  int current_coord = -1;
  Cell current_cell;

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {

      current_coord = matrix_coords_to_array_coords(row, col, cols);
      current_cell  = cells[current_coord];

      neighbors[0] = col + 1 < cols ? matrix_coords_to_array_coords(row, col + 1, cols) : -1;

      neighbors[1] = row + 1 < rows ? matrix_coords_to_array_coords(row + 1, col, cols) : -1;

      for (int i = 0; i < 2; i++) {
        if (neighbors[i] > -1) {
          if ((dirs[i] & current_cell.walls) == 0) {
            mergeSets(sets, current_coord, neighbors[i]);
          }
        }
      }
    }
  }
}

static int setUpEdges(Cell* cells, int* sets, Edge* edges, int rows, int cols) {

  int dirs[2]      = {RIGHT, BOTTOM};
  int neighbors[2] = {-1, -1};

  int indx          = 0;
  int current_coord = -1;
  Cell* current_cell;

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {

      current_coord = matrix_coords_to_array_coords(row, col, cols);
      current_cell  = &cells[current_coord];

      neighbors[0] = col + 1 < cols ? matrix_coords_to_array_coords(row, col + 1, cols) : -1;

      neighbors[1] = row + 1 < rows ? matrix_coords_to_array_coords(row + 1, col, cols) : -1;

      for (int i = 0; i < 2; i++) {

        if (neighbors[i] > -1) {

          if ((current_cell->walls & dirs[i]) != 0 &&
              find(sets, current_coord) != find(sets, neighbors[i])) {

            edges[indx++] = create_edge(current_cell, dirs[i]);
          }
        }
      }
    }
  }

  shuffleArray(edges, indx, sizeof(Edge));
  return indx;
}

// Kruskals is applied at the end of the hybrid maze so this does nothing
void kruskals_region(Cell* cells, int rows, int cols, MazeState* maze_state){}

// Applied at end of hynrid maze
void kruskals_unite(Cell* cells, int rows, int cols, MazeState* maze_state) {

  int edges_len = (rows * (cols - 1)) + ((rows - 1) * cols);
  Edge* edges   = malloc(sizeof(Edge) * edges_len);
  if (!edges) {
    fprintf(stderr, "Error: malloc failed for edges\n");
    free(cells);
    return;
  }

  int* sets = malloc(sizeof(int) * rows * cols);

  setUpSets(sets, NULL, rows, cols);

  setUpRegions(cells, sets, edges, rows, cols);

  int top = setUpEdges(cells, sets, edges, rows, cols) - 1;

  while (top >= 0) {
    Edge e        = edges[top];
    Cell* current = e.cell_ptr;

    int neighbor_row;
    int neighbor_column;

    int current_row    = current->row;
    int current_column = current->column;

    switch (e.direction) {
    case RIGHT:
      neighbor_row    = current_row;
      neighbor_column = current_column + 1;
      break;

    case BOTTOM:
      neighbor_row    = current_row + 1;
      neighbor_column = current_column;
      break;

    default:
      fprintf(stderr, "Error: invalid direction %u in kruskalsCreateMaze()\n",
              e.opposite_direction);
      abort();
      ;
    }

    if (neighbor_row >= 0 && neighbor_row < rows && neighbor_column >= 0 &&
        neighbor_column < cols) {
      int current_coords  = matrix_coords_to_array_coords(current_row, current_column, cols);
      int neighbor_coords = matrix_coords_to_array_coords(neighbor_row, neighbor_column, cols);

      // get sets
      int neighbor_set = find(sets, neighbor_coords);
      int current_set  = find(sets, current_coords);

      if (neighbor_set != current_set) {
        // remove walls
        cells[current_coords].walls &= ~e.direction;
        cells[neighbor_coords].walls &= ~e.opposite_direction;

        // merge sets
        mergeSets(sets, current_coords, neighbor_set);
      }
    }

    top--;
  }

  free(sets);
  free(edges);
}