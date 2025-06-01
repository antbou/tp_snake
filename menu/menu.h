#ifndef _MENU_H
#define _MENU_H

#include "../gfx/gfx.h"

#include <stdbool.h>

#define FONT_PATH "assets/PixelOperatorMono8.ttf"

enum difficulty_level {
    EASY,
    NORMAL,
    HARD,
    LEAVE,
};

/**
 * Display the start screen where the player can select a difficulty level.
 *
 * This menu allows navigation using the arrow keys (or W/S) and selection via
 * ENTER or SPACE. The options are highlighted dynamically. If the user closes
 * the window, the function returns LEAVE to signal an exit request.
 *
 * @param ctxt Pointer to the graphics context used for rendering.
 * @return The selected difficulty level (EASY, NORMAL, HARD), or LEAVE if the player quits.
 */
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
