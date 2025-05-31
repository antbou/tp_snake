#ifndef _FOOD_H
#define _FOOD_H

#include "../gfx/gfx.h"
#include "../coord/coord.h"

struct coord_t* generate_food(struct gfx_context_t* context, int border_offset, int zoom, uint32_t empty_color);

#endif
