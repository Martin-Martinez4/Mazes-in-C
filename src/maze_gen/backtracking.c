#include <string.h>

#include "backtracking.h"
#include "grid_utils.h"
#include "rand_utils.h"
// #include "sets.h"

void backtrack_region(Cell* cells, int rows, int cols, MazeState* maze_state) {
    while (maze_state->current_index != -1) {
        int current_index = maze_state->current_index;
        Cell* current_cell = &cells[current_index];

        int candidates[MAX_NEIGHBORS];
        int count = 0;

        // Collect valid unvisited neighbors
        for (int i = 0; i < current_cell->num_neighbors; i++) {
            int n_index = current_cell->neighbors[i];

            if (n_index >= 0 && !maze_state->visited[n_index]) {
              // store index into neighbors[]
                candidates[count++] = i; 
            }
        }

        if (count > 0) {
            // Pick random neighbor
            int choice = rand() % count;
            int i = candidates[choice];

            int next_index = current_cell->neighbors[i];

            // Carve walls using precomputed bitmasks
            current_cell->walls &= ~current_cell->dirs[i];
            cells[next_index].walls &= ~current_cell->opposite_dirs[i];

            // Mark visited
            maze_state->visited[next_index] = true;
            maze_state->number_visited++;

            // Push current to stack
            maze_state->parent_dirs_stack[++maze_state->parent_dirs_stack_size] = current_index;

            // Move forward
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