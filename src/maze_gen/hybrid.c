#include "hybrid.h"
#include "noise.h"
#include <stdlib.h>
#include <stdio.h>

// copy from kruskal's implementation
void shuffleEdgeArray(Edge* array, int length) {
  for (int i = length - 1; i >= 1; i--) {
    int j     = rand() % (i + 1);
    Edge temp = array[i];
    array[i]  = array[j];
    array[j]  = temp;
  }
}

void setUpCells(Cell* cells, Rooms* rooms, int rows, int columns) {
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

void setUpSets(int* sets, Rooms* rooms, int rows, int columns) {
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

void setUpEdges(Edge* edges, Cell* cells, int rows, int columns) {
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

int find(int* sets, int x) {
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

void mergeSets(int* sets, int current, int change) {
  int current_root = find(sets, current);
  int change_root  = find(sets, change);

  if (current_root != change_root) {
    sets[change_root] = current_root;
  }
}

void setUpEdges(Edge* edges, Cell* cells, int rows, int columns) {
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

int prim_step(ScratchBuffer scratch, Cell* cells, float* noise, int* sets, int row, int column, int columns) {
    int current_coord = matrix_coords_to_array_coords(row, column, columns);

    int neigh_coord_1 = matrix_coords_to_array_coords(row + 1, column, columns);
    int neigh_coord_2 = matrix_coords_to_array_coords(row, column + 1, columns);
    int neigh_coord_3 = matrix_coords_to_array_coords(row - 1, column, columns);
    int neigh_coord_3 = matrix_coords_to_array_coords(row, column - 1, columns);
}

int kruskal_step(ScratchBuffer scratch, Cell* cells, float* noise, int* sets, int row, int column) {}

Cell* create_maze_hybrid(MazeStats* mazeStats, Rooms* rooms, AlgoStepFunc** algoStepFuncs,
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
  setUpCells(cells, rooms, rows, columns);

  int* sets = malloc(sizeof(int) * rows * columns);
  if (!sets) {
    fprintf(stderr, "Error: malloc failed for sets\n");
    free(cells);
    return NULL;
  }

  // Set keeps track of state of overall maze
  setUpSets(sets, rooms, rows, columns);

  // maybe change later?
  int edges_len = (rows * (columns - 1)) + ((rows - 1) * columns);
  ScratchBuffer scratch;
  scratch.edges = malloc(edges_len * sizeof(Edge));

  // Figure out later how to get noise info
  float scale      = 0.05f;
  float* noiseGrid = applyNoise(mazeStats->rows, mazeStats->columns, &scale, simplexBilerp, NULL);
}