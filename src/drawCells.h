#ifndef H_DRAW_CELLS
#define H_DRAW_CELLS

#include <SDL3/SDL_rect.h>

typedef struct MazeStats{
  int canvasWidth;
  int canvasHeight;
  int totalCellWidth;
  int totalCellHeight;
  int rows;
  int columns;
  int borderWidth;

} MazeStats;

int coordsMatrixToArray(int x, int y, int columns);
MazeStats createMazeStats(int canvasWidth, int canvasHeight, int cellWidth, int cellHeight, int borderWidth);
int buildCellsArray(SDL_FRect *rects, size_t length, MazeStats mazeStats);

#endif