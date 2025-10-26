#ifndef H_GRID_UTILS
#define H_GRID_UTILS

typedef struct Coord{
  int row;
  int column;
} Coord;

int coordsMatrixToArray(int x, int y, int columns);
void array_coords_to_matrix_coords(int index, int columns, int *row, int *column);
Coord array_coords_to_matrix_coords_struct(int index, int columns);

#endif