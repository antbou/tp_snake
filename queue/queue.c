#include "queue.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct element_t {
    int x, y;
    struct element_t* next;
};

struct queue_t {
    struct element_t* head;
    struct element_t* tail;
};

struct queue_t* queue_create() {
    struct queue_t* queue = malloc(sizeof(struct queue_t));
    if (!queue) {
        fprintf(stderr, "Failed to allocate memory for queue");
        return NULL;
    }
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

bool queue_destroy(struct queue_t** queue) {
    if (!queue || !(*queue)) {
        return false;
    }

    struct element_t* current = (*queue)->head;
    while (current) {
        struct element_t* temp = current;
        current = current->next;
        free(temp);
    }

    free(*queue);
    *queue = NULL;
    return true;
}

bool queue_isEmpty(struct queue_t* queue) {
    return !queue || !queue->head;
}

bool queue_enqueue(struct queue_t* queue, struct element_t* element) {
    if (!queue || !element) {
        return false;
    }

    element->next = NULL;

    if (!queue->head) {
        queue->head = element;
        queue->tail = element;
    } else {
        queue->tail->next = element;
        queue->tail = element;
    }

    return true;
}

bool queue_dequeue(struct queue_t* queue) {
    if (!queue || !queue->head) {
        return false;
    }

    struct element_t* temp = queue->head;
    queue->head = queue->head->next;

    if (!queue->head) {
        queue->tail = NULL;
    }

    free(temp);
    return true;
}

struct element_t* queue_peek(struct queue_t* queue) {
    if (!queue || !queue->head) {
        return NULL;
    }
    return queue->head;
}
