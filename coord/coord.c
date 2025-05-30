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