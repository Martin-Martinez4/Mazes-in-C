#include <stdlib.h>
#include "cell.h"

Cell* create_walled_cell(int row, int column){
  Cell* c = malloc(sizeof(Cell));
  c->row = row;
  c->column = column;
  c->walls = ALL_WALLS;

  return c;
}