#include "game_map.h"
#include "grid_utils.h"
#include "stdlib.h"
#include "stdio.h"

static void setUpCells(Cell* cells, Rooms* rooms, bool* visited, int rows, int columns) {
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      cells[matrix_coords_to_array_coords(row, col, columns)] = create_walled_cell(row, col);
    }
  }

  if (rooms == NULL) {
    printf("NULL rooms: %d\n", rooms == NULL);
    return;
  }
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
        // if (row == y || row == y + height - 1 || col == x || col == x + width - 1) {
        //   continue;
        // } else {

          visited[matrix_coords_to_array_coords(row, col, columns)] = true;
        // }
      }
    }
    // cells[matrix_coords_to_array_coords(y, (int) (x + (width / 2)), columns)].walls     = 0;
    // cells[matrix_coords_to_array_coords(y - 1, (int) (x + (width / 2)), columns)].walls = 0;

    // cells[matrix_coords_to_array_coords(y + height, (int) (x + (width / 2)), columns)].walls = 0;
    // cells[matrix_coords_to_array_coords(y + height - 1, (int) (x + (width / 2)), columns)].walls
    // =
    //     0;

    // cells[matrix_coords_to_array_coords((int) (y + height / 2), x, columns)].walls     = 0;
    // cells[matrix_coords_to_array_coords((int) (y + height / 2), x - 1, columns)].walls = 0;

    // cells[matrix_coords_to_array_coords((int) (y + height / 2), x + width, columns)].walls     =
    // 0; cells[matrix_coords_to_array_coords((int) (y + height / 2), x + width - 1, columns)].walls
    // = 0;
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
int idx(int x, int y, int cols) {
  return y * cols + x;
}
int manhattan(int x1, int y1, int x2, int y2) {
  return abs(x1 - x2) + abs(y1 - y2);
}
void carve_between(Cell* cells, int current, int next, int cols) {
  int cx = current % cols;
  int cy = current / cols;

  int nx = next % cols;
  int ny = next / cols;

  if (nx == cx + 1) {
    // right
    cells[current].walls &= ~RIGHT;
    cells[next].walls    &= ~LEFT;
  }
  else if (nx == cx - 1) {
    // left
    cells[current].walls &= ~LEFT;
    cells[next].walls    &= ~RIGHT;
  }
  else if (ny == cy + 1) {
    // down
    cells[current].walls &= ~BOTTOM;
    cells[next].walls    &= ~TOP;
  }
  else if (ny == cy - 1) {
    // TOP
    cells[current].walls &= ~TOP;
    cells[next].walls    &= ~BOTTOM;
  }
}
void connect_rooms(Cell* cells, Rooms* rooms, bool* visited, int rows, int cols) {
  
}

Cell* create_game_map(MazeStats* mazeStats, Rooms* rooms, float* noise_grid,
                      AlgoStepFunc* algoStepFuncs) {
  printf("Rooms Len: %d\n", rooms->length);
  int rows    = mazeStats->rows;
  int columns = mazeStats->columns;

  bool* visited = malloc(sizeof(bool) * rows * columns);
  if (!visited) {
    fprintf(stderr, "Error: malloc failed for visited\n");
    return NULL;
  }

  Cell* cells = malloc(sizeof(Cell) * rows * columns);
  if (!cells) {
    fprintf(stderr, "Error: malloc failed for cells\n");
    return NULL;
  }
  setUpCells(cells, rooms, visited, rows, columns);

  int* sets = malloc(sizeof(int) * rows * columns);
  if (!sets) {
    fprintf(stderr, "Error: malloc failed for sets\n");
    free(cells);
    return NULL;
  }
  setUpSets(sets, rooms, rows, columns);

  connect_rooms(cells, rooms, visited, rows, columns);

  return cells;
}