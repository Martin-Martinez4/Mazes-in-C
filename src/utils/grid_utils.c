#include "grid_utils.h"


int coordsMatrixToArray(int x, int y, int columns){
  return x * columns + y;
}

void array_coords_to_matrix_coords(int index, int columns, int *row, int *column){
  *row = index/columns;
  *column = index%columns;
}

Coord array_coords_to_matrix_coords_struct(int index, int columns){
  Coord c = { index / columns, index % columns };
  return c;
}