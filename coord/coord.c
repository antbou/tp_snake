#include "coord.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

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

void coord_destroy(struct coord_t* head) {
    struct coord_t* current = head;
    while (current != NULL) {
        struct coord_t* next = current->next;
        free(current);
        current = next;
    }
}

void coord_add(struct coord_t** head, struct coord_t* new_element) {
    if (!new_element) {
        return;
    }
    new_element->next = *head;
    *head = new_element;
}