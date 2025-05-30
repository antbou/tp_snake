
#ifndef _COORD_H_
#define _COORD_H_

/**
 * Coordinates x, y, with linked list
 */
struct coord_t {
    int x, y;
    struct coord_t* next;
};

struct coord_t* coord_init(int x, int y);
#endif