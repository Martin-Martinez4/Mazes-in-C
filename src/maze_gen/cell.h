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

// 0111
#define TOP_REMOVER ~TOP
// 1011
#define LEFT_REMOVER ~LEFT
// 1101
#define BOTTOM_REMOVER ~BOTTOM
// 1110
#define RIGHT_REMOVER ~RIGHT

typedef struct Cell {
  int row;
  int column;
  uint8_t walls;
} Cell;

typedef struct Edge {
  Cell* cell_ptr;
  uint8_t direction;
  uint8_t opposite_direction;
} Edge;

Cell create_walled_cell(int row, int column);
Edge create_edge(Cell* cell, uint8_t direction);

#endif
