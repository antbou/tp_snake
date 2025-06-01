#include "coord.h"

#include <stdlib.h>
#include <stdio.h>

struct coord_t* coord_init(int x, int y) {
    struct coord_t* element = malloc(sizeof(struct coord_t));
    if (!element) {
        fprintf(stderr, "Failed to allocate memory for element");
        return NULL;
    }
    element->x = x;
    element->y = y;
    element->next = NULL;
    return element;
}

bool coord_list_destroy(struct coord_t** head) {
    if (!head || !*head) {
        return false;
    }

    struct coord_t* current = *head;
    while (current) {
        struct coord_t* next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
    return true;
}