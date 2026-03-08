#include "create_maze.h"
#include "rooms.h"
#include "stdio.h"
#include "stdlib.h"
#include "backtracking.h"
#include "draw_cells.h"
#include "kruskals.h"
#include "prims.h"

Cell* createCells(MazeStats* mazeStats, MazeGenAlgorithm algo, double roomSaturation) {
  printf("size is Maze Stats Created");
  Rooms* rooms = makeRooms(mazeStats, roomSaturation);
  printf("size is rooms Created");

  Cell* cells;
  switch (algo) {
  case KRUSKALS:
    cells = kruskalsCreateMaze(mazeStats, rooms);
    break;
  case PRIMS:
    cells = prims_create_maze(mazeStats, rooms);
    break;
  case BACKTRACKING:
    cells = backtrackingCreateMaze(mazeStats, rooms);
    break;
  default:
    cells = NULL;
  }

  return cells;
}

SDL_FRect* createSDLRects(MazeStats* mazeStats, Cell* cells, int* cellsCreated) {

  if (cells == NULL) {
    fprintf(stderr, "Something went wrong: Tried to create SDL rectangles from NULL");
    exit(-1);
  }

  size_t rectsSize = 4 * mazeStats->rows * mazeStats->columns * sizeof(SDL_FRect);

  SDL_FRect* rects = (SDL_FRect*) malloc(rectsSize);
  int lenRects     = rectsSize / sizeof(SDL_FRect);

  printf("size is: %d\n", (int) lenRects);
  printf("rows is: %d\n", mazeStats->rows);
  printf("columns is: %d\n", mazeStats->columns);

  *cellsCreated = rectsFromCells(cells, rects, lenRects, mazeStats);

  return rects;
}