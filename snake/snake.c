#include "snake.h"
#include "../queue/queue.h"
#include <stdlib.h>

struct queue_t* init_snake(const int width, const int height) {
    struct queue_t* queue = queue_create();

    const int x = width / 2;
    const int y = height / 2;

    // Initialize the snake's body (head, mid, tail) at the center of the grid
    struct coord_t* head = coord_init(x, y);
    struct coord_t* mid = coord_init(x, y - 1);
    struct coord_t* tail = coord_init(x, y - 2);

    // Enqueue the snake's body parts into the queue
    queue_enqueue(queue, tail);
    queue_enqueue(queue, mid);
    queue_enqueue(queue, head);

    return queue;
}

struct coord_t* new_position(enum direction dir, struct coord_t* element) {
    int new_x = element->x;
    int new_y = element->y;
    const int dist = 4;
    switch (dir) {
    case left:
        new_x -= dist;
        break;
    case right:
        new_x += dist;
        break;
    case up:
        new_y -= dist;
        break;
    case down:
        new_y += dist;
        break;
    }

    return coord_init(new_x, new_y);
}


// lorsque le serpent “mange” de la nourriture sa longueur augmente de un et la nourriture disparait ;