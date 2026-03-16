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
int add_neighbors_to_frontier_h(int* frontier, int frontier_index, Cell* cells, float* noise,
                                int* sets, int current_coord, int columns) {
  int row;
  int column;
  array_coords_to_matrix_coords(current_coord, columns, &row, &column);
  // check for out of bounds and all that later
  // thought if not viable coord should be a negative number.
  int neigh_coord_1 = matrix_coords_to_array_coords(row + 1, column, columns);
  int neigh_coord_2 = matrix_coords_to_array_coords(row, column + 1, columns);
  int neigh_coord_3 = matrix_coords_to_array_coords(row - 1, column, columns);
  int neigh_coord_4 = matrix_coords_to_array_coords(row, column - 1, columns);

  int neigh_cords[4] = {neigh_coord_1, neigh_coord_2, neigh_coord_3, neigh_coord_4};

  for (int i = 0; i < 4; i++) {

    int coord = neigh_cords[i];
    if (coord >= 0) {
      if (sets[coord] != sets[current_coord] && cells[coord].walls == ALL_WALLS) {
        frontier[++frontier_index] = coord;
      }
    }
  }

  return frontier_index;
}

void carve_to_visited_neighbor_h(Cell* cells, int current_index, int columns, int rows) {
  int row, col;
  array_coords_to_matrix_coords(current_index, columns, &row, &col);

  const int d_coords[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  const int wall_dir[4]    = {TOP, BOTTOM, LEFT, RIGHT};
  const int opp_dir[4]     = {BOTTOM, TOP, RIGHT, LEFT};

  int eligibles[4];
  int count = 0;

  for (int i = 0; i < 4; i++) {
    int nr = row + d_coords[i][0];
    int nc = col + d_coords[i][1];

    if (nr < 0 || nr >= rows || nc < 0 || nc >= columns)
      continue;

    int n_index = matrix_coords_to_array_coords(nr, nc, columns);

    // Neighbor is **already in maze**
    if (cells[n_index].walls != ALL_WALLS) {
      eligibles[count++] = i;
    }
  }

  if (count == 0)
    return; // no neighbor in maze to carve into

  int chosen  = eligibles[rand() % count];
  int n_row   = row + d_coords[chosen][0];
  int n_col   = col + d_coords[chosen][1];
  int n_index = matrix_coords_to_array_coords(n_row, n_col, columns);

  // carve walls
  cells[current_index].walls &= ~wall_dir[chosen];
  cells[n_index].walls &= ~opp_dir[chosen];
}

int prim_step(ScratchBuffer* scratch, Cell* cells, float* noise, int* sets, int row, int column,
              int columns, int rows) {
  int frontier_index = -1;
  int* frontier      = (int*) scratch->edges;

  int current_coord = matrix_coords_to_array_coords(row, column, columns);

  frontier_index = add_neighbors_to_frontier_h(frontier, frontier_index, cells, noise, sets,
                                               current_coord, columns);

  while (frontier_index >= 0) {
    // copied from prims.c
    int random_index  = rand() % (frontier_index + 1);
    int current_index = frontier[random_index];

    // swap top with rand; random selection from stack
    int temp                 = frontier[frontier_index];
    frontier[frontier_index] = frontier[random_index];
    frontier[random_index]   = temp;
    frontier_index--;

    carve_to_visited_neighbor_h(cells, current_index, rows, columns);
    frontier_index = add_neighbors_to_frontier_h(frontier, frontier_index, cells, noise, sets,
                                                 current_index, columns);
  }
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