#include "menu.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "../gfx/gfx.h"

static void draw_menu_item(struct gfx_context_t* ctxt, const char* label, int y, bool selected) {
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color red = { 255, 0, 0, 255 };
    draw_text_ttf(ctxt, label, 860, y, 32, selected ? red : white, FONT_PATH);
}

static void draw_label(struct gfx_context_t* ctxt, const char* text, int y, int size, SDL_Color color) {
    draw_text_ttf(ctxt, text, 860, y, size, color, FONT_PATH);
}

/// Handle keyboard navigation for a menu
/// @param selection Pointer to the currently selected index
/// @param min Minimum selectable value
/// @param max Maximum selectable value
/// @param confirmed Output flag set to true if user validated (enter/space)
/// @return true if quit signal received, false otherwise
static bool handle_selection_input(int* selection, int min, int max, bool* confirmed) {
    SDL_Keycode key = gfx_keypressed();
    switch (key) {
    case SDLK_UP: {
        if (*selection > min) {
            (*selection)--;  // Move selection up
        }
        break;
    }
    case SDLK_DOWN: {
        if (*selection < max) {
            (*selection)++;  // Move selection down
        }
        break;
    }
    case SDLK_RETURN:
    case SDLK_SPACE: {
        *confirmed = true;  // Validate selection
        break;
    }
    default: {
        break;
    }
    }

    if (quit_signal()) {
        return true;
    }

    return false;
}

enum difficulty_level show_start_screen(struct gfx_context_t* ctxt) {
    SDL_RenderClear(ctxt->renderer);
    int selection = NORMAL;
    bool choosing = true;
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color blue = { 0, 0, 255, 255 };

    while (choosing) {
        gfx_clear(ctxt, COLOR_BLACK);
        draw_label(ctxt, "SNAKE", 100, 48, white);
        draw_menu_item(ctxt, "EASY", 250, selection == EASY);
        draw_menu_item(ctxt, "NORMAL", 310, selection == NORMAL);
        draw_menu_item(ctxt, "HARD", 370, selection == HARD);
        draw_label(ctxt, "PRESS ENTER", 450, 24, blue);

        SDL_RenderPresent(ctxt->renderer);
        bool confirmed = false;
        if (handle_selection_input(&selection, EASY, HARD, &confirmed)) {
            return LEAVE;
        }

        if (confirmed) {
            choosing = false;
        }

        usleep(120000); // Avoid skipping options by holding key
    }

    // Cast selection index back to enum
    return (enum difficulty_level)selection;
}

bool show_end_screen(struct gfx_context_t* ctxt, int score, bool does_player_win) {
    int selection = 0;  // 0 = play again, 1 = leave
    bool choosing = true;
    SDL_Color white = { 255, 255, 255, 255 };

    while (choosing) {
        gfx_clear(ctxt, COLOR_BLACK);

        const char* result_text = does_player_win ? "YOU WIN" : "GAME OVER";

        char score_text[32]; // 15 + 11 + 1 = max 27
        snprintf(score_text, sizeof(score_text), "Your score is %d", score);

        draw_label(ctxt, result_text, 100, 48, white);
        draw_label(ctxt, score_text, 250, 32, white);
        draw_menu_item(ctxt, "PLAY AGAIN", 310, selection == 0);
        draw_menu_item(ctxt, "LEAVE", 370, selection == 1);

        SDL_RenderPresent(ctxt->renderer);

        bool confirmed = false;
        if (handle_selection_input(&selection, 0, 1, &confirmed)) {
            return false;
        }
        if (confirmed) {
            choosing = false;
        }

        usleep(120000); // Avoid double selection due to fast key repeat
    }

    return (selection == 0);
}
