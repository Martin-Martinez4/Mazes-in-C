#include "draw_cells.h"
#include "cell.h"
#include "grid_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL3/SDL_pixels.h>

// pass by value is good enough for now
MazeStats* createMazeStats(int canvasWidth, int canvasHeight, int cellWidth, int cellHeight,
                           int borderWidth, int sides) {
  // typedef struct MazeStats{
  //     int canvasWidth;
  //     int canvasHeight;
  //     int totalCellWidth;
  //     int totalCellHeight;
  //     int rows;
  //     int columns;

  // } MazeStats;

  int totalCellHeight;
  int totalCellWidth;

  MazeStats* m = malloc(sizeof(MazeStats));

  m->canvasWidth  = canvasWidth;
  m->canvasHeight = canvasHeight;

  switch (sides) {
  case 4:
    totalCellHeight = cellHeight + borderWidth;
    totalCellWidth  = cellWidth + borderWidth;
    m->rows         = (int) ((canvasHeight - borderWidth) / totalCellHeight);
    m->columns      = (int) ((canvasWidth - borderWidth) / totalCellWidth);
    break;

  case 3:
    totalCellHeight = cellHeight + borderWidth;
    totalCellWidth  = cellWidth + borderWidth;
    m->rows         = (int) ((canvasHeight - borderWidth) / totalCellHeight);
    m->columns      = (int) ((canvasWidth - borderWidth) / totalCellWidth) * 2;
    break;

  default:
    printf("Invalid number of sides: %d", sides);
    // add better error handling later
    exit(0);
    break;
  }
  m->num_edges       = sides;
  m->totalCellHeight = totalCellHeight;
  m->totalCellWidth  = totalCellWidth;
  m->borderWidth     = borderWidth;

  return m;
}

int rectsFromCells(Cell* cells, SDL_FRect* rects, size_t length, MazeStats* mazeStats) {
  int borderWidth     = mazeStats->borderWidth;
  int totalCellHeight = mazeStats->totalCellHeight;
  int totalCellWidth  = mazeStats->totalCellWidth;

  int rows    = mazeStats->rows;
  int columns = mazeStats->columns;

  int current = 0;

  for (int row = 0; row < rows; row++) {
    int offsetY = totalCellHeight * row;
    for (int col = 0; col < columns; col++) {
      int offsetX = totalCellWidth * col;
      Cell* c     = &cells[matrix_coords_to_array_coords(row, col, columns)];
      int walls   = c->walls;

      if (walls == ALL_WALLS) {
        rects[current++] =
            (SDL_FRect){.x = offsetX, .y = offsetY, .w = totalCellWidth, .h = totalCellHeight};
      } else {

        // LEFT
        if ((walls & LEFT) != 0 || col == 0) {
          if (current >= (int) length) {
            fprintf(stderr, "rects array overflow at LEFT, current=%d, max=%zu\n", current, length);
            exit(1);
          }
          rects[current++] =
              (SDL_FRect){.x = offsetX, .y = offsetY, .w = borderWidth, .h = totalCellHeight};
        }

        // TOP
        if ((walls & TOP) != 0 || row == 0) {
          if (current >= (int) length) {
            fprintf(stderr, "rects array overflow at TOP, current=%d, max=%zu\n", current, length);
            exit(1);
          }
          rects[current++] =
              (SDL_FRect){.x = offsetX, .y = offsetY, .w = totalCellWidth, .h = borderWidth};
        }

        // RIGHT
        if ((walls & RIGHT) != 0 || col == columns - 1) {
          if (current >= (int) length) {
            fprintf(stderr, "rects array overflow at RIGHT, current=%d, max=%zu\n", current,
                    length);
            exit(1);
          }
          rects[current++] = (SDL_FRect){
              .x = offsetX + totalCellWidth, .y = offsetY, .w = borderWidth, .h = totalCellHeight};
        }

        // BOTTOM
        if ((walls & BOTTOM) != 0 || row == rows - 1) {
          if (current >= (int) length) {
            fprintf(stderr, "rects array overflow at BOTTOM, current=%d, max=%zu\n", current,
                    length);
            exit(1);
          }
          rects[current++] = (SDL_FRect){
              .x = offsetX, .y = offsetY + totalCellHeight, .w = totalCellWidth, .h = borderWidth};
        }
      }
    }
  }

  return current;
}

