#ifndef H_CELLS
#define H_CELLS

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
  unsigned int walls : 4;
}Cell;

typedef struct Edge{
  int row;
  int column;
  Cell* cell_ptr;
}Edge;

#endif

