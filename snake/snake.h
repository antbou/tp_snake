#ifndef _SNAKE_H_
#define _SNAKE_H_

#include "../coord/coord.h"

enum direction {
    left,
    up,
    down,
    right
};

struct queue_t* init_snake(int width, int height);
struct coord_t* new_position(enum direction dir, struct coord_t* element);

#endif