int rectsFromStats(SDL_FRect* rects, size_t length, MazeStats mazeStats) {
  int borderWidth     = mazeStats.borderWidth;
  int totalCellHeight = mazeStats.totalCellHeight;
  int totalCellWidth  = mazeStats.totalCellWidth;

  int verticalBorderHeight = totalCellHeight;

  int rows    = mazeStats.rows;
  int columns = mazeStats.columns;

  // printf("Rows: %d\n", rows);
  // printf("Cols: %d\n", columns);
  // mazeStats  = createMazeStats(100, 100, 8, 8, 2);

  int current = 0;

  int offsetX = 0;
  int offsetY = 0;
  for (int row = 0; row < rows; row++) {
    offsetY = (totalCellHeight * row);
    for (int col = 0; col < columns; col++) {
      int randomNumber = rand() % 10 + 1;

      offsetX = (totalCellWidth * col);

      if (randomNumber > 2 || col == 0) {

        SDL_FRect leftBorder = {
            .x = offsetX, .y = offsetY, .h = verticalBorderHeight, .w = borderWidth};

        rects[current] = leftBorder;
        current++;
      }

      randomNumber = rand() % 10 + 1;
      if (randomNumber > 3 || row == 0) {

        SDL_FRect topBorder = {.x = offsetX, .y = offsetY, .h = borderWidth, .w = totalCellWidth};

        rects[current] = topBorder;
        current++;
      }

      if (col == columns - 1) {
        SDL_FRect rightBorder = {.x = offsetX + totalCellWidth,
                                 .y = offsetY,
                                 .h = verticalBorderHeight,
                                 .w = borderWidth};

        rects[current] = rightBorder;
        current++;
      }
      if (row == rows - 1) {
        SDL_FRect bottomBorder = {.x = offsetX,
                                  .y = offsetY + totalCellHeight,
                                  .h = borderWidth,
                                  .w = totalCellWidth + borderWidth};

        rects[current] = bottomBorder;
        current++;
      }
    }
  }

  return current;
}

int draw_quad(SDL_Vertex* quads, int starting_index, float x_from, float y_from, float x_to,
              float y_to, int border_thickness) {
  // Edge vector: bottom-left -> top
  float dx = x_to - x_from;
  float dy = y_to - y_from;

  float len = sqrtf(dx * dx + dy * dy);
  if (len == 0.0f) {

    return starting_index;
  }

  float half_t = border_thickness * 0.5f;
  // Perpendicular offset
  float px = -dy / len * half_t;
  float py = dx / len * half_t;

  // Quad for left wall
  quads[starting_index].position     = (SDL_FPoint){x_from + px, y_from + py};
  quads[starting_index + 2].position = (SDL_FPoint){x_to + px, y_to + py};
  quads[starting_index + 1].position = (SDL_FPoint){x_from - px, y_from - py};
  quads[starting_index + 3].position = (SDL_FPoint){x_to - px, y_to - py};

  return starting_index + 4;
}

void trisFromStats(Cell* cells, MazeStats* mazeStats, SDL_Renderer* renderer) {
  int rows           = mazeStats->rows;
  int columns        = mazeStats->columns;
  float cell_width   = mazeStats->totalCellWidth;
  float cell_height  = mazeStats->totalCellHeight;
  float border_width = mazeStats->borderWidth;

  SDL_Vertex* quads = (SDL_Vertex*) malloc(sizeof(SDL_Vertex) * rows * columns * 3 * 4);
  int* indices      = (int*) malloc(sizeof(int) * rows * columns * 3 * 6);

  int current_index = 0;

  Cell* current_cell;

  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < columns; c++) {

      current_cell = &cells[matrix_coords_to_array_coords(r, c, columns)];

      float x0 = c * (cell_width * 0.5f);
      float y0 = cell_height * r;

      if (IS_DOWN(r, c)) {
        // Down triangle
        float xl = x0;
        float yl = y0;

        float xr = x0 + cell_width;
        float yr = y0;

        float xb = x0 + cell_width * 0.5f;
        float yb = y0 + cell_height;

        if (current_cell->walls & TRI_BASE) {
          current_index = draw_quad(quads, current_index, xl, yl, xr, yr, border_width);
        }
        if (current_cell->walls & TRI_LEFT) {

          current_index = draw_quad(quads, current_index, xl, yl, xb, yb, border_width);
        }
        if (current_cell->walls & TRI_RIGHT) {

          current_index = draw_quad(quads, current_index, xr, yr, xb, yb, border_width);
        }

      } else {
        // Up triangle
        float xl = x0;
        float yl = y0 + cell_height;

        float xr = x0 + cell_width;
        float yr = y0 + cell_height;

        float xt = x0 + cell_width * 0.5f;
        float yt = y0;

        if (current_cell->walls & TRI_BASE) {
          current_index = draw_quad(quads, current_index, xl, yl, xr, yr, border_width);
        }
        if (current_cell->walls & TRI_LEFT) {

          current_index = draw_quad(quads, current_index, xl, yl, xt, yt, border_width);
        }
        if (current_cell->walls & TRI_RIGHT) {
          current_index = draw_quad(quads, current_index, xr, yr, xt, yt, border_width);
        }
      }
    }
  }

  SDL_FColor color = {186 / 255.0f, 167 / 255.0f, 136 / 255.0f, 1.0f};

  for (int i = 0; i < current_index; i++) {
    quads[i].color = color;
  }

  int quad_count = current_index / 4;

  for (int q = 0; q < quad_count; q++) {
    int v = q * 4;
    int i = q * 6;

    indices[i + 0] = v + 0;
    indices[i + 1] = v + 1;
    indices[i + 2] = v + 2;

    indices[i + 3] = v + 2;
    indices[i + 4] = v + 1;
    indices[i + 5] = v + 3;
  }

  SDL_RenderGeometry(renderer, NULL, quads, current_index, indices, quad_count * 6);
  free(indices);
  free(quads);
}
