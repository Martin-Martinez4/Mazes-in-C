#include "cell.h"

#include <stdio.h>
#include <stdlib.h>

// return by copy for now
Cell create_walled_cell(int row, int column) {
  Cell c;
  c.row    = row;
  c.column = column;
  c.walls  = ALL_WALLS;

  return c;
}

Edge create_edge(Cell* cell, uint8_t direction) {
  Edge e;
  e.cell_ptr = cell;

  switch (direction) {
  case TOP:
    e.direction          = TOP;
    e.opposite_direction = BOTTOM;
    break;

  case LEFT:
    e.direction          = LEFT;
    e.opposite_direction = RIGHT;
    break;

  case RIGHT:
    e.direction          = RIGHT;
    e.opposite_direction = LEFT;
    break;

  case BOTTOM:
    e.direction          = BOTTOM;
    e.opposite_direction = TOP;
    break;
  default:
    fprintf(stderr, "Error: invalid direction %u in create_edge()\n", direction);
    abort(); // or exit(EXIT_FAILURE);
  }

  return e;
}