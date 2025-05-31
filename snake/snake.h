#ifndef _SNAKE_H_
#define _SNAKE_H_

#include "../coord/coord.h"
#include "../gfx/gfx.h"

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

struct queue_t* init_snake(const int width, const int height, const int zoom);
struct coord_t* new_position(enum direction dir, struct coord_t* element, const int zoom);
void draw_snake_initial(struct gfx_context_t* ctxt, struct queue_t* queue, int zoom, uint32_t color);
void move_snake(struct gfx_context_t* ctxt, struct queue_t* queue, struct coord_t* new_pos, int zoom, uint32_t color_snake, uint32_t color_empty);
/**
 * Determines the type of collision based on the pixel content
 * at the given coordinate in the context.
 *
 * @param context the graphics context
 * @param pos the position to evaluate
 * @return the corresponding collision type (wall, snake, food, or none)
 */
enum collision_type get_collision_type(struct gfx_context_t* ctxt, struct coord_t* pos, int zoom);
#endif