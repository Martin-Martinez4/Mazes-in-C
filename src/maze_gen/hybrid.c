#include "hybrid.h"
#include "noise.h"
#include "grid_utils.h"
#include <stdlib.h>
#include <stdio.h>

// copy from kruskal's implementation
void shuffleEdgeArray_h(Edge* array, int length) {
  for (int i = length - 1; i >= 1; i--) {
    int j     = rand() % (i + 1);
    Edge temp = array[i];
    array[i]  = array[j];
    array[j]  = temp;
  }
}

void setUpCells_h(Cell* cells, Rooms* rooms, int rows, int columns) {
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

void setUpSets_h(int* sets, Rooms* rooms, int rows, int columns) {
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

void setUpEdges_h(Edge* edges, Cell* cells, int rows, int columns) {
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

  shuffleEdgeArray_h(edges, (rows * (columns - 1)) + ((rows - 1) * columns));
}

int find_h(int* sets, int x) {
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

void mergeSets_h(int* sets, int current, int change) {
  int current_root = find_h(sets, current);
  int change_root  = find_h(sets, change);

  if (current_root != change_root) {
    sets[change_root] = current_root;
  }
}

// so many parameters
int add_neighbors_to_frontier_h(int* frontier, bool* in_frontier, int frontier_index, Cell* cells,
                                float* noise, int* sets, int current_coord, int columns, int rows) {
  int row;
  int column;
  array_coords_to_matrix_coords(current_coord, columns, &row, &column);
  printf("before: %d ", frontier_index);
  // check for out of bounds and all that later
  // thought if not viable coord should be a negative number.
  int neigh_coord_1 =
      row + 1 >= rows ? -1 : matrix_coords_to_array_coords(row + 1, column, columns);
  int neigh_coord_2 =
      column + 1 >= columns ? -1 : matrix_coords_to_array_coords(row, column + 1, columns);
  int neigh_coord_3 = row - 1 < 0 ? -1 : matrix_coords_to_array_coords(row - 1, column, columns);
  ;
  int neigh_coord_4 = column - 1 < 0 ? -1 : matrix_coords_to_array_coords(row, column - 1, columns);
  ;

  int neigh_cords[4] = {neigh_coord_1, neigh_coord_2, neigh_coord_3, neigh_coord_4};

  for (int i = 0; i < 4; i++) {

    int coord = neigh_cords[i];
    if (coord >= 0) {
      if (!in_frontier[coord] && cells[coord].walls == ALL_WALLS) {
        frontier[++frontier_index] = coord;
        in_frontier[coord]         = true;
      }
    }
  }
  printf("after: %d\n", frontier_index);

  return frontier_index;
}

void carve_to_visited_neighbor_h(Cell* cells, bool* in_frontier, int current_index, int columns,
                                 int rows) {
  // [row][column] - row-major order
  // TOP BOTTOM LEFT RIGHT
  int current_row;
  int current_col;
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
    // But Prim must carve to a visited neighbor.
    if (cells[neigh_index].walls != ALL_WALLS) {
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

      default:
        break;
      }
    }
  }

  if (eligibles_index > -1) {
    Cell* current_cell = &cells[matrix_coords_to_array_coords(current_row, current_col, columns)];
    Cell* n_cell;
    int dir = eligibles[rand() % (eligibles_index + 1)];

    switch (dir) {
    case LEFT:
      current_cell->walls &= ~LEFT;
      n_cell = &cells[matrix_coords_to_array_coords(current_row, current_col - 1, columns)];
      n_cell->walls &= ~RIGHT;
      break;

    case RIGHT:
      current_cell->walls &= ~RIGHT;
      n_cell = &cells[matrix_coords_to_array_coords(current_row, current_col + 1, columns)];
      n_cell->walls &= ~LEFT;
      break;

    case TOP:
      current_cell->walls &= ~TOP;
      n_cell = &cells[matrix_coords_to_array_coords(current_row - 1, current_col, columns)];
      n_cell->walls &= ~BOTTOM;
      break;

    case BOTTOM:
      current_cell->walls &= ~BOTTOM;
      n_cell = &cells[matrix_coords_to_array_coords(current_row + 1, current_col, columns)];
      n_cell->walls &= ~TOP;
      break;

    default:
      break;
    }
  }
}


int prim_step(ScratchBuffer* scratch, Cell* cells, float* noise, int* sets, int row, int column,
              int columns, int rows) {
  bool* in_frontier = calloc(columns * rows, sizeof(bool));

  int* frontier      = (int*) scratch->edges;
  int frontier_index = -1;

  int current_coord = matrix_coords_to_array_coords(row, column, columns);

  frontier[++frontier_index] = current_coord;
  in_frontier[current_coord] = true;
  cells[current_coord].walls = 0;

  do {
    // copied from prims.c
    int random_index = rand() % (frontier_index + 1);
    current_coord    = frontier[random_index];

    // swap top with rand; random selection from stack
    int temp                 = frontier[frontier_index];
    frontier[frontier_index] = frontier[random_index];
    frontier[random_index]   = temp;
    frontier_index--;

    carve_to_visited_neighbor_h(cells, in_frontier, current_coord, columns, rows);
    in_frontier[current_coord] = false;

    frontier_index = add_neighbors_to_frontier_h(frontier, in_frontier, frontier_index, cells,
                                                 noise, sets, current_coord, columns, rows);
  } while (frontier_index >= 0);

  free(in_frontier);

  return current_coord;
}

int kruskal_step(ScratchBuffer scratch, Cell* cells, float* noise, int* sets, int row, int column,
                 int columns, int rows) {}

// size is # of algos
Cell* create_maze_hybrid(MazeStats* mazeStats, float roomSaturation, AlgoStepFunc** algoStepFuncs,
                         int size) {
  // divide 1 by the number of algos
  float divide = 1.0f / size;
  int rows     = mazeStats->rows;
  int columns  = mazeStats->columns;
  Cell* cells  = malloc(sizeof(Cell) * rows * columns);

  if (!cells) {
    fprintf(stderr, "Error: malloc failed for cells\n");
    return NULL;
  }
  setUpCells_h(cells, NULL, rows, columns);

  int* sets = malloc(sizeof(int) * rows * columns);
  if (!sets) {
    fprintf(stderr, "Error: malloc failed for sets\n");
    free(cells);
    return NULL;
  }

  // Set keeps track of state of overall maze
  setUpSets_h(sets, NULL, rows, columns);

  // maybe change later?
  int edges_len = (rows * (columns - 1)) + ((rows - 1) * columns);
  ScratchBuffer scratch;
  scratch.edges = malloc(edges_len * sizeof(Edge));

  // Figure out later how to get noise info
  float scale      = 0.05f;
  float* noiseGrid = applyNoise(mazeStats->rows, mazeStats->columns, &scale, simplexBilerp, NULL);
  cells[0].walls &= ~ALL_WALLS;
  prim_step(&scratch, cells, noiseGrid, sets, 0, 0, columns, rows);

  return cells;
}