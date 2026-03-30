#include "layout_nk.h"
#include "ui_state.h"
#include "create_maze.h"
#include <SDL3/SDL.h>
#include "stdio.h"
#include "stdbool.h"
#include "noise.h"

// static int text_len[9];
// static char text[9][64];
const char* algos[3]         = {"Backtracking", "Prim's", "Kruskal's"};
const char* noise_choices[5] = {"Value", "Perlin", "Simplex", "Linear Gradient", "Radial Gradient"};

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

      struct nk_vec2 old_spacing = ctx->style.window.spacing;

      if (nk_begin(ctx, "Menu", nk_rect(10, 36, 240, 380),
                   NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_DYNAMIC)) {

        if (nk_tree_push(ctx, NK_TREE_TAB, "Cells Settings", NK_MINIMIZED)) {

          nk_layout_row_dynamic(ctx, 0, 1);
          nk_property_int(ctx, "Cell Width", 1, &state.cell_width, 128, 1, 1);
          nk_property_int(ctx, "Cell Height", 1, &state.cell_height, 128, 1, 1);
          nk_property_int(ctx, "Border Thickness", 1, &state.border_thickness, 16, 1, 1);

          nk_tree_pop(ctx);
        }

        if (nk_tree_push(ctx, NK_TREE_TAB, "Algorithms", NK_MINIMIZED)) {
          nk_layout_row_dynamic(ctx, 0, 1);

          nk_property_float(ctx, "Room Saturation", 0.0f, &state.room_saturation, 0.95f, 0.01f,
                            0.01f);
          nk_property_int(ctx, "Prune Aggressiveness", 0, &state.prune_aggressiveness, 20, 1, 1);

          if (nk_option_label(ctx, "Backtracking", state.algoSelected == BACKTRACKING)) {
            state.hybrid_options[0] = BACKTRACKING;
            state.algoSelected      = BACKTRACKING;
          }
          if (nk_option_label(ctx, "Prim's", state.algoSelected == PRIMS)) {
            state.hybrid_options[0] = PRIMS;
            state.algoSelected      = PRIMS;
          }
          if (nk_option_label(ctx, "Kruskal's", state.algoSelected == KRUSKALS)) {
            state.hybrid_options[0] = KRUSKALS;
            state.algoSelected      = KRUSKALS;
          }
          if (nk_option_label(ctx, "Hybrid", state.algoSelected == HYBRID))
            state.algoSelected = HYBRID;

          if (state.algoSelected == HYBRID) {
            // Number of algorithms
            nk_layout_row_dynamic(ctx, 22, 1);
            nk_property_int(ctx, "Number of Algorithms", 2, &state.num_algos, 5, 1, 1);

            // Hybrid combos
            for (int i = 0; i < state.num_algos; i++) {
              char label[32];
              snprintf(label, sizeof(label), "Hybrid %d", i + 1);

              // One row: label + combo
              nk_layout_row_dynamic(ctx, 25, 2);
              nk_label(ctx, label, NK_TEXT_LEFT);
              state.hybrid_options[i] =
                  nk_combo(ctx, algos, 3, state.hybrid_options[i], 25, nk_vec2(200, 150));
            }
          }

          nk_tree_pop(ctx);
        }

        if (nk_tree_push(ctx, NK_TREE_TAB, "Noise", NK_MINIMIZED)) {
          nk_layout_row_dynamic(ctx, 0, 1);
          nk_checkbox_label(ctx, "See Noise", &state.mCheck);

          nk_layout_row_dynamic(ctx, 22, 1);
          nk_property_float(ctx, "Scale", 0.01f, &state.scale, 0.99f, 0.01f, 0.01f);

          char label[32];
          snprintf(label, sizeof(label), "Noise");

          // One row: label + combo
          nk_layout_row_dynamic(ctx, 25, 2);
          nk_label(ctx, label, NK_TEXT_LEFT);
          state.noise_selected =
              nk_combo(ctx, noise_choices, 5, state.noise_selected, 25, nk_vec2(200, 150));

          if (state.noise_selected == LINEAR_GRADIENT) {
            nk_layout_row_dynamic(ctx, 22, 1);
            nk_property_float(ctx, "Degrees", 0.01f, &state.degrees, 360.0f, 1.0f, 1.0f);
          }
          if (state.noise_selected == RADIAL_GRADIENT) {
            nk_layout_row_dynamic(ctx, 22, 1);
            nk_property_int(ctx, "cx in %", 0, &state.cx, 100, 1, 1);

            nk_layout_row_dynamic(ctx, 22, 1);
            nk_property_int(ctx, "cy in %", 0, &state.cy, 100, 1, 1);
          }

          if (nk_button_label(ctx, "Generate Noise")) {
            state.generate_noise = true;
          }

          nk_tree_pop(ctx);
        }

        if (nk_tree_push(ctx, NK_TREE_TAB, "File", NK_MINIMIZED)) {
          nk_layout_row_dynamic(ctx, 5, 1);
          nk_spacing(ctx, 1);

          nk_layout_row_dynamic(ctx, 0, 1);
          nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, state.fileName, sizeof(state.fileName),
                                         nk_filter_default);
          if (nk_button_label(ctx, "Export")) {

            state.export = true;
            state.upload = false;
          }
          nk_layout_row_dynamic(ctx, 5, 1);
          nk_spacing(ctx, 1);

          nk_layout_row_dynamic(ctx, 0, 1);
          nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, state.uploadFileName,
                                         sizeof(state.uploadFileName), nk_filter_default);
          if (nk_button_label(ctx, "Load")) {
            state.upload = true;
            state.export = false;
          }
          nk_tree_pop(ctx);
        }

        if (nk_button_label(ctx, "Generate Maze")) {
          SDL_Log("Generate Maze clicked (algo=%d)", state.algoSelected);
          state.redrawMaze = true;
        }
      }
      nk_end(ctx);
      ctx->style.window.spacing = old_spacing;
    }
  }
}