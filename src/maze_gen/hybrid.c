#include "hybrid.h"
#include "noise.h"
#include "grid_utils.h"
#include "rand_utils.h"
#include "dead_end.h"
#include "sets.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static void setUpCells(Cell* cells, Rooms* rooms, bool* visited, int rows, int columns) {
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      cells[matrix_coords_to_array_coords(row, col, columns)] = create_walled_cell(row, col);
    }
  }

  if (rooms == NULL)
    return;
  for (int i = 0; i < rooms->length; i++) {
    Room room = rooms->data[i];
    int set   = matrix_coords_to_array_coords(room.aabb.y, room.aabb.x, columns);

    int x      = room.aabb.x;
    int y      = room.aabb.y;
    int width  = room.aabb.width;
    int height = room.aabb.height;

    for (int row = y; row < (y + height); row++) {
      for (int col = x; col < (x + width); col++) {
        int walls = 0;
        if (row == y)
          walls |= TOP;
        if (row == y + height - 1)
          walls |= BOTTOM;
        if (col == x)
          walls |= LEFT;
        if (col == x + width - 1)
          walls |= RIGHT;
        cells[matrix_coords_to_array_coords(row, col, columns)].walls = walls;

        visited[matrix_coords_to_array_coords(row, col, columns)] = true;
      }
    }
  }
}



static void setUpEdges(Edge* edges, Cell* cells, int rows, int columns) {
  int indx = 0;
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      Cell* c = &cells[matrix_coords_to_array_coords(row, col, columns)];

      // Right neighbor
      if (col + 1 < columns)
        edges[indx++] = create_edge(c, RIGHT);
      // Bottom neighbor
      if (row + 1 < rows)
        edges[indx++] = create_edge(c, BOTTOM);
    }
  }

  shuffleArray(edges, (rows * (columns - 1)) + ((rows - 1) * columns), sizeof(Edge));
}



Cell* create_maze_hybrid(MazeStats* mazeStats, float* noise_grid, float room_saturation,
                         AlgoStepFunc* algoStepFuncs, int num_algos) {
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
  setUpCells(cells, rooms, visited, rows, columns);

  // Generate noise for hybrid selection
  float scale = 0.06f;
  //   float* noiseGrid = applyNoise(mazeStats->rows, mazeStats->columns, &scale, simplexBilerp,
  //   NULL);

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
  //   double free happens here
  free(maze_state.visited);
  free(maze_state.parent_dirs_stack);
  free(maze_state.frontier);
  free(maze_state.in_frontier);

  // copy paste from kruskals
  int edges_len = (rows * (columns - 1)) + ((rows - 1) * columns);
  Edge* edges   = malloc(sizeof(Edge) * edges_len);
  if (!edges) {
    fprintf(stderr, "Error: malloc failed for edges\n");
    free(cells);
    free(maze_state.sets);
    return NULL;
  }
  setUpEdges(edges, cells, rows, columns);

  int top = edges_len - 1;

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
      abort(); // or exit(EXIT_FAILURE);
    }

    if (neighbor_row >= 0 && neighbor_row < rows && neighbor_column >= 0 &&
        neighbor_column < columns) {
      int current_coords  = matrix_coords_to_array_coords(current_row, current_column, columns);
      int neighbor_coords = matrix_coords_to_array_coords(neighbor_row, neighbor_column, columns);

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
  if (rooms != NULL) {
    free(rooms);
  }
  free(edges);
  free(maze_state.sets);

  while (prune_dead_ends(cells, rows, columns) > 2)
    ;

  return cells;
}