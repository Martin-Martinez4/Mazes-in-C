#include "layout_nk.h"
#include "ui_state.h"
#include "nuklear.h"
#include "create_maze.h"
#include <SDL3/SDL.h>
#include "stdio.h"

// static int text_len[9];
// static char text[9][64];

static char buf[256] = {0};
static int len       = 0;

void renderNk(struct nk_context* ctx) {
  if (state.menuVisible) {

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
        state.menuExpanded = !state.menuExpanded;
        SDL_Log("Toggled menu: %s", state.menuExpanded ? "expanded" : "hidden");
      }

      // Restore old style
      ctx->style.button.padding        = old_padding;
      ctx->style.button.text_alignment = old_align;
    }
    nk_end(ctx);

    /* ----------------- Optional menu ----------------- */
    if (state.menuExpanded) {
      if (nk_begin(ctx, "Menu", nk_rect(10, 36, 240, 220),
                   NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Algorithms", NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 20, 1);
        if (nk_option_label(ctx, "Backtracking", state.algoSelected == BACKTRACKING))
          state.algoSelected = BACKTRACKING;
        if (nk_option_label(ctx, "Prim's", state.algoSelected == PRIMS))
          state.algoSelected = PRIMS;
        if (nk_option_label(ctx, "Kruskal's", state.algoSelected == KRUSKALS))
          state.algoSelected = KRUSKALS;

        nk_layout_row_dynamic(ctx, 20, 1);
        if (nk_button_label(ctx, "Generate Maze")) {
          SDL_Log("Generate Maze clicked (algo=%d)", state.algoSelected);
          state.redrawMaze = true;
        }
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, state.fileName, sizeof(state.fileName),
                                       nk_filter_default);
        if (nk_button_label(ctx, "Export")) {

          state.export = true;
        }
      }
      nk_end(ctx);
    }
  }
}