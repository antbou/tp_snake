#include "snake.h"
#include "../queue/queue.h"
#include <stdlib.h>

struct queue_t* init_snake(const int width, const int height) {
    struct queue_t* queue = queue_create();

    const int x = width / 2;
    const int y = height / 2;

    // Initialize the snake's body (head, mid, tail) at the center of the grid
    struct element_t* head = position_init(x, y);
    struct element_t* mid = position_init(x, y - 1);
    struct element_t* tail = position_init(x, y - 2);

    // Enqueue the snake's body parts into the queue
    queue_enqueue(queue, tail);
    queue_enqueue(queue, mid);
    queue_enqueue(queue, head);

    return queue;
}

// lorsque le serpent “mange” de la nourriture sa longueur augmente de un et la nourriture disparait ;