#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdbool.h>

#include "../coord/coord.h"

struct queue_t {
    struct coord_t* head;
    struct coord_t* tail;
    int size;
};

struct coord_t* coord_init(int x, int y);

/**
 * Create and initialize an empty queue.
 * @return A pointer to the newly created queue, or NULL if allocation fails.
 */
struct queue_t* queue_create();

/**
 * Destroy a queue and free all associated memory.
 * @param queue A pointer to the pointer of the queue to destroy.
 * @return true if the queue was successfully destroyed, false otherwise.
 */
bool queue_destroy(struct queue_t** queue);

/**
 * Check if the queue is empty.
 * @param queue A pointer to the queue.
 * @return true if the queue is empty or NULL, false otherwise.
 */
bool queue_isEmpty(struct queue_t* queue);

/**
 * Add an element to the end of the queue.
 * @param queue A pointer to the queue.
 * @param element A pointer to the element to add.
 * @return true on success, false if queue or element is NULL.
 */
bool queue_enqueue(struct queue_t* queue, struct coord_t* element);

/**
 * Remove the element at the front of the queue.
 * @param queue A pointer to the queue.
 * @return true if an element was successfully removed, false if the queue is empty or NULL.
 */
bool queue_dequeue(struct queue_t* queue);

/**
 * Get the element at the front of the queue without removing it.
 * @param queue A pointer to the queue.
 * @return A pointer to the head element, or NULL if the queue is empty or NULL.
 */
struct coord_t* queue_peek(struct queue_t* queue);

#endif
