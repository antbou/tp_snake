#ifndef _SNAKE_H_
#define _SNAKE_H_

enum direction {
    left,
    up,
    down,
    right
};

struct queue_t* init_snake(int width, int height);
struct element_t* new_position(enum direction dir, struct element_t* element);

#endif