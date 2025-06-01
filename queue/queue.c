#include "queue.h"

#include <stdlib.h>
#include <stdio.h>

struct queue_t* queue_create() {
    struct queue_t* queue = malloc(sizeof(struct queue_t));
    if (!queue) {
        fprintf(stderr, "Failed to allocate memory for queue");
        return NULL;
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    return queue;
}

bool queue_destroy(struct queue_t** queue) {
    if (!queue || !(*queue)) {
        return false;
    }

    bool success = coord_list_destroy(&(*queue)->head);

    (*queue)->tail = NULL;
    free(*queue);
    *queue = NULL;

    return success;
}

bool queue_isEmpty(struct queue_t* queue) {
    return !queue || !queue->head;
}

bool queue_enqueue(struct queue_t* queue, struct coord_t* element) {
    if (!queue || !element) {
        return false;
    }
    element->next = NULL;
    if (!queue->head) {
        queue->head = element;
        queue->tail = element;
    } else {
        // 1. links the current last element to the new one, extending the chain
        queue->tail->next = element;
        // 2. updates the tail pointer to mark the new element as the last in the queue
        queue->tail = element;
    }
    queue->size++;
    return true;
}

bool queue_dequeue(struct queue_t* queue) {
    if (!queue || !queue->head) {
        return false;
    }

    struct coord_t* temp = queue->head;
    queue->head = queue->head->next;

    if (!queue->head) {
        queue->tail = NULL;
    }
    queue->size--;

    free(temp);
    return true;
}

struct coord_t* queue_peek(struct queue_t* queue) {
    if (!queue || !queue->head) {
        return NULL;
    }
    return queue->head;
}
