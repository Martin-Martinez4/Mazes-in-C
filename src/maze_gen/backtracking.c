#include <string.h>

#include "backtracking.h"
#include "grid_utils.h"
#include "rand_utils.h"
// #include "sets.h"

int opposite_direction(int direction) {
  switch (direction) {
  case TOP:
    return BOTTOM;
  case BOTTOM:
    return TOP;
  case LEFT:
    return RIGHT;
  case RIGHT:
    return LEFT;

  default:
    return -1;
  }
}

int neighbor_index(int current_index, int direction, int rows, int cols) {
  int row, col;
  array_coords_to_matrix_coords(current_index, cols, &row, &col);
  int n_row = row, n_col = col;

  switch (direction) {
  case TOP:
    n_row = row - 1;
    break;
  case RIGHT:
    n_col = col + 1;
    break;
  case BOTTOM:
    n_row = row + 1;
    break;
  case LEFT:
    n_col = col - 1;
    break;
  default:
    return -1;
  }

  if (n_row < 0 || n_row >= rows || n_col < 0 || n_col >= cols)
    return -1;
  return matrix_coords_to_array_coords(n_row, n_col, cols);
}


void backtrack_region(Cell* cells, int rows, int cols, MazeState* maze_state) {
  while (maze_state->current_index != -1) {
    int current_index = maze_state->current_index;

    int unvisited[4];
    int count = 0;

    uint8_t directions[] = {TOP, RIGHT, BOTTOM, LEFT};

    for (int i = 0; i < 4; i++) {
      uint8_t dir = directions[i];
      int n_index = neighbor_index(current_index, dir, rows, cols);

      if (n_index >= 0 && !maze_state->visited[n_index] &&
          maze_state->current_algo_index == (int) maze_state->noise[n_index]) {
        unvisited[count++] = dir;
      }
    }

    if (count > 0) {
      uint8_t chosen_dir = unvisited[rand() % count];
      int next_index     = neighbor_index(current_index, chosen_dir, rows, cols);

      // Carve walls
      cells[current_index].walls &= ~chosen_dir;
      cells[next_index].walls &= ~opposite_direction(chosen_dir);

      // mergeSets(maze_state->sets, next_index, current_index);

      maze_state->visited[next_index] = true;
      maze_state->number_visited++;

      // Push current onto stack
      maze_state->parent_dirs_stack[++maze_state->parent_dirs_stack_size] = current_index;

      // Move to neighbor
      maze_state->current_index = next_index;
    } else {
      // Backtrack
      if (maze_state->parent_dirs_stack_size >= 0) {
        maze_state->current_index =
            maze_state->parent_dirs_stack[maze_state->parent_dirs_stack_size--];
      } else {
        maze_state->current_index = -1;
      }
    }
  }
}