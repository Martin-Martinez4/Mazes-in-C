#include <stdlib.h>
#include "cell.h"

// return by copy for now
Cell create_walled_cell(int row, int column){
  Cell c;
  c.row = row;
  c.column = column;
  c.walls = ALL_WALLS;

  return c;
}