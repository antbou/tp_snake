#include "menu.h"

#include <unistd.h>
#include <SDL2/SDL.h>
#include "../gfx/gfx.h"

enum difficulty_level show_start_screen(struct gfx_context_t* ctxt) {
    enum difficulty_level selection = NORMAL;
    bool choosing = true;

    while (choosing) {
        gfx_clear(ctxt, COLOR_BLACK);
        draw_text(ctxt, "EASY", 860, 250, 2, (selection == EASY ? COLOR_RED : COLOR_WHITE));
        draw_text(ctxt, "NORMAL", 860, 310, 2, (selection == NORMAL ? COLOR_RED : COLOR_WHITE));
        draw_text(ctxt, "HARD", 860, 370, 2, (selection == HARD ? COLOR_RED : COLOR_WHITE));
        draw_text(ctxt, "PRESS ENTER", 800, 450, 2, COLOR_BLUE);
        draw_text(ctxt, "SNAKE", 880, 100, 3, COLOR_WHITE);

        gfx_present(ctxt);

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
        case SDLK_ESCAPE:
            exit(EXIT_SUCCESS);
        }

        usleep(120000);
    }

    return selection;
}
