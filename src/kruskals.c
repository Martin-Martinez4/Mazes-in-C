#include "grid_utils.h"
#include "cell.h"
#include "kruskals.h"
#include <stdlib.h>
#include <stdio.h>


void shuffleEdgeArray(Edge* array, int length){
  for(int i = length - 1; i >= 1; i--){
    int j = rand() % (i + 1);
    Edge temp = array[i];
    array[i] = array[j];
    array[j] = temp;
  }
}

void setUpCells(Cell* cells, int rows, int columns){
  for(int row = 0; row < rows; row++){
    for(int col = 0; col < columns; col++){

      cells[matrix_coords_to_array_coords(row, col, columns)] = create_walled_cell(row, col);

    }
  }
}

void setUpSets(int* sets, int rows, int columns){
  for(int i = 0; i < rows * columns; i++){
    sets[i] = i;
  }
}

void setUpEdges(Edge* edges, Cell* cells, int rows, int columns){
  int indx = 0;
  for(int row = 0; row < rows; row++){
    for(int col = 0; col < columns; col++){
      Cell *c = &cells[matrix_coords_to_array_coords(row, col, columns)];

      // // directions are represented by 1,2,4,8
      // for(int side = 1; side <= 8; side *= 2){

      //   edges[indx] = create_edge(c, side);
      //   indx++;
      // }
      // Right neighbor
      if (col + 1 < columns) edges[indx++] = create_edge(c, RIGHT);
      // Bottom neighbor
      if (row + 1 < rows) edges[indx++] = create_edge(c, BOTTOM);
    }
  }

  shuffleEdgeArray(edges, (rows * (columns - 1)) + ((rows - 1) * columns));
}

int find(int* sets, int x){
  int root = x;

  // find true root
  while(sets[root] != root){
    root = sets[root];
  }

  // compress root
  while(sets[x] != root){
    int next = sets[x];
    sets[x] = root;
    x = next;
  }

  return root;
}

void mergeSets(int* sets, int current, int change){
  int current_root = find(sets, current);
  int change_root = find(sets, change);

  if(current_root != change_root){
    sets[change_root] = current_root;
  }
}

Cell *kruskalsCreateMaze(MazeStats *mazeStats){
  int rows = mazeStats->rows;
  int columns = mazeStats->columns;
  Cell* cells = malloc(sizeof(Cell) * rows * columns);

  if (!cells) {
    fprintf(stderr, "Error: malloc failed for cells\n");
    return NULL;
  }
  setUpCells(cells, rows, columns);

  int* sets = malloc(sizeof(int) * rows * columns);
  if (!sets) {
    fprintf(stderr, "Error: malloc failed for sets\n");
    free(cells);
    return NULL;
  }
  setUpSets(sets, rows, columns);

  int edges_len = (rows * (columns - 1)) + ((rows - 1) * columns);
  Edge* edges = malloc(sizeof(Edge) * edges_len);
  if (!edges) {
    fprintf(stderr, "Error: malloc failed for edges\n");
    free(cells);
    free(sets);
    return NULL;
  }
  setUpEdges(edges, cells, rows, columns);
  
  int top = edges_len - 1;

  while(top >= 0){
    Edge e = edges[top];
    Cell* current = e.cell_ptr;

    int neighbor_row;
    int neighbor_column; 


    int current_row = current->row;
    int current_column = current->column;

    switch(e.direction){

      case RIGHT:
        neighbor_row = current_row;
        neighbor_column = current_column + 1; 
        break;
      
      case BOTTOM:
        neighbor_row = current_row + 1;
        neighbor_column = current_column; 
        break;
     
      default:
        fprintf(stderr, "Error: invalid direction %u in kruskalsCreateMaze()\n", e.opposite_direction);
        abort(); // or exit(EXIT_FAILURE);
    }

    if(neighbor_row >= 0 && neighbor_row < rows && neighbor_column >= 0 && neighbor_column < columns ){
      int current_coords = matrix_coords_to_array_coords(current_row, current_column, columns);
      int neighbor_coords = matrix_coords_to_array_coords(neighbor_row, neighbor_column, columns);

      // get sets
      int neighbor_set = find(sets, neighbor_coords);
      int current_set = find(sets, current_coords);
      
      if(neighbor_set != current_set){
        // remove walls
        cells[current_coords].walls &= ~e.direction;
        cells[neighbor_coords].walls &= ~e.opposite_direction;

        // merge sets
        mergeSets(sets, current_set, neighbor_set);

      }

    }

    top--;
  }


  return cells;

}

