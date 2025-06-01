#include "menu.h"

#include <SDL2/SDL.h>

#include <stdio.h>
#include <unistd.h>

/**
 * Render a menu item (e.g., "EASY", "PLAY AGAIN") at a given vertical position.
 * The text is centered horizontally and rendered in red if selected, otherwise white.
 *
 * @param ctxt The graphics context containing rendering state and dimensions.
 * @param label The text to render.
 * @param y The vertical position (in pixels) of the text.
 * @param selected True if this item is currently selected (highlighted).
 */
static void draw_menu_item(struct gfx_context_t* ctxt, const char* label, int y, bool selected) {
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color red = { 255, 0, 0, 255 };
    draw_text_ttf(ctxt, label, ctxt->width / 2 - 100, y, 32, selected ? red : white, FONT_PATH);
}

/**
 * Render a generic text label (e.g., title or instructions) centered horizontally.
 *
 * @param ctxt The graphics context for drawing.
 * @param text The string to display.
 * @param y The vertical position on the screen.
 * @param size Font size in points.
 * @param color Text color (SDL_Color struct).
 */
static void draw_label(struct gfx_context_t* ctxt, const char* text, int y, int size, SDL_Color color) {
    draw_text_ttf(ctxt, text, ctxt->width / 2 - 100, y, size, color, FONT_PATH);
}

/**
 * Processes user input to navigate the menu.
 *
 * @param selection Current menu selection.
 * @param min Minimum selection index.
 * @param max Maximum selection index.
 * @param confirmed Set to true if the user confirms the selection (ENTER or SPACE).
 * @return Updated selection index (same or modified based on input).
 */
static int handle_selection_input(int selection, int min, int max, bool* confirmed) {
    SDL_Keycode key = gfx_keypressed();
    switch (key) {
    case SDLK_UP:
        if (selection > min) selection--;
        break;
    case SDLK_DOWN:
        if (selection < max) selection++;
        break;
    case SDLK_RETURN:
    case SDLK_SPACE:
        *confirmed = true;
        break;
    default:
        break;
    }
    return selection;
}

enum difficulty_level show_start_screen(struct gfx_context_t* ctxt) {
    int selection = NORMAL;
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color blue = { 0, 0, 255, 255 };

    const int spacing = 60;
    const int y = ctxt->height / 2 - 2 * spacing;

    while (true) {
        if (quit_signal()) {
            return LEAVE;
        }

        gfx_clear(ctxt, COLOR_BLACK);
        draw_label(ctxt, "SNAKE", y - 2 * spacing, 48, white);
        draw_menu_item(ctxt, "EASY", y, selection == EASY);
        draw_menu_item(ctxt, "NORMAL", y + spacing, selection == NORMAL);
        draw_menu_item(ctxt, "HARD", y + 2 * spacing, selection == HARD);
        draw_label(ctxt, "PRESS ENTER", y + 4 * spacing, 24, blue);

        SDL_RenderPresent(ctxt->renderer);

        bool confirmed = false;
        selection = handle_selection_input(selection, EASY, HARD, &confirmed);
        if (confirmed) {
            break;
        }
        if (quit_signal()) {
            return LEAVE;
        }

        usleep(2000); // Avoid skipping options by holding key
    }

    return (enum difficulty_level)selection;
}

bool show_end_screen(struct gfx_context_t* ctxt, int score, bool does_player_win) {
    int selection = 0;  // 0 = play again, 1 = leave
    SDL_Color white = { 255, 255, 255, 255 };

    const int spacing = 60;
    const int y = ctxt->height / 2 - 2 * spacing;

    while (true) {
        if (quit_signal()) {
            return false;
        }

        gfx_clear(ctxt, COLOR_BLACK);

        const char* result_text = does_player_win ? "YOU WIN" : "GAME OVER";

        char score_text[32];
        snprintf(score_text, sizeof(score_text), "Your score is %d", score);

        draw_label(ctxt, result_text, y - 2 * spacing, 48, white);
        draw_label(ctxt, score_text, y - spacing, 32, white);
        draw_menu_item(ctxt, "PLAY AGAIN", y, selection == 0);
        draw_menu_item(ctxt, "LEAVE", y + spacing, selection == 1);

        SDL_RenderPresent(ctxt->renderer);

        bool confirmed = false;
        selection = handle_selection_input(selection, 0, 1, &confirmed);
        if (confirmed) {
            break;
        }
        if (quit_signal()) {
            return LEAVE;
        }

        usleep(2000); // Avoid double selection due to fast key repeat
    }

    return (selection == 0);
}