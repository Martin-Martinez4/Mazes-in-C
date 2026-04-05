#include "kruskals.h"

#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "grid_utils.h"
#include "rooms.h"
#include "sets.h"
#include "rand_utils.h"

static void setUpRegions(Cell* cells, int* sets, Edge* edges, int rows, int cols) {

  int current_coord = -1;
  Cell current_cell;

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {

      current_coord = matrix_coords_to_array_coords(row, col, cols);
      current_cell  = cells[current_coord];

      for (int i = 0; i < current_cell.num_neighbors; i++) {

        if ((current_cell.dirs[i] & current_cell.walls) == 0) {
          if ((cells[current_cell.neighbors[i]].walls & current_cell.opposite_dirs[i]) == 0) {
            mergeSets(sets, current_coord, current_cell.neighbors[i]);
          }
        }
      }
    }
  }
}

static int setUpEdges(Cell* cells, int* sets, Edge* edges, int rows, int cols) {
  int indx = 0;

  for (int i = 0; i < rows * cols; i++) {
    Cell* c = &cells[i];

    for (int n = 0; n < c->num_neighbors; n++) {
      int neighbor_idx = c->neighbors[n];

      // Only add one edge per pair
      if (i < neighbor_idx) {
        edges[indx++] = create_edge(c, n);
      }
    }
  }

  shuffleArray(edges, indx, sizeof(Edge));
  return indx;
}

// Kruskals is applied at the end of the hybrid maze so this does nothing
void kruskals_region(Cell* cells, int rows, int cols, MazeState* maze_state) {}

// Applied at end of hynrid maze
void kruskals_unite(Cell* cells, int rows, int cols, MazeState* maze_state) {

  int edges_len = (rows * cols * 4);
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

  while (--top >= 0) {
    Edge e           = edges[top];
    int current_idx  = e.cell_ptr - cells;
    int neighbor_idx = e.neighbor_idx; // ✅ actual neighbor array index

    int set_a = find(sets, current_idx);
    int set_b = find(sets, neighbor_idx);

    if (set_a != set_b) {
      cells[current_idx].walls &= ~e.direction;
      cells[neighbor_idx].walls &= ~e.opposite_direction;
      mergeSets(sets, current_idx, neighbor_idx);
    }
  }
  free(sets);
  free(edges);
}