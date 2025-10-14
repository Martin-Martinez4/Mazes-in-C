#ifndef H_CELLS
#define H_CELLS

// 0111
#define TOP_REMOVER 14
// 1011
#define LEFT_REMOVER 13
// 1101
#define BOTTOM_REMOVER 11
// 1110
#define RIGHT_REMOVER 7

// TOP_CHECK & num =0 means there is no top wall
// 1000
#define TOP_CHECK 1
// 0100
#define LEFT_CHECK 2
// 0010
#define BOTTOM_CHECK 4
// 0001
#define RIGHT_CHECK 8

typedef struct Cell {
  int row;
  int column;
  unsigned int walls : 4;
}Cell;

#endif

