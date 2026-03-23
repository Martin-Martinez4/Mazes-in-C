#include "hybrid.h"
#include "noise.h"
#include "grid_utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rand_utils.h"

// copy from kruskal's implementation
static void shuffleEdgeArray(Edge* array, int length) {
  for (int i = length - 1; i >= 1; i--) {
    int j     = rand() % (i + 1);
    Edge temp = array[i];
    array[i]  = array[j];
    array[j]  = temp;
  }
}

static int value_map_int(float value, float min_val, float max_val, int min, int max) {
  if (value < min_val)
    value = min_val;
  if (value > max_val)
    value = max_val;

  float normalized = (value - min_val) / (max_val - min_val);

  int result = (int) (normalized * (max - min)) + min;

  return result;
}

static void setUpCells(Cell* cells, Rooms* rooms, int rows, int columns) {
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
      }
    }
  }
}

static void setUpSets(int* sets, Rooms* rooms, int rows, int columns) {
  for (int i = 0; i < rows * columns; i++) {
    sets[i] = i;
  }

  if (rooms == NULL) {
    return;
  }
  for (int i = 0; i < rooms->length; i++) {
    Room room = rooms->data[i];
    int set   = matrix_coords_to_array_coords(room.aabb.y, room.aabb.x, columns);

    int x      = room.aabb.x;
    int y      = room.aabb.y;
    int width  = room.aabb.width;
    int height = room.aabb.height;

    for (int row = y; row < (y + height - 1); row++) {
      for (int col = x; col < (x + width - 1); col++) {
        sets[matrix_coords_to_array_coords(row, col, columns)] = set;
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

  shuffleEdgeArray(edges, (rows * (columns - 1)) + ((rows - 1) * columns));
}

static int find(int* sets, int x) {
  int root = x;

  // find true root
  while (sets[root] != root) {
    root = sets[root];
  }

  // compress root
  while (sets[x] != root) {
    int next = sets[x];
    sets[x]  = root;
    x        = next;
  }

  return root;
}

static void mergeSets(int* sets, int current, int change) {
  int current_root = find(sets, current);
  int change_root  = find(sets, change);

  if (current_root != change_root) {
    sets[change_root] = current_root;
  }
}

// so many parameters
static void add_neighbors_to_frontier(int current_index, int columns, int rows,
                                      MazeState* maze_state, Cell* cells) {
  int row, column;
  array_coords_to_matrix_coords(current_index, columns, &row, &column);

  // Direction offsets: DOWN, RIGHT, UP, LEFT
  const int d_coords[4][2] = {
      {1, 0},  // down
      {0, 1},  // right
      {-1, 0}, // up
      {0, -1}  // left
  };

  for (int i = 0; i < 4; i++) {
    int neigh_row = row + d_coords[i][0];
    int neigh_col = column + d_coords[i][1];

    // Skip out-of-bounds neighbors
    if (neigh_row < 0 || neigh_row >= rows || neigh_col < 0 || neigh_col >= columns) {
      continue;
    }

    int neigh_index = matrix_coords_to_array_coords(neigh_row, neigh_col, columns);

    // Only add unvisited cells that are not already in the frontier
    if (!maze_state->visited[neigh_index] && !maze_state->in_frontier[neigh_index]) {
      maze_state->frontier[++(maze_state->frontier_index)] = neigh_index;
      maze_state->in_frontier[neigh_index]                 = true;
    }
  }
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
}

// void prim_step(ScratchBuffer* scratch, Cell* cells, float* noise, int* sets, int row, int column,
//                int columns, int rows, int indx, int size) {
//   bool* in_frontier = calloc(columns * rows, sizeof(bool));

//   int* frontier      = (int*) scratch->edges;
//   int frontier_index = -1;

//   int current_coord = matrix_coords_to_array_coords(row, column, columns);

//   frontier[++frontier_index] = current_coord;
//   in_frontier[current_coord] = true;
//   cells[current_coord].walls = 0;

//   do {
//     // copied from prims.c
//     int random_index = rand() % (frontier_index + 1);
//     current_coord    = frontier[random_index];

//     // swap top with rand; random selection from stack
//     int temp                 = frontier[frontier_index];
//     frontier[frontier_index] = frontier[random_index];
//     frontier[random_index]   = temp;
//     frontier_index--;

//     carve_to_visited_neighbor(cells, in_frontier, current_coord, columns, rows);
//     in_frontier[current_coord] = false;

//     frontier_index = add_neighbors_to_frontier(frontier, in_frontier, frontier_index, cells,
//     noise,
//                                                sets, current_coord, columns, rows, indx, size);
//   } while (frontier_index >= 0);

//   free(in_frontier);
// }

void prim_step(Cell* cells, int rows, int cols, MazeState* maze_state) {
  // Initialize first cell if maze is empty
  if (maze_state->number_visited == 0) {
    int current_row = rand_int(0, rows - 1);
    int current_col = rand_int(0, cols - 1);
    int start_idx   = matrix_coords_to_array_coords(current_row, current_col, cols);

    maze_state->visited[start_idx] = true;
    maze_state->number_visited++;
    maze_state->current_index = start_idx;

    add_neighbors_to_frontier(start_idx, cols, rows, maze_state, cells);
    return;
  }

  // If frontier empty, Prim done
  if (maze_state->frontier_index < 0) {
    maze_state->current_index = -1;
    return;
  }

  // Pick random frontier cell
  int random_index  = rand() % (maze_state->frontier_index + 1);
  int current_index = maze_state->frontier[random_index];

  // Remove from frontier
  int temp = maze_state->frontier[maze_state->frontier_index];
  maze_state->frontier[maze_state->frontier_index] = maze_state->frontier[random_index];
  maze_state->frontier[random_index]               = temp;
  maze_state->frontier_index--;

  // Connect to a visited neighbor
  carve_to_visited_neighbor(cells, maze_state, current_index, cols, rows);

  // Mark visited
  maze_state->visited[current_index]     = true;
  maze_state->in_frontier[current_index] = false;
  maze_state->number_visited++;
  maze_state->current_index = current_index;

  // Add unvisited neighbors
  add_neighbors_to_frontier(current_index, cols, rows, maze_state, cells);
}
static void shuffleArray(void* array, int length, size_t element_size) {
  if (length <= 1 || element_size == 0)
    return;

  unsigned char* arr  = (unsigned char*) array;
  unsigned char* temp = (unsigned char*) malloc(element_size);
  if (!temp)
    return; // allocation failed

  for (int i = length - 1; i >= 1; i--) {
    int j = rand() % (i + 1);

    // swap arr[i] and arr[j]
    memcpy(temp, arr + i * element_size, element_size);
    memcpy(arr + i * element_size, arr + j * element_size, element_size);
    memcpy(arr + j * element_size, temp, element_size);
  }

  free(temp);
}

int kruskal_step(ScratchBuffer* scratch, Cell* cells, float* noise, int* sets, int row, int column,
                 int columns, int rows) {}

int to_direction(int current_index, int next_index, int cols) {
  int current_row;
  int current_col;
  array_coords_to_matrix_coords(current_index, cols, &current_row, &current_col);

  int next_row;
  int next_col;
  array_coords_to_matrix_coords(next_index, cols, &next_row, &next_col);

  if (next_row == current_row - 1 && next_col == current_col)
    return TOP;
  else if (next_row == current_row && next_col == current_col + 1)
    return RIGHT;
  else if (next_row == current_row + 1 && next_col == current_col)
    return BOTTOM;
  else if (next_row == current_row && next_col == current_col - 1)
    return LEFT;
  else
    return -1;
}

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

int move_to_parent(int index, int parent_dir, int rows, int cols) {
  int parent_idx = neighbor_index(index, parent_dir, rows, cols);
  return parent_idx;
}

void backtrack_step(Cell* cells, int rows, int cols, MazeState* maze_state) {
  int current_index = maze_state->current_index;
  printf("current_index: %d\n", current_index);
  if (current_index == -1) {
    return;
  }

  int unvisited[4];
  int count = 0;

  uint8_t directions[] = {TOP, RIGHT, BOTTOM, LEFT};

  for (int i = 0; i < 4; i++) {
    uint8_t dir = directions[i];
    int n_index = neighbor_index(current_index, dir, rows, cols);

    if (n_index >= 0 && !maze_state->visited[n_index]) {
      unvisited[count++] = dir;
    }
  }

  if (count > 0) {
    // Pick random neighbor
    uint8_t chosen_dir = unvisited[rand() % count];
    int next_index     = neighbor_index(current_index, chosen_dir, rows, cols);

    // Carve walls
    cells[current_index].walls &= ~chosen_dir;
    cells[next_index].walls &= ~opposite_direction(chosen_dir);

    maze_state->visited[next_index] = true;
    maze_state->number_visited++;

    // Push chosen bitmask onto stack for backtracking
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

Cell* create_maze_hybrid(MazeStats* mazeStats, float roomSaturation, AlgoStepFunc* algoStepFuncs,
                         int num_algos) {
  int rows    = mazeStats->rows;
  int columns = mazeStats->columns;

  Cell* cells = malloc(sizeof(Cell) * rows * columns);

  if (!cells) {
    fprintf(stderr, "Error: malloc failed for cells\n");
    return NULL;
  }
  setUpCells(cells, NULL, rows, columns);

  int* sets = malloc(sizeof(int) * rows * columns);
  if (!sets) {
    fprintf(stderr, "Error: malloc failed for sets\n");
    free(cells);
    return NULL;
  }

  // Generate noise for hybrid selection
  float scale      = 0.06f;
  float* noiseGrid = applyNoise(mazeStats->rows, mazeStats->columns, &scale, simplexBilerp, NULL);

  // might make into a function later
  // Allocate maze state
  MazeState maze_state;
  maze_state.visited                = calloc(rows * columns, sizeof(bool));
  maze_state.parent_dirs_stack      = malloc(sizeof(uint8_t) * rows * columns);
  maze_state.parent_dirs_stack_size = -1;

  maze_state.frontier       = malloc(sizeof(int) * rows * columns);
  maze_state.in_frontier    = calloc(rows * columns, sizeof(bool));
  maze_state.frontier_index = -1;

  maze_state.number_visited = 0;
  maze_state.current_index  = -1;

  while (maze_state.number_visited < rows * columns) {

    // If current_index == -1, pick a new random cell
    if (maze_state.current_index == -1) {
      int index;
      do {
        index = rand_int(0, rows * columns - 1);
      } while (maze_state.visited[index]);
      maze_state.current_index = index;
      // For DFS:

      maze_state.parent_dirs_stack_size = -1;

      // For Prim:
      maze_state.frontier_index = -1;
      add_neighbors_to_frontier(index, columns, rows, &maze_state, cells);

      maze_state.visited[index] = true;
      maze_state.number_visited++;
    }

    // Map noise to algorithm
    int algo_index =
        value_map_int(noiseGrid[maze_state.current_index], 0.0f, 1.0f, 0, num_algos - 1);
    AlgoStepFunc algo = algoStepFuncs[algo_index];

    // Execute step (Prim or backtrack)
    algo(cells, rows, columns, &maze_state);
  }

  // Free temporary resources
  free(maze_state.visited);
  free(maze_state.parent_dirs_stack);
  free(maze_state.frontier);
  free(maze_state.in_frontier);
  free(noiseGrid);

  return cells;
}