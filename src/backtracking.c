#include <string.h>

#include "backtracking.h"
#include "grid_utils.h"
#include "rand_utils.h"

void shuffleArray(void* array, int length, size_t element_size) {
  // just to get the pointer ot the first thing
  char* arr = (char*)array;

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

  if(visited[array_coords]){
    return;
  }
  visited[array_coords] = true;


  int sides[4] = {TOP, LEFT, BOTTOM, RIGHT};
  shuffleArray(sides, 4, sizeof(int));

  for(int i = 0; i < 4; i++){
    int neigh_index = 0;

    switch (sides[i])
    {

    case TOP:
      if(row == 0){
        continue;
      }

      neigh_index = matrix_coords_to_array_coords(row-1, column, columns); 
      if (!visited[neigh_index]) {
        cells[array_coords].walls &= ~TOP;
        cells[neigh_index].walls &= ~BOTTOM;
        backtrack(cells, visited, row - 1, column, rows, columns);
      }
      break;

    case LEFT:
      if(column == 0){
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
      if(row == rows-1){
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
      if(column == columns-1){
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
      cells[matrix_coords_to_array_coords(row, col, columns)] =
          create_walled_cell(row, col);
    }
  }
}

Cell* backtrackingCreateMaze(MazeStats* mazeStates, Rooms* rooms) {
  int columns = mazeStates->columns;
  int rows = mazeStates->rows;

  bool* visited = malloc(sizeof(bool) * columns * rows);

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      visited[matrix_coords_to_array_coords(row, col, columns)] = false;
    }
  }

  Cell* cells = malloc(sizeof(Cell) * columns * rows);

  // ------- Get rid of NULL when handling rooms -------
  setUpCellsbt(cells, NULL, rows, columns);

  int start_col = rand_int(0, columns - 1);
  int start_row = rand_int(0, rows - 1);

  backtrack(cells, visited, start_row, start_col, rows, columns);

  free(visited);

  return cells;
}