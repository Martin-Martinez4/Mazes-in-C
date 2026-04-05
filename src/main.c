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
#include "backtracking.h"
#include "kruskals.h"
#include "rooms.h"
#include "hybrid.h"
#include "export.h"
#include "load.h"
#include "noise.h"

#include "config_nk.h"
#include "init_font.h"
#include "init_nk.h"
#include "nuklear.h"
#include "nuklear_sdl3_renderer.h"
#include "layout_nk.h"
#include "init_font.h"


const int WINDOW_WIDTH  = 1080;
const int WINDOW_HEIGHT = 900;
// const int WINDOW_HEIGHT = 1080;

const int CELL_HEIGHT  = 16;
const int CELL_WIDTH   = 16;
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

  FontSystem* font_system = init_SDLFont(renderer);
  if (!font_system) {
    return 1;
  }

  struct nk_context* ctx = initNk(window, renderer);

  // Initial Maze Creation
  MazeStats* mazeStats = createMazeStats((int) (WINDOW_WIDTH), (int) (WINDOW_HEIGHT), CELL_HEIGHT,
                                         CELL_WIDTH, BORDER_WIDTH);

  AlgoStepFunc algos[5] = {prim_region, backtrack_region, prim_region};
  int algo_array_size   = 3;

  RadialGradientParams gp =
      create_radial_gradient_params(mazeStats->rows, mazeStats->columns, 0, 0);
  float* noiseGrid =
      applyNoise(mazeStats->rows, mazeStats->columns, &state.scale, radial_gradient, &gp);

  SDL_Texture* texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                        mazeStats->columns, mazeStats->rows);
  updateNoiseTexture(texture, noiseGrid, mazeStats->columns, mazeStats->rows);

  Cell* cells = create_maze_hybrid(mazeStats, noiseGrid, state.room_saturation, algos,
                                   algo_array_size, state.prune_aggressiveness);
  int count   = BFS_count(cells, mazeStats->rows, mazeStats->columns);

  SDL_Log("\ncount: %d; want: %d\n", count, mazeStats->rows * mazeStats->columns);

  int cellsToDraw;
  SDL_FRect* rects = createSDLRects(mazeStats, cells, &cellsToDraw);

  int width, height;

  SDL_FRect dst = {0, 0, 800, 800};

  while (!done) {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            done = true;
        }
    }


    // nk_input_begin(ctx);

    // while (SDL_PollEvent(&event)) {
    //   if (event.type == SDL_EVENT_QUIT) {

    //     done = true;
    //   }
    //   nk_sdl_handle_event(ctx, &event);
    //   // nk_input_char(ctx, 'A');

    //   switch (event.type) {
    //   case SDL_EVENT_KEY_DOWN:
    //     if (event.key.scancode == SDL_SCANCODE_0) {
    //       printf("columns: %d; rows: %d", mazeStats->columns, mazeStats->rows);
    //       cells = loadMaze(mazeStats, &cellsToDraw, "./maze4.maze");
    //       rects = createSDLRects(mazeStats, cells, &cellsToDraw);
    //     } else if (event.key.scancode == SDL_SCANCODE_M) {
    //       state.menuVisible = !state.menuVisible;
    //     }
    //   }
    // }

    // nk_input_end(ctx);

    // SDL_GetWindowSize(window, &width, &height);

    // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // SDL_RenderClear(renderer);

    // renderNk(ctx);

    // if (state.generate_noise) {

    //   free(noiseGrid);

    //   switch (state.noise_selected) {
    //   case VALUE:
    //     noiseGrid =
    //         applyNoise(mazeStats->rows, mazeStats->columns, &state.scale, bilerpFromRowCol, NULL);
    //     break;
    //   case PERLIN:
    //     noiseGrid =
    //         applyNoise(mazeStats->rows, mazeStats->columns, &state.scale, perlinBilerp, NULL);
    //     break;
    //   case SIMPLEX:
    //     noiseGrid =
    //         applyNoise(mazeStats->rows, mazeStats->columns, &state.scale, simplexBilerp, NULL);
    //     break;
    //   case LINEAR_GRADIENT:

    //     LinearGradientParams gp =
    //         create_linear_gradient_params(mazeStats->rows, mazeStats->columns, state.degrees);
    //     SDL_Log("Degrees: %f", state.degrees);

    //     noiseGrid =
    //         applyNoise(mazeStats->rows, mazeStats->columns, &state.scale, linear_gradient, &gp);
    //     break;
    //   case RADIAL_GRADIENT:
    //     RadialGradientParams rp = create_radial_gradient_params(
    //         mazeStats->rows, mazeStats->columns, (int) (state.cx / 100.0f * mazeStats->columns),
    //         (int) (state.cy / 100.0f * mazeStats->rows));

    //     noiseGrid =
    //         applyNoise(mazeStats->rows, mazeStats->columns, &state.scale, radial_gradient, &rp);
    //     break;

    //   default:
    //     break;
    //   }

    //   updateNoiseTexture(texture, noiseGrid, mazeStats->columns, mazeStats->rows);

    //   state.generate_noise = false;
    // }

    // if (state.redrawMaze) {

    //   free(cells);
    //   free(rects);
    //   free(noiseGrid);
    //   free(mazeStats);

    //   mazeStats = createMazeStats((int) (WINDOW_WIDTH), (int) (WINDOW_HEIGHT), state.cell_height,
    //                               state.cell_width, state.border_thickness);

    //   texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
    //                               mazeStats->columns, mazeStats->rows);

    //   switch (state.noise_selected) {
    //   case VALUE:
    //     noiseGrid =
    //         applyNoise(mazeStats->rows, mazeStats->columns, &state.scale, bilerpFromRowCol, NULL);
    //     break;
    //   case PERLIN:
    //     noiseGrid =
    //         applyNoise(mazeStats->rows, mazeStats->columns, &state.scale, perlinBilerp, NULL);
    //     break;
    //   case SIMPLEX:
    //     noiseGrid =
    //         applyNoise(mazeStats->rows, mazeStats->columns, &state.scale, simplexBilerp, NULL);
    //     break;
    //   case LINEAR_GRADIENT:
    //     LinearGradientParams* gp = malloc(sizeof(*gp));
    //     *gp = create_linear_gradient_params(mazeStats->rows, mazeStats->columns, state.degrees);

    //     noiseGrid =
    //         applyNoise(mazeStats->rows, mazeStats->columns, &state.scale, linear_gradient, &gp);
    //     free(gp);
    //     break;
    //   case RADIAL_GRADIENT:
    //     RadialGradientParams rp = create_radial_gradient_params(
    //         mazeStats->rows, mazeStats->columns, (int) (state.cx / 100.0f * mazeStats->columns),
    //         (int) (state.cy / 100.0f * mazeStats->rows));

    //     noiseGrid =
    //         applyNoise(mazeStats->rows, mazeStats->columns, &state.scale, radial_gradient, &rp);
    //     break;

    //   default:
    //     break;
    //   }

    //   updateNoiseTexture(texture, noiseGrid, mazeStats->columns, mazeStats->rows);

    //   for (int i = 0; i < state.num_algos; i++) {

    //     switch (state.hybrid_options[i]) {
    //     case BACKTRACKING:
    //       algos[i] = backtrack_region;
    //       break;
    //     case PRIMS:
    //       algos[i] = prim_region;
    //       break;
    //     case KRUSKALS:
    //       algos[i] = kruskals_region;
    //       break;
    //     default:
    //       SDL_Log("Illegal Hybrid Algoirthm Option");
    //       exit(1);
    //     }
    //   }
    //   SDL_Log("SDL_Log num_algos: %d\n", state.num_algos);
    //   cells            = create_maze_hybrid(mazeStats, noiseGrid, state.room_saturation, algos,
    //                                         state.num_algos, state.prune_aggressiveness);
    //   rects            = createSDLRects(mazeStats, cells, &cellsToDraw);
    //   state.redrawMaze = false;
    // }

    // if (!state.mCheck) {

    //   // Draw Maze Walls
    //   SDL_SetRenderDrawColor(renderer, 186, 167, 136, 255);
    //   SDL_RenderFillRects(renderer, rects, cellsToDraw);

    // } else {

    //   SDL_RenderTexture(renderer, texture, NULL, &dst);
    // }
    // nk_sdl_render(ctx, NK_ANTI_ALIASING_ON);
    // if (state.export) {
    //   printf("Exporting: %s\n", state.fileName);
    //   exportMaze(mazeStats, cells, state.fileName);
    //   state.export = false;
    // }
    // if (state.upload) {
    //   printf("Loading: %s\n", state.uploadFileName);
    //   free(cells);
    //   free(rects);

    //   Cell* newCells = loadMaze(mazeStats, &cellsToDraw, state.uploadFileName);
    //   if (newCells) {
    //     free(cells);
    //     free(rects);

    //     cells = newCells;
    //     rects = createSDLRects(mazeStats, cells, &cellsToDraw);
    //   }

    //   state.upload = false;
    // }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    trisFromStats(mazeStats, renderer);

    SDL_RenderPresent(renderer);
  }

  // Clean up, Close and destroy the window
  // free(cells);
  // free(rects);
  // free(noiseGrid);
  // free(mazeStats);

  // for (int i = 0; i < 1; i++) {
  //   if (font_system->fonts[i])
  //     TTF_CloseFont(font_system->fonts[i]);
  // }
  // SDL_free(font_system->fonts);

  // TTF_DestroyRendererTextEngine(font_system->font_engine);

  // nk_sdl_shutdown(ctx);

  TTF_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}