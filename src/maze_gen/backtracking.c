#include <string.h>

#include "backtracking.h"
#include "grid_utils.h"
#include "rand_utils.h"

void shuffleArray(void* array, int length, size_t element_size) {
  // just to get the pointer ot the first thing
  char* arr = (char*) array;

  for (int i = length - 1; i >= 1; i--) {
    int j = rand() % (i + 1);

    char* temp = malloc(element_size);

    // swap arr[i] and arr[j]
    memcpy(temp, arr + i * element_size, element_size);
    memcpy(arr + i * element_size, arr + j * element_size, element_size);
    memcpy(arr + j * element_size, temp, element_size);
  }
}

void backtrack(Cell* cells, bool* visited, int row, int column, int rows, int columns) {

  int array_coords = matrix_coords_to_array_coords(row, column, columns);

  if (visited[array_coords]) {
    return;
  }
  visited[array_coords] = true;

  int sides[4] = {TOP, LEFT, BOTTOM, RIGHT};
  shuffleArray(sides, 4, sizeof(int));

  for (int i = 0; i < 4; i++) {
    int neigh_index = 0;

    switch (sides[i]) {

    case TOP:
      if (row == 0) {
        continue;
      }

      neigh_index = matrix_coords_to_array_coords(row - 1, column, columns);
      if (!visited[neigh_index]) {
        cells[array_coords].walls &= ~TOP;
        cells[neigh_index].walls &= ~BOTTOM;
        backtrack(cells, visited, row - 1, column, rows, columns);
      }
      break;

    case LEFT:
      if (column == 0) {
        continue;
      }

      neigh_index = matrix_coords_to_array_coords(row, column - 1, columns);
      if (!visited[neigh_index]) {
        cells[array_coords].walls &= ~LEFT;
        cells[neigh_index].walls &= ~RIGHT;
        backtrack(cells, visited, row, column - 1, rows, columns);
      }
      break;

    case BOTTOM:
      if (row == rows - 1) {
        continue;
      }

      neigh_index = matrix_coords_to_array_coords(row + 1, column, columns);
      if (!visited[neigh_index]) {

        cells[array_coords].walls &= ~BOTTOM;
        cells[neigh_index].walls &= ~TOP;
        backtrack(cells, visited, row + 1, column, rows, columns);
      }
      break;

    case RIGHT:
      if (column == columns - 1) {
        continue;
      }

      neigh_index = matrix_coords_to_array_coords(row, column + 1, columns);
      if (!visited[neigh_index]) {

        cells[array_coords].walls &= ~RIGHT;
        cells[neigh_index].walls &= ~LEFT;
        backtrack(cells, visited, row, column + 1, rows, columns);
      }
      break;

    default:
      break;
    }
  }
}

// Idea: Make reusable and pass in a function to call to handle the rooms
void setUpCellsbt(Cell* cells, Rooms* rooms, int rows, int columns) {
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      cells[matrix_coords_to_array_coords(row, col, columns)] = create_walled_cell(row, col);
    }
  }

  if (rooms == NULL) {
    return;
  }

  for (int i = 0; i < rooms->length; i++) {
    Room room = rooms->data[i];
    int set   = matrix_coords_to_array_coords(room.aabb.y, room.aabb.x, columns);

    int x      = room.aabb.x;
    int y      = room.aabb.y;
    int width  = room.aabb.width;
    int height = room.aabb.height;

    for (int row = y; row < (y + height); row++) {
      for (int col = x; col < (x + width); col++) {
        int walls = 0;
        if (row == y)
          walls |= TOP;
        if (row == y + height - 1)
          walls |= BOTTOM;
        if (col == x)
          walls |= LEFT;
        if (col == x + width - 1)
          walls |= RIGHT;
        cells[matrix_coords_to_array_coords(row, col, columns)].walls = walls;
      }
    }
  }
}

Cell* backtrackingCreateMaze(MazeStats* mazeStates, Rooms* rooms) {
  int columns = mazeStates->columns;
  int rows    = mazeStates->rows;

  bool* visited = malloc(sizeof(bool) * columns * rows);

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      visited[matrix_coords_to_array_coords(row, col, columns)] = false;
    }
  }

  if (rooms != NULL) {

    for (int i = 0; i < rooms->length; i++) {
      Room room = rooms->data[i];
      int set   = matrix_coords_to_array_coords(room.aabb.y, room.aabb.x, columns);

      int x      = room.aabb.x;
      int y      = room.aabb.y;
      int width  = room.aabb.width;
      int height = room.aabb.height;

      for (int row = y; row < (y + height); row++) {
        for (int col = x; col < (x + width); col++) {
          int walls = 0;
          if (row == y || row == y + height - 1 || col == x || col == x + width - 1) {
            continue;
          } else {

            visited[matrix_coords_to_array_coords(row, col, columns)] = true;
          }
        }
      }
    }
  }

  Cell* cells = malloc(sizeof(Cell) * columns * rows);

  // ------- Get rid of NULL when handling rooms -------
  setUpCellsbt(cells, rooms, rows, columns);

  typedef struct {
    int r, c;
  } CellPos;

  int freeCount      = 0;
  CellPos* freeCells = malloc(sizeof(CellPos) * rows * columns);

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      if (!visited[matrix_coords_to_array_coords(row, col, columns)]) {
        freeCells[freeCount++] = (CellPos){row, col};
      }
    }
  }

  if (freeCount > 0) {
    CellPos start = freeCells[rand_int(0, freeCount - 1)];
    backtrack(cells, visited, start.r, start.c, rows, columns);
  }

  // check for cells that are not visited
  for(int row = 0; row < rows; row++){
    for(int col = 0; col < columns; col++){
      int index = matrix_coords_to_array_coords(row, col, columns);
      if(!visited[index]){
        backtrack(cells, visited, row, col, rows, columns);
      }
    }
  }

  free(freeCells);
  free(visited);

  return cells;
}