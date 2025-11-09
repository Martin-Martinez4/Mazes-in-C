#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "backtracking.h"
#include "cell.h"
#include "draw_cells.h"
#include "kruskals.h"
#include "prims.h"
#include "maze_stats.h"

const int WINDOW_WIDTH  = 1080;
const int WINDOW_HEIGHT = 720;

const int CELL_HEIGHT  = 10;
const int CELL_WIDTH   = 10;
const int BORDER_WIDTH = 2;

int main(int argc, char* argv[]) {
  // Seed the random number generator
  srand((unsigned int) time(NULL));

  int r  = rand();      // random number
  int r2 = rand() % 10; // random number from 0 to 9

  printf("RUNNING\n");

  SDL_Window* window; // Declare a pointer
  bool done = false;

  SDL_Init(SDL_INIT_VIDEO); // Initialize SDL3

  // Create an application window with the following settings:
  window = SDL_CreateWindow("An SDL3 window", // window title
                            WINDOW_WIDTH,     // width, in pixels
                            WINDOW_HEIGHT,    // height, in pixels
                            SDL_WINDOW_OPENGL // flags - see below
  );

  // Check that the window was successfully created
  if (window == NULL) {
    // In the case that the window could not be made...
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
  if (renderer == NULL) {
    printf("Renderer could not be created! SDL_Error: ");
    return 1;
  }

  MazeStats mazeStats = createMazeStats((int) (WINDOW_WIDTH), (int) (WINDOW_HEIGHT), CELL_HEIGHT,
                                        CELL_WIDTH, BORDER_WIDTH);

  Rooms* rooms = makeRooms(&mazeStats, 0.9);

  //   Cell* cells = kruskalsCreateMaze(&mazeStats, rooms);
  Cell* cells = backtrackingCreateMaze(&mazeStats, rooms);
  // Cell* cells = prims_create_maze(&mazeStats, rooms);

  // only holds the borders
  //   size_t rectsSize = ((3 * mazeStats.rows * mazeStats.columns +
  //                        (mazeStats.rows + mazeStats.columns)) -
  //                       (mazeStats.rows * mazeStats.columns)) *
  //                      sizeof(SDL_FRect);

  size_t rectsSize = 4 * mazeStats.rows * mazeStats.columns * sizeof(SDL_FRect);

  SDL_FRect* rects = (SDL_FRect*) malloc(rectsSize);
  int lenRects     = rectsSize / sizeof(SDL_FRect);

  // printf("size is: %d\n", (int)lenRects);
  // printf("rows is: %d\n", mazeStats.rows);
  // printf("columns is: %d\n", mazeStats.columns);

  //    int cellsToDraw =  rectsFromStats(rects, lenRects, mazeStats);
  int cellsToDraw = rectsFromCells(cells, rects, lenRects, mazeStats);

  SDL_FRect background = {.x = 0, .y = 0, .h = mazeStats.canvasHeight, .w = mazeStats.canvasWidth};

  while (!done) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        done = true;
      }
    }

    // Do game logic, present a frame, etc.

    // fill with black background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //  SDL_SetRenderDrawColor(renderer, 250, 249, 246, 255);
    SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
    int status = SDL_RenderFillRect(renderer, &background);

    // SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    // SDL_SetRenderDrawColor(renderer, 125, 120, 120, 255);
    SDL_SetRenderDrawColor(renderer, 186, 167, 136, 255);

    SDL_RenderFillRects(renderer, rects, cellsToDraw);

    SDL_RenderPresent(renderer);
  }

  // Close and destroy the window
  SDL_DestroyWindow(window);

  // Clean up
  SDL_Quit();
  return 0;
}