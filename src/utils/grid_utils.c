#include "grid_utils.h"

int matrix_coords_to_array_coords(int row, int column, int columns) {
  return row * columns + column;
}

void array_coords_to_matrix_coords(int index, int columns, int* row, int* column) {
  *row    = index / columns;
  *column = index % columns;
}

Coord array_coords_to_matrix_coords_struct(int index, int columns) {
  Coord c = {index / columns, index % columns};
  return c;
}