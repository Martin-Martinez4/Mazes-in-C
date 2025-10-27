#ifndef H_DRAW_CELLS
#define H_DRAW_CELLS

#include "cell.h"
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

MazeStats createMazeStats(int canvasWidth, int canvasHeight, int cellWidth, int cellHeight, int borderWidth);
int rectsFromStats(SDL_FRect *rects, size_t length, MazeStats mazeStats);
int rectsFromCells(Cell* cells, SDL_FRect *rects, size_t length, MazeStats mazeStats);

#endif