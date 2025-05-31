#include "snake.h"
#include <stdlib.h>
#include "../queue/queue.h"

struct queue_t* init_snake(const int width, const int height, const int zoom) {
    struct queue_t* queue = queue_create();

    // Align the central coordinates to the grid based on ZOOM
    const int x = (width / 2 / zoom) * zoom;
    const int y = (height / 2 / zoom) * zoom;

    // Initialize the snake's body (head, mid, tail) at the center of the grid
    struct coord_t* head = coord_init(x, y);
    struct coord_t* mid = coord_init(x, y - zoom);
    struct coord_t* tail = coord_init(x, y - 2 * zoom);

    // Enqueue the snake's body parts into the queue
    queue_enqueue(queue, tail);
    queue_enqueue(queue, mid);
    queue_enqueue(queue, head);

    return queue;
}

struct coord_t* new_position(enum direction dir, struct coord_t* element, const int zoom) {
    int new_x = element->x;
    int new_y = element->y;
    switch (dir) {
    case left:
        new_x -= zoom;
        break;
    case right:
        new_x += zoom;
        break;
    case up:
        new_y -= zoom;
        break;
    case down:
        new_y += zoom;
        break;
    }

    return coord_init(new_x, new_y);
}


void draw_snake_initial(struct gfx_context_t* ctxt, struct queue_t* queue, int zoom, uint32_t color) {
    struct coord_t* current = queue->head;
    while (current != NULL) {
        draw_pixel(ctxt, current->x, current->y, zoom, color);
        current = current->next;
    }
}

void move_snake(struct gfx_context_t* ctxt, struct queue_t* queue, struct coord_t* new_pos, int zoom, uint32_t color_snake, uint32_t color_empty) {
    draw_pixel(ctxt, new_pos->x, new_pos->y, zoom, color_snake);
    queue_enqueue(queue, new_pos);

    struct coord_t* old_tail = queue->head;
    int tail_x = old_tail->x;
    int tail_y = old_tail->y;

    queue_dequeue(queue);
    draw_pixel(ctxt, tail_x, tail_y, zoom, color_empty);
}

enum collision_type get_collision_type(struct gfx_context_t* ctxt, struct coord_t* pos, int zoom) {
    for (int ix = 0; ix < zoom; ix++) {
        for (int iy = 0; iy < zoom; iy++) {
            uint32_t pixel = gfx_getpixel(ctxt, pos->x + ix, pos->y + iy);
            switch (pixel) {
            case COLOR_BLUE:
                return WALL_COLLISION;
            case COLOR_WHITE:
                return SNAKE_COLLISION;
            case COLOR_RED:
                return FOOD_COLLISION;
            default:
                break;
            }
        }
    }
    return NO_COLLISION;
}