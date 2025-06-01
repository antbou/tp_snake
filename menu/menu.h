#ifndef _MENU_H
#define _MENU_H

#define FONT_PATH "assets/PixelOperatorMono8.ttf"

#include "../gfx/gfx.h"

enum difficulty_level {
    EASY,
    NORMAL,
    HARD,
    LEAVE,
};

enum difficulty_level show_start_screen(struct gfx_context_t* ctxt);

/**
 * @brief Displays the end screen with the final score and replay options.
 *
 * This function shows the game over screen, including the player's score,
 * and prompts the user to choose whether to play again or exit the game.
 * It also adapts the message depending on whether the player has won.
 *
 * @param ctxt The graphics context used for rendering.
 * @param score The final score achieved by the player.
 * @param does_player_win A boolean indicating if the player has won the game.
 *
 * @return true if the player chooses to play again, false to exit.
 */
bool show_end_screen(struct gfx_context_t* ctxt, int const score, bool does_player_win);

#endif
