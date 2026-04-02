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

  Cell* current_cell = &cells[current_index];

  int frontier_index = maze_state->frontier_index;

  for (int i = 0; i < current_cell->num_neighbors; i++) {

    int neigh_index = current_cell->neighbors[i];

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

  Cell* current_cell = &cells[current_index];

  int eligibles[4];
  int eligibles_index = -1;

  for (int i = 0; i < current_cell->num_neighbors; i++) {

    int neigh_index = current_cell->neighbors[i];

    if (maze_state->current_algo_index != maze_state->noise[neigh_index]) {
      continue;
    }

    if (maze_state->visited[neigh_index]) {

      eligibles[++eligibles_index] = i;
    }
  }

  if (eligibles_index == -1) {
    return;
  }

  int i          = eligibles[rand() % (eligibles_index + 1)];
  int next_index = current_cell->neighbors[i];
  int coords;

  current_cell->walls &= ~current_cell->dirs[i];
  cells[next_index].walls &= ~current_cell->opposite_dirs[i];
  // mergeSets(maze_state->sets, coords, current_index);
}

void prim_region(Cell* cells, int rows, int cols, MazeState* maze_state) {

  int current_coord = maze_state->current_index;

  maze_state->number_visited++;

  add_neighbors_to_frontier(current_coord, cols, rows, maze_state, cells);

  while (maze_state->frontier_index >= 0) {

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