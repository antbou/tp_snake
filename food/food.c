#include "food.h"

#include <stdlib.h>
#include <stdio.h>

/**
 * Generate a coordinate for placing a new food item in the game grid.
 *
 * The function randomly selects a position inside the playable area
 * (bounded by the window size and the border offset), ensuring the location is empty.
 *
 * @param context Pointer to the graphics context (used to check pixel colors and bounds)
 * @param border_offset Margin in pixels around the playable area
 * @param zoom Grid size to align the food position
 * @param empty_color Color representing an empty space (required for validation)
 * @return Pointer to a newly allocated coord_t with food position, or NULL on failure
 */
static struct coord_t* generate_food(struct gfx_context_t* context, const int border_offset, const int zoom, const uint32_t empty_color) {
    const int x_min = ((border_offset + zoom - 1) / zoom) * zoom;
    const int y_min = ((border_offset + zoom - 1) / zoom) * zoom;
    const int x_max = context->width - border_offset;
    const int y_max = context->height - border_offset;

    const int x_range = (x_max - x_min) / zoom;
    const int y_range = (y_max - y_min) / zoom;

    if (x_range <= 0 || y_range <= 0) {
        fprintf(stderr, "Error: playable area too small to place food.\n");
        return NULL;
    }

    struct coord_t* food = coord_init(x_min, y_min);
    if (!food) {
        return NULL;
    }

    do {
        food->x = (rand() % x_range) * zoom + x_min;
        food->y = (rand() % y_range) * zoom + y_min;
    } while (gfx_getpixel(context, food->x, food->y) != empty_color);

    return food;
}

void spawn_food(struct gfx_context_t* ctxt, const int border_offset, const int zoom, const uint32_t empty_color, const uint32_t food_color) {
    struct coord_t* food = generate_food(ctxt, border_offset, zoom, empty_color);
    if (food != NULL) {
        draw_pixel(ctxt, food->x, food->y, zoom, food_color);
        free(food);
    }
}