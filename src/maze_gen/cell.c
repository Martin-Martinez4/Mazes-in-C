#include "cell.h"
#include "grid_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

int BFS_count(Cell* cells, int rows, int columns) {
  bool* visited = calloc(rows * columns, sizeof(bool));
  int* stack     = malloc(sizeof(int) * rows * columns);
  int stack_head = 0;

  int current;
  int row;
  int column;
  uint8_t walls;

  int next;

  int count           = 0;
  stack[stack_head]   = 0;
  visited[0] = true;

  while (stack_head >= 0) {
    current = stack[stack_head];
    count++;
    --stack_head;

    walls = cells[current].walls;

    array_coords_to_matrix_coords(current, columns, &row, &column);

    if ((TOP & walls) == 0 && row > 0) {
      next = matrix_coords_to_array_coords(row - 1, column, columns);
      if (!visited[next]) {
        stack[++stack_head] = next;
        visited[next]       = true;
      }
    }

    if ((BOTTOM & walls) == 0 && row < rows-1) {
      next = matrix_coords_to_array_coords(row + 1, column, columns);
      if (!visited[next]) {
        stack[++stack_head] = next;
        visited[next]       = true;
      }
    }

    if ((RIGHT & walls) == 0 && column < columns - 1) {
      next = matrix_coords_to_array_coords(row, column + 1, columns);
      if (!visited[next]) {
        stack[++stack_head] = next;
        visited[next]       = true;
      }
    }

    if ((LEFT & walls) == 0 && column > 0) {
      next = matrix_coords_to_array_coords(row, column - 1, columns);
      if (!visited[next]) {
        stack[++stack_head] = next;
        visited[next]       = true;
      }
    }
  }

  free(stack);
  free(visited);
  return count;
}