#include "hybrid.h"
#include "noise.h"
#include "grid_utils.h"
#include "rand_utils.h"
#include "dead_end.h"
#include "kruskals.h"
#include "sets.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static void setUpCells(Cell* cells, Rooms* rooms, bool* visited, int rows, int columns, int num_edges) {
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < columns; col++) {
            switch (num_edges)
            {
                case 4:
                    cells[matrix_coords_to_array_coords(row, col, columns)] =
                        create_square_cell(row, col, rows, columns);
                    break;

                case 3:
                    cells[matrix_coords_to_array_coords(row, col, columns)] =
                        create_tri_cell(row, col, rows, columns);
                    break;

                default:
                    break;
            }

            // Mark all as unvisited initially
            int idx = matrix_coords_to_array_coords(row, col, columns);
            visited[idx] = false;
        }
    }

    // If there are no rooms, we're done
    if (rooms == NULL)
        return;

    // Room carving for triangle grids
    for (int r = 0; r < rooms->length; r++) {
        Room room = rooms->data[r];

        int x = room.aabb.x;
        int y = room.aabb.y;
        int width = room.aabb.width;
        int height = room.aabb.height;

        for (int row = y; row < y + height; row++) {
            for (int col = x; col < x + width; col++) {

                int idx = matrix_coords_to_array_coords(row, col, columns);
                Cell* cell = &cells[idx];

                // Remove all walls initially
                cell->walls = 0;

                for (int d = 0; d < cell->num_neighbors; d++) {
                    int n_idx = cell->neighbors[d];
                    Cell* neigh = &cells[n_idx];

                    bool outside = false;

                    // Horizontal walls
                    if (cell->dirs[d] == TRI_LEFT) {
                        if (neigh->column < x)
                            outside = true;
                    } else if (cell->dirs[d] == TRI_RIGHT) {
                        if (neigh->column >= x + width)
                            outside = true;
                    }
                    // Vertical walls
                    else if (cell->dirs[d] == TRI_BASE) {
                        if (IS_DOWN(row, col)) {
                            // Down triangle, top neighbor
                            if (neigh->row < y)
                                outside = true;
                        } else {
                            // Up triangle, bottom neighbor
                            if (neigh->row >= y + height)
                                outside = true;
                        }
                    }

                    if (outside) {
                        cell->walls |= cell->dirs[d];
                    }
                }

                visited[idx] = true;
            }
        }
    }
}

Cell* create_maze_hybrid(MazeStats* mazeStats, float* noise_grid, float room_saturation,
                         AlgoStepFunc* algoStepFuncs, int num_algos, int prune_aggressiveness) {
  int rows    = mazeStats->rows;
  int columns = mazeStats->columns;


  Rooms* rooms = makeRooms(mazeStats, room_saturation);

  Cell* cells = malloc(sizeof(Cell) * rows * columns);
  if (!cells) {
    fprintf(stderr, "Error: malloc failed for cells\n");
    return NULL;
  }
  bool* visited = calloc(rows * columns, sizeof(bool));
  if (!visited) {
    fprintf(stderr, "Error: calloc failed for visited\n");
    return NULL;
  }
  setUpCells(cells, rooms, visited, rows, columns, mazeStats->num_edges);

  float scale = 0.06f;

  for (int i = 0; i < rows * columns; i++) {
    int algo_index = value_map_int(noise_grid[i], 0.0, 1.0, 0, num_algos - 1);
    noise_grid[i]  = algo_index;
  }

  int* sets = malloc(sizeof(int) * rows * columns);
  if (!sets) {
    fprintf(stderr, "Error: malloc failed for sets\n");
    free(cells);
    return NULL;
  }
  setUpSets(sets, rooms, rows, columns);

  // might make into a function later
  // Allocate maze state
  MazeState maze_state;
  maze_state.visited = visited;
  maze_state.sets    = sets;
  maze_state.noise   = noise_grid;

  maze_state.parent_dirs_stack      = malloc(sizeof(uint8_t) * rows * columns);
  maze_state.parent_dirs_stack_size = -1;

  maze_state.frontier       = malloc(sizeof(int) * rows * columns);
  maze_state.in_frontier    = calloc(rows * columns, sizeof(bool));
  maze_state.frontier_index = -1;

  maze_state.number_visited = 0;
  maze_state.current_index  = -1;

  for (int i = 0; i < rows * columns; i++) {
    if (maze_state.visited[i])
      continue;

    int region_algo = noise_grid[i];

    maze_state.current_index      = i;
    maze_state.current_algo_index = region_algo;

    AlgoStepFunc algo = algoStepFuncs[region_algo];

    // Keep running until region fully processed
    if (!maze_state.visited[i]) {
      algo(cells, rows, columns, &maze_state);
    }
  }

  //   Free temporary resources
  free(maze_state.visited);
  free(maze_state.parent_dirs_stack);
  free(maze_state.frontier);
  free(maze_state.in_frontier);

  kruskals_unite(cells, rows, columns, &maze_state);
  if (rooms != NULL) {
    free(rooms);
  }
  free(maze_state.sets);

  if (prune_aggressiveness > 0) {

    int removed;
    int count            = 0;
    int i                = 0;
    int max_pruned_cells = (int) (rows * columns * (prune_aggressiveness / 100.f));

    while (count < max_pruned_cells && i < prune_aggressiveness) {
      removed = prune_dead_ends(cells, rows, columns, prune_aggressiveness);

      // no dead ends left avoid empty loops
      if (removed == 0) {
        break;
      }

      count += removed;
      i++;
    };
  }
  return cells;
}