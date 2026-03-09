#include "export.h"
#include "stddef.h"
#include "stdio.h"

int exportMaze(MazeStats* mazeStats, Cell* cells, char* filePath) {
  size_t len = mazeStats->rows * mazeStats->columns;
  int c[len + 6];
  // m
  c[0] = 109;
  // a
  c[1] = 97;
  // z
  c[2] = 122;
  // e
  c[3] = 101;
  // rows
  c[4] = mazeStats->rows;
  // columns
  c[5] = mazeStats->columns;

  for (size_t i = 0; i < len; i++) {
    c[i + 6] = cells[i].walls;
  }

  //   for (size_t i = 0; i < len; i++) {
  //     printf("c: %d\n", c[i]);
  //   }

  FILE* file = fopen(filePath, "wb");
  if (!file) {
    perror("Failed to open file");
    return 1;
  }

  size_t written = fwrite(c, sizeof(int), len + 6, file);
  if (written != len+6) {
    perror("Failed to write all data");
  }

  fclose(file);
  return 0;
}