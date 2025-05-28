#include "snake.h"
#include "../queue/queue.h"
#include <stdlib.h>

struct queue_t* init_snake(const int width, const int height) {
    struct queue_t* queue = queue_create();

    const int x = width / 2;
    const int y = height / 2;

    // Initialize the snake's body dynamically at the center of the grid
    // The head of the snake is at the front of the queue
    const int initial_length = 3; // Initial length of the snake
    for (int i = 0; i < initial_length; i++) {
        struct coord_t* segment = coord_init(x, y - i);
        queue_enqueue(queue, segment);
    }

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


// lorsque le serpent “mange” de la nourriture sa longueur augmente de un et la nourriture disparait