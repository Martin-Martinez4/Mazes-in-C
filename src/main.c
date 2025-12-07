#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
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

#define CLAY_IMPLEMENTATION

#include "clay.h"

#include "./ui/clay_renderer_SDL3.c"

void MyClayErrorHandler(Clay_ErrorData errorData) {
  const char* typeName = "Unknown";

  switch (errorData.errorType) {
  case CLAY_ERROR_TYPE_TEXT_MEASUREMENT_FUNCTION_NOT_PROVIDED:
    typeName = "TEXT_MEASUREMENT_FUNCTION_NOT_PROVIDED";
    break;
  case CLAY_ERROR_TYPE_ARENA_CAPACITY_EXCEEDED:
    typeName = "ARENA_CAPACITY_EXCEEDED";
    break;
  case CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED:
    typeName = "ELEMENTS_CAPACITY_EXCEEDED";
    break;
  case CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED:
    typeName = "TEXT_MEASUREMENT_CAPACITY_EXCEEDED";
    break;
  case CLAY_ERROR_TYPE_DUPLICATE_ID:
    typeName = "DUPLICATE_ID";
    break;
  case CLAY_ERROR_TYPE_FLOATING_CONTAINER_PARENT_NOT_FOUND:
    typeName = "FLOATING_CONTAINER_PARENT_NOT_FOUND";
    break;
  case CLAY_ERROR_TYPE_PERCENTAGE_OVER_1:
    typeName = "PERCENTAGE_OVER_1";
    break;
  case CLAY_ERROR_TYPE_INTERNAL_ERROR:
    typeName = "INTERNAL_ERROR";
    break;
  default:
    break;
  }
}

const int WINDOW_WIDTH  = 1080;
const int WINDOW_HEIGHT = 800;

const int CELL_HEIGHT  = 12;
const int CELL_WIDTH   = 12;
const int BORDER_WIDTH = 1;

