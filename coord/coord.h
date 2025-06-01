
#ifndef _COORD_H_
#define _COORD_H_

#include <stdbool.h>

/**
 * Coordinates x, y, with linked list
 */
struct coord_t {
    int x, y;
    struct coord_t* next;
};
/**
 * Allocate and initialize a new coordinate node.
 *
 * @param x The x-coordinate value.
 * @param y The y-coordinate value.
 * @return A pointer to the newly allocated coord_t struct, or NULL if allocation fails.
 */
struct coord_t* coord_init(int x, int y);

/**
 * Frees an entire singly linked list of coord_t elements.
 *
 * @param head A pointer to the pointer to the head of the list.
 * @return true if the list was successfully destroyed, false if the input was invalid.
 */
bool coord_list_destroy(struct coord_t** head);

#endif