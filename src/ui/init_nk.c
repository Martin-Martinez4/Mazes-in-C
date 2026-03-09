
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "config_nk.h"

#define NK_IMPLEMENTATION
#include "nuklear.h"

#define NK_SDL3_RENDERER_IMPLEMENTATION
#include "nuklear_sdl3_renderer.h"

#include "init_nk.h"

struct nk_sdl_app {
  SDL_Window* window;
  SDL_Renderer* renderer;
  struct nk_context* ctx;
  struct nk_colorf bg;
  enum nk_anti_aliasing AA;
};


struct nk_context* initNk(SDL_Window* window, SDL_Renderer* renderer) {
  struct nk_context* ctx = nk_sdl_init(window, renderer, nk_sdl_allocator());
  if (!ctx) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "nk_sdl3_init failed");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(-1);
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

  return ctx;
}