int main(int argc, char* argv[]) {
  // Seed the random number generator
  srand((unsigned int) time(NULL));

  int r  = rand();      // random number
  int r2 = rand() % 10; // random number from 0 to 9

  printf("RUNNING\n");

  SDL_Window* window;
  bool done = false;

  // Initialize SDL3
  SDL_Init(SDL_INIT_VIDEO);

  if (!TTF_Init()) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_Init failed: %s", SDL_GetError());
    return -1;
  }

  // Create an application window with the following settings:
  window = SDL_CreateWindow("An SDL3 window",                        // window title
                            WINDOW_WIDTH,                            // width, in pixels
                            WINDOW_HEIGHT,                           // height, in pixels
                            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE // flags - see below
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

  TTF_TextEngine* textEngine = TTF_CreateRendererTextEngine(renderer);
  if (!textEngine) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create text engine: %s", SDL_GetError());
    return -1;
  }

  // load font
  TTF_Font* font = TTF_OpenFont("resources/Roboto-Regular.ttf", 24);
  if (!font) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font: %s", SDL_GetError());
    return -1;
  }

  TTF_Font** fonts = SDL_calloc(1, sizeof(TTF_Font*));
  if (!fonts) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Out of memory!");
    return -1;
  }

  // Load a font and assign it to the first slot
  fonts[0] = TTF_OpenFont("resources/Roboto-Regular.ttf", 24);
  if (!fonts[0]) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font: %s", SDL_GetError());
    return -1;
  }

  Clay_SDL3RendererData* clay_sdl3 = SDL_calloc(1, sizeof(*clay_sdl3));
  if (!clay_sdl3) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Out of memory");
    return 1;
  }

  clay_sdl3->renderer   = renderer;
  clay_sdl3->fonts      = fonts;
  clay_sdl3->textEngine = textEngine;

  MazeStats mazeStats = createMazeStats((int) (WINDOW_WIDTH), (int) (WINDOW_HEIGHT), CELL_HEIGHT,
                                        CELL_WIDTH, BORDER_WIDTH);
  printf("size is Maze Stats Created");
  Rooms* rooms = makeRooms(&mazeStats, 0.25);
  printf("size is rooms Created");

  // Cell* cells = kruskalsCreateMaze(&mazeStats, rooms);
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

  printf("size is: %d\n", (int) lenRects);
  printf("rows is: %d\n", mazeStats.rows);
  printf("columns is: %d\n", mazeStats.columns);

  //    int cellsToDraw =  rectsFromStats(rects, lenRects, mazeStats);
  int cellsToDraw = rectsFromCells(cells, rects, lenRects, mazeStats);

  SDL_FRect background = {.x = 0, .y = 0, .h = mazeStats.canvasHeight, .w = mazeStats.canvasWidth};

  uint64_t clayRequiredMemory = Clay_MinMemorySize();

  Clay_Arena clayMem =
      (Clay_Arena){.memory = malloc(clayRequiredMemory), .capacity = clayRequiredMemory};

  Clay_Initialize(
      clayMem, (Clay_Dimensions){.width = (float) WINDOW_WIDTH, .height = (float) WINDOW_HEIGHT},
      (Clay_ErrorHandler){.errorHandlerFunction = MyClayErrorHandler, .userData = NULL});

  int width, height;

  bool menuVisible = false;

  while (!done) {

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_QUIT:
        done = true;
        break;

      case SDL_EVENT_KEY_UP:
        if (event.key.scancode == SDL_SCANCODE_SPACE) {
          SDL_Log("Space pressed!");
          // Do something when space is released
        }
        break;

      case SDL_EVENT_WINDOW_RESIZED:
        Clay_SetLayoutDimensions(
            (Clay_Dimensions){(float) event.window.data1, (float) event.window.data2});
        break;

      case SDL_EVENT_MOUSE_MOTION:
        // Update Clay pointer position every frame
        Clay_Vector2 mousePosition = (Clay_Vector2){(float) event.motion.x, (float) event.motion.y};
        Clay_SetPointerState(mousePosition, false);
        break;

      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        // Update Clay pointer state with the new pressed state
        if (event.button.button == SDL_BUTTON_LEFT) {
          Clay_SetPointerState((Clay_Vector2){(float) event.button.x, (float) event.button.y},
                               true);

          // Check if pointer is over HeaderBar
          if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("HeaderBarToggle")))) {
            menuVisible = !menuVisible;
            SDL_Log("HeaderBar clicked! menuVisible = %s", menuVisible ? "true" : "false");
          }
        }
        break;

      case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button == SDL_BUTTON_LEFT) {
          Clay_SetPointerState((Clay_Vector2){(float) event.button.x, (float) event.button.y},
                               false);
        }
        break;

      case SDL_EVENT_MOUSE_WHEEL:
        // Optionally update scroll containers
        // Clay_UpdateScrollContainers(true, (Clay_Vector2){ event.wheel.x, event.wheel.y }, 0.01f);
        break;

      default:
        break;
      }
    }

    SDL_GetWindowSize(window, &width, &height);
    Clay_SetLayoutDimensions((Clay_Dimensions){.width = width, .height = height});
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

    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterContainer"), {.backgroundColor = {43, 41, 51, 0},
                                     .layout          = {.layoutDirection = CLAY_TOP_TO_BOTTOM,
                                                         .sizing          = layoutExpand,
                                                         .padding         = CLAY_PADDING_ALL(16),
                                                         .childGap        = 16}}) {

      if (!menuVisible) {
        CLAY(CLAY_ID("HeaderBarToggle"),
             {.backgroundColor = {90, 90, 90, 255},

              .layout = {
                  .layoutDirection = CLAY_TOP_TO_BOTTOM,
                  .sizing = {.width = CLAY_SIZING_FIXED(20), .height = CLAY_SIZING_FIXED(20)},

              }}) {
          CLAY_AUTO_ID({.layout = {.padding = {5, 0, 0, 0},
                                   .sizing  = {.width  = CLAY_SIZING_FIXED(20),
                                               .height = CLAY_SIZING_FIXED(20)}},

                        // .backgroundColor = { 0, 180, 0, 255 },

                        .cornerRadius = CLAY_CORNER_RADIUS(5)}) {
            CLAY_TEXT(CLAY_STRING("+"),
                      CLAY_TEXT_CONFIG({.fontId        = 0,
                                        .fontSize      = 20,
                                        .textColor     = {255, 255, 255, 255},
                                        .textAlignment = CLAY_TEXT_ALIGN_CENTER}));
          }
        }
      } else {

        CLAY(CLAY_ID("HeaderBar"),
             {.backgroundColor = {90, 90, 90, 255},

              .layout = {.padding = CLAY_PADDING_ALL(5),

                         .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(60)}

              }}) {
          CLAY(CLAY_ID("HeaderBarToggle"),
               {.backgroundColor = {180, 90, 90, 150},
                .layout          = {
                             .layoutDirection = CLAY_TOP_TO_BOTTOM,
                             .sizing = {.width = CLAY_SIZING_FIXED(20), .height = CLAY_SIZING_FIXED(20)},
                             .childAlignment = {.y = CLAY_ALIGN_Y_CENTER, .x = CLAY_ALIGN_X_CENTER},
                             .padding        = CLAY_PADDING_ALL(0)}}) {
            CLAY_AUTO_ID({.layout = {.padding = {5, 10, 0, 0},
                                     .sizing  = {.width  = CLAY_SIZING_FIXED(20),
                                                 .height = CLAY_SIZING_FIXED(20)}},

                          // .backgroundColor = { 0, 180, 0, 255 },

                          .cornerRadius = CLAY_CORNER_RADIUS(5)}) {
              CLAY_TEXT(CLAY_STRING("-"),
                        CLAY_TEXT_CONFIG({.fontId        = 0,
                                          .fontSize      = 20,
                                          .textColor     = {255, 255, 255, 255},
                                          .textAlignment = CLAY_TEXT_ALIGN_CENTER}));
            }
          }

          CLAY(CLAY_ID("opts"),
               {
                   .layout =
                       {

                           .layoutDirection = CLAY_TOP_TO_BOTTOM,
                           .childGap        = 16,
                           .padding         = {16, 16, 16, 16},
                           .sizing = {.width = CLAY_SIZING_FIT(), .height = CLAY_SIZING_FIT()}},
                   .backgroundColor = {200, 90, 200, 150},

               }) {

            CLAY(CLAY_ID("opt"),
                 {.backgroundColor = {180, 90, 180, 150},
                  .layout = {.sizing  = {.width = CLAY_SIZING_GROW(), .height = CLAY_SIZING_FIT()},
                             .padding = CLAY_PADDING_ALL(0)}}) {
              CLAY_AUTO_ID(
                  {.layout = {.padding = {5, 10, 0, 0},
                              .sizing  = {.width = CLAY_SIZING_FIT(), .height = CLAY_SIZING_FIT()}},

                   // .backgroundColor = { 0, 180, 0, 255 },

                   .cornerRadius = CLAY_CORNER_RADIUS(5)}) {
               CLAY_TEXT(CLAY_STRING("COPY"),
                        CLAY_TEXT_CONFIG({.fontId        = 0,
                                          .fontSize      = 20,
                                          .textColor     = {255, 255, 255, 255}}));
              }
            }
          }
        }
      };
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    // SDL_Clay_RenderClayCommands(Clay_SDL3RendererData *rendererData, Clay_RenderCommandArray
    // *rcommands)
    SDL_SetRenderDrawColor(renderer, 186, 167, 136, 255);

    SDL_RenderFillRects(renderer, rects, cellsToDraw);
    SDL_Clay_RenderClayCommands(clay_sdl3, &renderCommands);
    // SDL_SetRenderDrawColor(renderer, 186, 167, 136, 255);

    // SDL_RenderFillRects(renderer, rects, cellsToDraw);

    SDL_RenderPresent(renderer);
  }

  // Close and destroy the window
  SDL_DestroyWindow(window);

  // Clean up
  SDL_Quit();
  return 0;
}