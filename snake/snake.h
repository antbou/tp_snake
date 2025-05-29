#ifndef _SNAKE_H_
#define _SNAKE_H_

#include "../coord/coord.h"

enum direction {
    left,
    up,
    down,
    right
};

struct queue_t* init_snake(const int width, const int height, const int zoom);
struct coord_t* new_position(enum direction dir, struct coord_t* element, const int zoom);

#endif