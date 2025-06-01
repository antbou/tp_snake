#ifndef _FOOD_H_
#define _FOOD_H_

#include "../gfx/gfx.h"
#include "../coord/coord.h"

/**
 * Spawn a new food item at a random empty location within the game area.
 *
 * This function searches for a position where the pixel color matches `empty_color`
 * and places a food pixel (`food_color`) at that location.
 *
 * @param ctxt Pointer to the graphics context
 * @param border_offset Distance in pixels from the window edges defining the game border
 * @param zoom Size of a grid cell (used to align the food to the grid)
 * @param empty_color Color representing an empty cell
 * @param food_color Color used to draw the food
 */
void spawn_food(struct gfx_context_t* ctxt, const int border_offset, const int zoom, const uint32_t empty_color, const uint32_t food_color);

#endif
