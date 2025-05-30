#include "menu.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

enum difficulty_level show_start_screen(struct gfx_context_t* ctxt) {
    enum difficulty_level selection = NORMAL;
    bool choosing = true;

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color red = { 255, 0, 0, 255 };
    SDL_Color blue = { 0, 0, 255, 255 };

    while (choosing) {
        gfx_clear(ctxt, COLOR_BLACK);

        draw_text_ttf(ctxt, "SNAKE", 860, 100, 48, white, "assets/PixelOperatorMono8.ttf");
        draw_text_ttf(ctxt, "EASY", 860, 250, 32, selection == EASY ? red : white, "assets/PixelOperatorMono8.ttf");
        draw_text_ttf(ctxt, "NORMAL", 860, 310, 32, selection == NORMAL ? red : white, "assets/PixelOperatorMono8.ttf");
        draw_text_ttf(ctxt, "HARD", 860, 370, 32, selection == HARD ? red : white, "assets/PixelOperatorMono8.ttf");
        draw_text_ttf(ctxt, "PRESS ENTER", 860, 450, 24, blue, "assets/PixelOperatorMono8.ttf");

        SDL_RenderPresent(ctxt->renderer);

        SDL_Keycode key = gfx_keypressed();
        switch (key) {
        case SDLK_UP:
            if (selection > 0) selection--;
            break;
        case SDLK_DOWN:
            if (selection < 2) selection++;
            break;
        case SDLK_RETURN:
        case SDLK_SPACE:
            choosing = false;
            break;
        }

        if (quit_signal()) {
            return LEAVE;
        }

        usleep(120000); // Add a short delay to prevent the menu from scrolling too fast when holding a key
    }

    return selection;
}
