#include "export.h"
#include "stdlib.h"
#include "stddef.h"
#include "stdio.h"

int exportMaze(MazeStats* mazeStats, Cell* cells, char* filePath) {

  FILE* file = fopen(filePath, "wb");
  if (!file) {
    perror("Failed to open file");
    return 1;
  }

  size_t len = mazeStats->rows * mazeStats->columns;
  uint8_t* c     = malloc(len * sizeof(uint8_t));

  int header[4] = {'m', 'a', 'z', 'e'};
  fwrite(header, sizeof(int), 4, file);
  fwrite(&mazeStats->columns, sizeof(int), 1, file);
  fwrite(&mazeStats->rows, sizeof(int), 1, file);

  for (size_t i = 0; i < len; i++) {
    c[i] = cells[i].walls;
  }

  size_t written = fwrite(c, sizeof(uint8_t), len, file);
  if (written != len) {
    fclose(file);
    free(c);
    perror("Failed to write all data");
    return 1;
  }

  fclose(file);
  free(c);
  return 0;
}