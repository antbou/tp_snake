#ifndef _FOOD_H
#define _FOOD_H

#include "../gfx/gfx.h"
#include "../coord/coord.h"

void spawn_food(struct gfx_context_t* ctxt, int border_offset, int zoom, uint32_t empty_color, uint32_t food_color);

#endif
