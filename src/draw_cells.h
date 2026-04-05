#ifndef H_DRAW_CELLS
#define H_DRAW_CELLS

#include "cell.h"
#include "maze_stats.h"
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

MazeStats* createMazeStats(int canvasWidth, int canvasHeight, int cellWidth, int cellHeight,
                           int borderWidth, int sides) ;
int rectsFromStats(SDL_FRect* rects, size_t length, MazeStats mazeStats);
int rectsFromCells(Cell* cells, SDL_FRect* rects, size_t length, MazeStats* mazeStats);
int draw_quad(SDL_Vertex* quads, int starting_index, float x_from, float y_from, float x_to,
              float y_to, int border_thickness);
void trisFromStats(Cell* cells, MazeStats* mazeStats, SDL_Renderer* renderer);
#endif