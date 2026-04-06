#ifndef H_CELLS
#define H_CELLS
#include <stdint.h>

// TOP_CHECK & num = 0 means there is no top wall
// 1000
#define TOP 1
// 0100
#define LEFT 2
// 0010
#define BOTTOM 4
// 0001
#define RIGHT 8

#define ALL_WALLS (TOP | RIGHT | BOTTOM | LEFT)

// Triangle Cell things
#define IS_DOWN(r, c) (((r) + (c)) % 2 == 0)

#define TRI_LEFT  1
#define TRI_RIGHT  2
#define TRI_BASE  4
#define TRI_ALL_WALL (TRI_LEFT | TRI_RIGHT | TRI_BASE)

// cells are represented using uint8_t
// 8 walls can be represented
#define MAX_NEIGHBORS 8

typedef struct {
    int dr, dc;
    uint8_t dir;
    uint8_t opposite;
} TriNeighbor;

typedef struct Cell {
  int row;
  int column;
  uint8_t max_edges;
  uint8_t walls;
  uint8_t num_neighbors;
  int neighbors[MAX_NEIGHBORS];
  uint8_t dirs[MAX_NEIGHBORS];
  uint8_t opposite_dirs[MAX_NEIGHBORS];
  uint8_t cell_all_walls;

} Cell;

typedef struct Edge {
    Cell* cell_ptr;       // pointer to the current cell
    int neighbor_idx;     // index of the neighbor cell
    uint8_t direction;    // wall bit in current cell
    uint8_t opposite_direction; // wall bit in neighbor cell
} Edge;

// Kruskal

Cell create_walled_cell(int row, int column);
Cell create_square_cell(int row, int column, int rows, int columns);
Cell create_tri_cell(int row, int column, int rows, int columns);
Edge create_edge(Cell* cell, int direction_index);

int BFS_count(Cell* cells, int rows, int columns);

#endif
