#ifndef _MENU_H
#define _MENU_H

#include "../gfx/gfx.h"

enum difficulty_level {
    EASY,
    NORMAL,
    HARD,
    LEAVE,
};

enum difficulty_level show_start_screen(struct gfx_context_t* ctxt);
bool show_end_screen(struct gfx_context_t* ctxt, int const score);

#endif
