#ifndef _SNAKE_H_
#define _SNAKE_H_

#include "../coord/coord.h"
#include "../gfx/gfx.h"
#include "../queue/queue.h"

enum direction {
    left,
    up,
    down,
    right
};

enum collision_type {
    NO_COLLISION,
    WALL_COLLISION,
    SNAKE_COLLISION,
    FOOD_COLLISION
};

/**
 * Initializes the snake at the center of the screen.
 *
 * @param width the screen width
 * @param height the screen height
 * @param zoom the zoom level (grid size)
 * @return a pointer to a queue containing the initial snake body
 */
struct queue_t* init_snake(const int width, const int height, const int zoom);

/**
 * Calculates a new position based on the current direction and position.
 *
 * @param dir the direction of movement
 * @param element a pointer to the current coordinate (will not be modified)
 * @param zoom the grid size
 * @return a newly allocated coordinate for the new position
 */
struct coord_t* new_position(enum direction dir, const struct coord_t* element, const int zoom);

/**
 * Draws the initial snake on the screen using the provided queue.
 *
 * @param ctxt the graphics context
 * @param queue a pointer to the snake's coordinate queue (read-only)
 * @param zoom the size of each snake segment
 * @param color the color to use for the snake
 */
void draw_snake_initial(struct gfx_context_t* ctxt, const struct queue_t* queue, int zoom, uint32_t color);

/**
 * Moves the snake by drawing the new head and clearing the tail.
 *
 * @param ctxt the graphics context
 * @param queue a pointer to the snake queue (will be updated)
 * @param new_pos the new head position
 * @param zoom the size of each cell
 * @param color_snake the color for the new head
 * @param color_empty the color to clear the tail
 */
void move_snake(struct gfx_context_t* ctxt, struct queue_t* queue, struct coord_t* new_pos, int zoom, uint32_t color_snake, uint32_t color_empty);

/**
 * Determines the type of collision based on the pixel content
 * at the given coordinate in the context.
 *
 * @param ctxt the graphics context
 * @param pos the position to evaluate
 * @param zoom the size of the cell (used to scale coordinates)
 * @return the corresponding collision type (wall, snake, food, or none)
 */
enum collision_type get_collision_type(struct gfx_context_t* ctxt, const struct coord_t* pos, int zoom);
#endif