#include "grid_utils.h"
#include "cell.h"
#include "kruskals.h"
#include <stdlib.h>




Cell *kruskalsCreateMaze(MazeStats *mazeStats){
  int rows = mazeStats->rows;
  int columns = mazeStats->columns;
  Cell* cells = malloc(sizeof(Cell) * rows * columns);

  for(int row = 0; row < rows; row++){
    for(int col = 0; col < columns; col++){

      cells[matrix_coords_to_array_coords(row, col, columns)] = create_walled_cell(row, col);

    }
  }

}

