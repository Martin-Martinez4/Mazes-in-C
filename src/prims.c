#include "prims.h"
#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "grid_utils.h"
#include "rooms.h"

void setUpCells_prims(Cell* cells, Rooms* rooms, int rows, int columns) {
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      cells[matrix_coords_to_array_coords(row, col, columns)] = create_walled_cell(row, col);
    }
  }
}


// array of the size of the matrix of bools to keep track of visited status

Cell* prims_create_maze(MazeStats* mazeStats, Rooms* rooms) {
  int rows    = mazeStats->rows;
  int columns = mazeStats->columns;
  Cell* cells = malloc(sizeof(Cell) * rows * columns);

  if (!cells) {
    fprintf(stderr, "Error: malloc failed for cells in prims\n");
    return NULL;
  }

  setUpCells_prims(cells, rooms, rows, columns);

  
  // setup array of neighbors (frontier)
  // for now frontier is going to be half the size of the grid
  int *frontier = malloc(sizeof(int) * rows * columns);
  int frontier_index = -1;
  
  // created visited and set to all false
  bool *visited = malloc(sizeof(bool) * rows * columns);
  for(int i = 0; i < rows * columns; i++){
    visited[i] = false;
  }
  
  int current_col = rand_int(0, columns - 1);
  int current_row = rand_int(0, rows - 1);

  // directions can be placed in an array and iterated through. 

  do{

    visited[matrix_coords_to_array_coords(current_row, current_col, columns)] = true;

    if(current_col < columns - 1){

      int indx = matrix_coords_to_array_coords(current_row, current_col + 1, columns);
      if(!visited[indx]){

        frontier[++frontier_index] = indx;
      }
    }
    if(current_col > 0){

       int indx = matrix_coords_to_array_coords(current_row, current_col - 1, columns);
       if(!visited[indx]){

         frontier[++frontier_index] = indx;
   
       }
    }
    if(current_row < rows - 1){

      int indx = matrix_coords_to_array_coords(current_row + 1, current_col, columns);
      if(!visited[indx]){
        
        frontier[++frontier_index] = indx;        
       }
    }
    if(current_row > 0){

      int indx = matrix_coords_to_array_coords(current_row - 1, current_col, columns);
      if(!visited[indx]){
        
        frontier[++frontier_index] = indx;        
       }
    }

    array_coords_to_matrix_coords(frontier[frontier_index], columns, &current_row, &current_col);

    // Same effect as poping
    frontier_index--;


    

  }while(frontier_index > -1);



}