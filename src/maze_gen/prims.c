#include "prims.h"
#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "grid_utils.h"
#include "rand_utils.h"

static void add_neighbors_to_frontier(int current_index, int columns, int rows,
                                      MazeState* maze_state, Cell* cells) {
  int row, column;
  array_coords_to_matrix_coords(current_index, columns, &row, &column);

  int frontier_index = maze_state->frontier_index;

  // Direction offsets: DOWN, RIGHT, UP, LEFT
  const int d_coords[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

  for (int i = 0; i < 4; i++) {
    int neigh_row = row + d_coords[i][0];
    int neigh_col = column + d_coords[i][1];

    // Skip out-of-bounds neighbors
    if (neigh_row < 0 || neigh_row >= rows || neigh_col < 0 || neigh_col >= columns) {
      continue;
    }

    int neigh_index = matrix_coords_to_array_coords(neigh_row, neigh_col, columns);

    if (maze_state->current_algo_index != maze_state->noise[neigh_index]) {
      continue;
    }

    // Only add unvisited cells that are not already in the frontier
    if (!maze_state->visited[neigh_index] && !maze_state->in_frontier[neigh_index]) {
      maze_state->frontier[++frontier_index] = neigh_index;
      maze_state->in_frontier[neigh_index]   = true;
    }
  }
  maze_state->frontier_index = frontier_index;
}

static void carve_to_visited_neighbor(Cell* cells, MazeState* maze_state, int current_index,
                                      int columns, int rows) {

  int current_row, current_col;
  array_coords_to_matrix_coords(current_index, columns, &current_row, &current_col);

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

    if (maze_state->current_algo_index != maze_state->noise[neigh_index]) {
      continue;
    }

    if (maze_state->visited[neigh_index]) {
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
      }
    }
  }

  if (eligibles_index == -1) {
    // This should NEVER happen in correct Prim’s
    return;
  }

  int dir            = eligibles[rand() % (eligibles_index + 1)];
  Cell* current_cell = &cells[current_index];
  Cell* n_cell;
  int coords;

  switch (dir) {
  case LEFT:
    coords = matrix_coords_to_array_coords(current_row, current_col - 1, columns);
    current_cell->walls &= ~LEFT;
    n_cell = &cells[coords];
    n_cell->walls &= ~RIGHT;
    break;

  case RIGHT:
    coords = matrix_coords_to_array_coords(current_row, current_col + 1, columns);
    current_cell->walls &= ~RIGHT;
    n_cell = &cells[coords];
    n_cell->walls &= ~LEFT;
    break;

  case TOP:
    coords = matrix_coords_to_array_coords(current_row - 1, current_col, columns);
    current_cell->walls &= ~TOP;
    n_cell = &cells[coords];
    n_cell->walls &= ~BOTTOM;
    break;

  case BOTTOM:
    coords = matrix_coords_to_array_coords(current_row + 1, current_col, columns);
    current_cell->walls &= ~BOTTOM;
    n_cell = &cells[coords];
    n_cell->walls &= ~TOP;
    break;
  }
  // mergeSets(maze_state->sets, coords, current_index);
}

void prim_region(Cell* cells, int rows, int cols, MazeState* maze_state) {

  int current_coord = maze_state->current_index;

  maze_state->visited[current_coord] = true;
  maze_state->number_visited++;

  add_neighbors_to_frontier(current_coord, cols, rows, maze_state, cells);

  while (maze_state->frontier_index >= 0) {
    if (maze_state->frontier_index < 0) {
      break;
    }

    int random_index = rand() % (maze_state->frontier_index + 1);
    current_coord    = maze_state->frontier[random_index];

    // swap top with rand; random selection from stack
    int temp = maze_state->frontier[maze_state->frontier_index];
    maze_state->frontier[maze_state->frontier_index] = maze_state->frontier[random_index];
    maze_state->frontier[random_index]               = temp;
    // printf("frontier_index before: %d\n", (maze_state->frontier_index));
    (maze_state->frontier_index) -= 1;
    // printf("frontier_index after: %d\n", (maze_state->frontier_index));

    carve_to_visited_neighbor(cells, maze_state, current_coord, cols, rows);
    maze_state->in_frontier[current_coord] = false;
    if (!maze_state->visited[current_coord]) {
      maze_state->visited[current_coord] = true;
      maze_state->number_visited++;
    }

    add_neighbors_to_frontier(current_coord, cols, rows, maze_state, cells);
    if (maze_state->frontier_index < 0) {
      return;
    }
  }
}