#include "cell.h"
#include "grid_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// return by copy for now
Cell create_walled_cell(int row, int column) {
  Cell c;
  c.row    = row;
  c.column = column;
  c.walls  = ALL_WALLS;

  return c;
}

Cell create_square_cell(int row, int column, int rows, int columns) {

  static uint8_t square_directions[4] = {TOP, RIGHT, BOTTOM, LEFT};
  static uint8_t opposite_lookup[4]   = {BOTTOM, LEFT, TOP, RIGHT};

  int num_neighbors = 0;
  int dirs[4][2]    = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

  Cell c;
  c.max_edges = 4;
  c.row       = row;
  c.column    = column;
  c.walls     = ALL_WALLS;

  int n_row;
  int n_column;

  for (int i = 0; i < 4; i++) {
    n_row    = row + dirs[i][0];
    n_column = column + dirs[i][1];

    if (n_row >= 0 && n_column >= 0 && n_row < rows && n_column < columns) {

      c.neighbors[num_neighbors]     = matrix_coords_to_array_coords(n_row, n_column, columns);
      c.opposite_dirs[num_neighbors] = opposite_lookup[i];
      c.dirs[num_neighbors]          = square_directions[i];
      num_neighbors++;
    }
  }

  c.num_neighbors = num_neighbors;

  return c;
}



Cell create_tri_cell(int row, int col, int rows, int cols) {
    Cell c = {0};
    c.row = row;
    c.column = col;
    c.max_edges = 3;
    c.walls = TRI_ALL_WALL;
    c.num_neighbors = 0;

    bool is_down = ((row + col) % 2 == 0); // down triangle

    int neighbor_offsets[3][2];

    if (is_down) {
        // left, right, top
        neighbor_offsets[0][0] = 0; neighbor_offsets[0][1] = -1; // left
        neighbor_offsets[1][0] = 0; neighbor_offsets[1][1] = 1;  // right
        neighbor_offsets[2][0] = -1; neighbor_offsets[2][1] = 0; // top
    } else {
        // left, right, bottom
        neighbor_offsets[0][0] = 0; neighbor_offsets[0][1] = -1; // left
        neighbor_offsets[1][0] = 0; neighbor_offsets[1][1] = 1;  // right
        neighbor_offsets[2][0] = 1; neighbor_offsets[2][1] = 0;  // bottom
    }

    uint8_t dirs[3] = {TRI_LEFT, TRI_RIGHT, TRI_BASE};
    uint8_t opposite[3] = {TRI_RIGHT, TRI_LEFT, TRI_BASE};

    for (int i = 0; i < 3; i++) {
        int n_row = row + neighbor_offsets[i][0];
        int n_col = col + neighbor_offsets[i][1];

        // skip out-of-bounds
        if (n_row < 0 || n_row >= rows || n_col < 0 || n_col >= cols) continue;

        int n_idx = matrix_coords_to_array_coords(n_row, n_col, cols);
        c.neighbors[c.num_neighbors] = n_idx;
        c.dirs[c.num_neighbors] = dirs[i];
        c.opposite_dirs[c.num_neighbors] = opposite[i];
        c.num_neighbors++;
    }

    return c;
}

Edge create_edge(Cell* cell, int neighbor_array_idx) {
    Edge e;
    e.cell_ptr = cell;
    e.neighbor_idx = cell->neighbors[neighbor_array_idx]; // actual array index
    e.direction = cell->dirs[neighbor_array_idx];
    e.opposite_direction = cell->opposite_dirs[neighbor_array_idx];
    return e;
}

int BFS_count(Cell* cells, int rows, int columns) {
  bool* visited  = calloc(rows * columns, sizeof(bool));
  int* stack     = malloc(sizeof(int) * rows * columns);
  int stack_head = 0;

  int current;
  int row;
  int column;
  uint8_t walls;

  int next;

  int count         = 0;
  stack[stack_head] = 0;
  visited[0]        = true;

  while (stack_head >= 0) {
    current = stack[stack_head];
    count++;
    --stack_head;

    Cell* c = &cells[current];

    for (int i = 0; i < c->num_neighbors; i++) {
      int next = c->neighbors[i];

      // If wall in this direction is REMOVED, we can traverse
      if ((c->walls & c->dirs[i]) == 0) {
        if (!visited[next]) {
          stack[++stack_head] = next;
          visited[next]       = true;
        }
      }
    }
  }

  free(stack);
  free(visited);
  return count;
}