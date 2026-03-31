#include "create_maze.h"
#include "rooms.h"
#include "stdio.h"
#include "stdlib.h"
#include "backtracking.h"
#include "draw_cells.h"
#include "kruskals.h"
#include "prims.h"
#include "grid_utils.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include <SDL3/SDL.h>
#include "hybrid.h"

Cell* createCells(MazeStats* mazeStats, float* noise_grid, MazeGenAlgorithm algo, double roomSaturation, int prune_aggressiveness) {
  printf("size is Maze Stats Created\n");
  Rooms* rooms = makeRooms(mazeStats, roomSaturation);
  printf("size is rooms Created\n");

  AlgoStepFunc algos[1] = {backtrack_region};

  Cell* cells;
  switch (algo) {
  case KRUSKALS:
    algos[0] = kruskals_region;
    break;
  case PRIMS:
    algos[0] = prim_region;
    break;
  case BACKTRACKING:
    algos[0] = backtrack_region;
    break;
  default:
    return NULL;
  }
  cells = create_maze_hybrid(mazeStats, noise_grid, roomSaturation, algos, 1, prune_aggressiveness);

  free(rooms->data);
  free(rooms);
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

void updateNoiseTexture(SDL_Texture* texture, float* noiseGrid, int columns, int rows) {
  void* pixels;
  int pitch;

  SDL_LockTexture(texture, NULL, &pixels, &pitch);
  uint32_t* buffer = (uint32_t*) pixels;

  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < columns; x++) {
      float v = noiseGrid[y * columns + x];
      // SDL_Log("v: %f\n", v);
      if (v < 0) {
        v = 0;
      }
      if (v > 1) {
        v = 1;
      }

      Uint8 gray = (Uint8) (v * 255);

      buffer[y * (pitch / 4) + x] = (255 << 24) | (gray << 16) | (gray << 8) | gray;
    }
  }

  SDL_UnlockTexture(texture);
}