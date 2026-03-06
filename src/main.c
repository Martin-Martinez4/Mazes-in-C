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

#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_INCLUDE_COMMAND_USERDATA
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT

#include "nuklear.h"

#define NK_SDL3_RENDERER_IMPLEMENTATION
#include "nuklear_sdl3_renderer.h"

const int WINDOW_WIDTH  = 1080;
const int WINDOW_HEIGHT = 800;

const int CELL_HEIGHT  = 12;
const int CELL_WIDTH   = 12;
const int BORDER_WIDTH = 1;

struct nk_sdl_app {
  SDL_Window* window;
  SDL_Renderer* renderer;
  struct nk_context* ctx;
  struct nk_colorf bg;
  enum nk_anti_aliasing AA;
};

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

  struct nk_context* ctx = nk_sdl_init(window, renderer, nk_sdl_allocator());
  if (!ctx) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "nk_sdl3_init failed");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }
  enum nk_anti_aliasing AA;
  int font_scale = 1;
  {
    /* This scaling logic was kept simple for the demo purpose.
     * On some platforms, this might not be the exact scale
     * that you want to use. For more information, see:
     * https://wiki.libsdl.org/SDL3/README-highdpi */
    const float scale = SDL_GetWindowDisplayScale(window);
    SDL_SetRenderScale(renderer, scale, scale);
    font_scale = scale;
  }

  ctx = nk_sdl_init(window, renderer, nk_sdl_allocator());
  ctx = ctx;

#if 0
    {
        /* If you don't want to use advanced Nuklear font baking API
         * you can use simple ASCII debug font provided by SDL
         * just change the `#if 0` above to `#if 1` */
        nk_sdl_style_set_debug_font(ctx);

        /* Note that since debug font is extremely small (only 8x8 pixels),
         * scaling it does not make much sense. The font would appear blurry. */
        NK_UNUSED(font_scale);

        /* You may wish to change a few style options, here are few recommendations: */
        ctx->style.button.rounding = 0.0f;
        ctx->style.menu_button.rounding = 0.0f;
        ctx->style.property.rounding = 0.0f;
        ctx->style.property.border = 0.0f;
        ctx->style.option.border = -1.0f;
        ctx->style.checkbox.border = -1.0f;
        ctx->style.property.dec_button.border = -2.0f;
        ctx->style.property.inc_button.border = -2.0f;
        ctx->style.tab.tab_minimize_button.border = -2.0f;
        ctx->style.tab.tab_maximize_button.border = -2.0f;
        ctx->style.tab.node_minimize_button.border = -2.0f;
        ctx->style.tab.node_maximize_button.border = -2.0f;
        ctx->style.checkbox.spacing = 5.0f;

        /* It's better to disable anti-aliasing when using small fonts */
        app->AA = NK_ANTI_ALIASING_OFF;
    }
#else
  {
    struct nk_font_atlas* atlas;
    struct nk_font_config config = nk_font_config(0);
    struct nk_font* font;

    /* set up the font atlas and add desired font; note that font sizes are
     * multiplied by font_scale to produce better results at higher DPIs */
    atlas = nk_sdl_font_stash_begin(ctx);
    font  = nk_font_atlas_add_default(atlas, 13 * font_scale, &config);
    /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14 *
     * font_scale, &config);*/
    /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16 *
     * font_scale, &config);*/
    /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13
     * * font_scale, &config);*/
    /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12 *
     * font_scale, &config);*/
    /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10 *
     * font_scale, &config);*/
    /*font = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13 *
     * font_scale, &config);*/
    nk_sdl_font_stash_end(ctx);

    /* this hack makes the font appear to be scaled down to the desired
     * size and is only necessary when font_scale > 1 */
    font->handle.height /= font_scale;
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    nk_style_set_font(ctx, &font->handle);

    AA = NK_ANTI_ALIASING_ON;
  }
#endif

  // 0 = Backtracking, 1 = Prim, 2 = Kruskal
  int algoSelected = 0;

  // --- Here ---
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
  // --- To Here ---

  // SDL_FRect background = {.x = 0, .y = 0, .h = mazeStats.canvasHeight, .w = mazeStats.canvasWidth};

  int width, height;

  bool menuVisible  = true;
  bool menuExpanded = false;

  while (!done) {

    SDL_Event event;

    nk_input_begin(ctx);

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        done = true;
      nk_sdl_handle_event(ctx, &event);

      switch (event.type) {
      case SDL_EVENT_KEY_DOWN:
        if (event.key.scancode == SDL_SCANCODE_M) {
          menuVisible = !menuVisible;
        }
      }
    }

    nk_input_end(ctx);

    SDL_GetWindowSize(window, &width, &height);
    // Do game logic, present a frame, etc.

    if (menuVisible) {

      // fill with black background
      if (nk_begin(ctx, "HeaderBar", nk_rect(10, 10, 24, 24),
                   NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {

        // row with height 48px
        nk_layout_row_dynamic(ctx, 24, 1);

        // Save old padding & alignment
        struct nk_vec2 old_padding       = ctx->style.button.padding;
        enum nk_text_alignment old_align = ctx->style.button.text_alignment;

        // Temporarily change button style
        ctx->style.button.padding        = nk_vec2(0, 0);
        ctx->style.button.text_alignment = NK_TEXT_CENTERED;
        if (nk_button_label(ctx, "-")) {
          menuExpanded = !menuExpanded;
          SDL_Log("Toggled menu: %s", menuExpanded ? "expanded" : "hidden");
        }

        // Restore old style
        ctx->style.button.padding        = old_padding;
        ctx->style.button.text_alignment = old_align;
      }
      nk_end(ctx);

      /* ----------------- Optional menu ----------------- */
      if (menuExpanded) {
        if (nk_begin(ctx, "Menu", nk_rect(10, 36, 240, 220),
                     NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {

          nk_layout_row_dynamic(ctx, 20, 1);
          nk_label(ctx, "Algorithms", NK_TEXT_LEFT);

          nk_layout_row_dynamic(ctx, 20, 1);
          if (nk_option_label(ctx, "Backtracking", algoSelected == 0))
            algoSelected = 0;
          if (nk_option_label(ctx, "Prim's", algoSelected == 1))
            algoSelected = 1;
          if (nk_option_label(ctx, "Kruskal's", algoSelected == 2))
            algoSelected = 2;

          nk_layout_row_dynamic(ctx, 20, 1);
          if (nk_button_label(ctx, "Generate Maze")) {
            SDL_Log("Generate Maze clicked (algo=%d)", algoSelected);
          }
        }
        nk_end(ctx);
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 186, 167, 136, 255);
    SDL_RenderFillRects(renderer, rects, cellsToDraw);

   
    nk_sdl_render(ctx, NK_ANTI_ALIASING_ON);

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