#include "load.h"
#include "stdio.h"
#include "cell.h"

Cell* loadMaze(MazeStats* mazeStats, int* cellsCreated, char* filePath) {
  // open file using filepath read
  FILE* file;
  int character;

  file = fopen(filePath, "r");
  if (file == NULL) {
    perror("Could not open file for reading");
    return NULL;
  }

  // iterate over first 6 characters
  // make sure magic number is there
  int l;
  int magicNum[4] = {109, 97, 122, 101};
  for (uint8_t i = 0; i < 4; i++) {
    l = fgetc(file);
    if (l != magicNum[i]) {
      perror("File type check failed");
      return NULL;
    }
  }

  // read rows / columns
  int columns = fgetc(file);
  if (columns == EOF) {
    perror("File type check failed");
    return NULL;
  }
  int rows = fgetc(file);
  if (rows == EOF) {
    perror("File type check failed");
    return NULL;
  }

  // will have to be changed of maybe?
  // Change mazeStats rows and columns
  mazeStats->rows    = rows;
  mazeStats->columns = columns;

  // create cells
  Cell* cells = malloc(sizeof(Cell) * rows * columns);

  // write data from file
  size_t i = 0;
  while (l = fgetc(file) != EOF) {
    // need to handle 
    // r or c are out of bounds
    
    int r;
    int c;
    array_coords_to_matrix_coords(0, columns, &r, &c);
    Cell cell = {.column = c, .row = r, .walls = l};

    cells[i] = cell;
  }

  return cells;
}
