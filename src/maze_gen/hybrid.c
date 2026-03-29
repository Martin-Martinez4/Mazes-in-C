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
  if (value < min_val) {
    value = min_val;
  }
  if (value > max_val) {

    value = max_val;
  }

  float normalized = (value - min_val) / (max_val - min_val);

  int result = min + (int) (normalized * (max - min + 1));

  // clamp to max
  if (result > max) {

    result = max;
  }
  return result;
}

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
  mergeSets(maze_state->sets, coords, current_index);
}

void prim_step(Cell* cells, int rows, int cols, MazeState* maze_state) {

  int current_coord = maze_state->current_index;

  maze_state->frontier[++maze_state->frontier_index] = current_coord;
  maze_state->in_frontier[current_coord]             = true;
  cells[current_coord].walls                         = 0;

  do {
    // copied from prims.c
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
  } while (maze_state->frontier_index >= 0);
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

      mergeSets(maze_state->sets, next_index, current_index);

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

// Kruskals is applied at the end of the hybrid maze so this does nothing
void kruskals_region(Cell* cells, int rows, int cols, MazeState* maze_state) {}

bool is_dead_end(Cell* cell) {
  uint8_t count = 0;
  uint8_t walls = cell->walls;

  uint8_t wall_dirs[4] = {TOP, RIGHT, LEFT, BOTTOM};

  for (int i = 0; i < 4; i++) {
    if ((walls & wall_dirs[i]) == 0) {
      count++;
    }
  }
  return count == 1;
}

uint8_t get_open_end(Cell* cell) {
  uint8_t count = 0;
  uint8_t walls = cell->walls;

  uint8_t wall_dirs[4] = {TOP, RIGHT, LEFT, BOTTOM};

  for (int i = 0; i < 4; i++) {
    if ((walls & wall_dirs[i]) == 0) {
      return wall_dirs[i];
    }
  }

  return 0;
}

int prune_dead_ends(Cell* cells, int rows, int cols) {
  int count = 0;
  // TOP, BOTTOM, LEFT, RIGHT
  const int neigh_coords[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  const int neigh_dirs[4]      = {BOTTOM, TOP, RIGHT, LEFT};
  int neigh_coord;

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      int coords = matrix_coords_to_array_coords(row, col, cols);

      Cell* current_cell = &cells[coords];

      int current_row = row;
      int current_col = col;

      while (is_dead_end(current_cell)) {

        cells[coords].walls = ALL_WALLS;
        count++;

        for (int i = 0; i < 4; i++) {
          int new_row = row + neigh_coords[i][0];
          int new_col = col + neigh_coords[i][1];

          // Bounds check
          if (new_row < 0 || new_row >= rows || new_col < 0 || new_col >= cols) {
            continue;
          }

          int neigh_coord = matrix_coords_to_array_coords(new_row, new_col, cols);
          cells[neigh_coord].walls |= neigh_dirs[i];
        }

        int8_t open_end = get_open_end(current_cell);
        if (open_end == -1) {
          break;
        }

        switch (open_end) {
        case TOP:
          current_row -= 1;
          break;
        case RIGHT:
          current_col += 1;
          break;
        case BOTTOM:
          current_row += 1;
          break;
        case LEFT:
          current_col -= 1;
          break;
        default:
          break;
        }
        coords       = matrix_coords_to_array_coords(current_row, current_col, cols);
        current_cell = &cells[coords];
      }
    }
  }

  return count;
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

  while(prune_dead_ends(cells, rows, columns) > 2);

  return cells;
}