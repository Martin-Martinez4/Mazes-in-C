#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <grid_utils.h>
#include <time.h>

#include "cell.h"
#include "draw_cells.h"
#include "maze_stats.h"
#include "create_maze.h"
#include "hybrid.h"
#include "export.h"
#include "load.h"
#include "noise.h"

#include "config_nk.h"

#include "init_nk.h"
#include "nuklear.h"
#include "nuklear_sdl3_renderer.h"
#include "layout_nk.h"
#include "init_font.h"

const int WINDOW_WIDTH  = 1080;
const int WINDOW_HEIGHT = 800;
// const int WINDOW_HEIGHT = 1080;

const int CELL_HEIGHT  = 8;
const int CELL_WIDTH   = 8;
const int BORDER_WIDTH = 1;

int main(int argc, char* argv[]) {
  // Seed the random number generator
  srand((unsigned int) time(NULL));

  // random number
  // int r  = rand();
  // // random number from 0 to 9
  // int r2 = rand() % 10;

  printf("RUNNING\n");

  SDL_Window* window;
  bool done     = false;
  bool seeNoise = false;

  // Initialize SDL3
  SDL_Init(SDL_INIT_VIDEO);

  // Create an application window with the following settings:
  window = SDL_CreateWindow("An SDL3 window",                        // window title
                            WINDOW_WIDTH,                            // width, in pixels
                            WINDOW_HEIGHT,                           // height, in pixels
                            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE // flags - see below
  );
  SDL_StartTextInput(window);

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

  init_SDLFont(renderer);

  struct nk_context* ctx = initNk(window, renderer);

  // Initial Maze Creation
  MazeStats* mazeStats = createMazeStats((int) (WINDOW_WIDTH), (int) (WINDOW_HEIGHT), CELL_HEIGHT,
                                         CELL_WIDTH, BORDER_WIDTH);

  // Cell* cells = createCells(mazeStats, state.algoSelected, 0.0f);
  AlgoStepFunc algos[] = { backtrack_region, prim_step, kruskals_region };
  int algo_array_size = 3;


  Cell* cells = create_maze_hybrid(mazeStats, 0.0f, algos, algo_array_size);
  int count = BFS_count(cells, mazeStats->rows, mazeStats->columns);
  printf("count: %d; want: %d\n", count, mazeStats->rows * mazeStats->columns);

  float scale      = 0.0756f;
  float* noiseGrid = applyNoise(mazeStats->rows, mazeStats->columns, &scale, perlinBilerp, NULL);

  SDL_Texture* texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                        mazeStats->columns, mazeStats->rows);
  updateNoiseTexture(texture, noiseGrid, mazeStats->columns, mazeStats->rows);

  int cellsToDraw;
  SDL_FRect* rects = createSDLRects(mazeStats, cells, &cellsToDraw);

  int width, height;

  SDL_FRect dst = {0, 0, 800, 800};

  while (!done) {

    SDL_Event event;

    nk_input_begin(ctx);

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {

        done = true;
      }
      nk_sdl_handle_event(ctx, &event);
      // nk_input_char(ctx, 'A');

      switch (event.type) {
      case SDL_EVENT_KEY_DOWN:
        if (event.key.scancode == SDL_SCANCODE_0) {
          printf("columns: %d; rows: %d", mazeStats->columns, mazeStats->rows);
          cells = loadMaze(mazeStats, &cellsToDraw, "./maze4.maze");
          rects = createSDLRects(mazeStats, cells, &cellsToDraw);
        } else if (event.key.scancode == SDL_SCANCODE_M) {
          state.menuVisible = !state.menuVisible;
        }
      }
    }

    nk_input_end(ctx);

    SDL_GetWindowSize(window, &width, &height);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderNk(ctx);

    if (state.redrawMaze) {
      cells            = createCells(mazeStats, state.algoSelected, 0.25);
      rects            = createSDLRects(mazeStats, cells, &cellsToDraw);
      state.redrawMaze = false;
    }

    if (!state.mCheck) {

      // Draw Maze Walls
      SDL_SetRenderDrawColor(renderer, 186, 167, 136, 255);
      SDL_RenderFillRects(renderer, rects, cellsToDraw);

    } else {

      SDL_RenderTexture(renderer, texture, NULL, &dst);
    }
    nk_sdl_render(ctx, NK_ANTI_ALIASING_ON);
    if (state.export) {
      printf("Exporting: %s\n", state.fileName);
      exportMaze(mazeStats, cells, state.fileName);
      state.export = false;
    }
    if (state.upload) {
      printf("Loading: %s\n", state.uploadFileName);
      Cell* cTemp = loadMaze(mazeStats, &cellsToDraw, state.uploadFileName);
      if (!cTemp) {
        printf("Failed to load file: %s\n", state.uploadFileName);
      } else {
        cells = cTemp;
        rects = createSDLRects(mazeStats, cells, &cellsToDraw);
      }
      state.upload = false;
    }

    SDL_RenderPresent(renderer);
  }

  // Close and destroy the window
  nk_sdl_shutdown(ctx);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  // Clean up
  SDL_Quit();
  return 0;
}