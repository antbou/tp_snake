/// @file gfx.c
/// @author Florent Gluck
/// @date November 6, 2016
/// Helper routines to render pixels in fullscreen graphic mode.
/// Uses the SDL2 library.
/// Update : Add full circle
/// @author Michaël El Kharroubi
/// @date 19 November 2021
/// Update : Add better system to quit window
/// @author Michaël El Kharroubi
/// @date November 2024

#include "gfx.h"

#include <assert.h>
#include <SDL2/SDL_ttf.h>

/// Create a fullscreen graphic window.
/// @param title Title of the window.
/// @param width Width of the window in pixels.
/// @param height Height of the window in pixels.
/// @return a pointer to the graphic context or NULL if it failed.
struct gfx_context_t* gfx_create(char* title, uint32_t width, uint32_t height) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		goto error;
	SDL_Window* window = SDL_CreateWindow(
		title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
		SDL_WINDOW_RESIZABLE);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Texture* texture = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	uint32_t* pixels = malloc(width * height * sizeof(uint32_t));
	struct gfx_context_t* ctxt = malloc(sizeof(struct gfx_context_t));
	if (TTF_Init() == -1)
		goto error;

	if (!window || !renderer || !texture || !pixels || !ctxt)
		goto error;

	ctxt->renderer = renderer;
	ctxt->texture = texture;
	ctxt->window = window;
	ctxt->width = width;
	ctxt->height = height;
	ctxt->pixels = pixels;

	SDL_ShowCursor(SDL_DISABLE);
	gfx_clear(ctxt, COLOR_BLACK);
	return ctxt;

error:
	return NULL;
}

/// Draw a pixel in the specified graphic context.
/// @param ctxt Graphic context where the pixel is to be drawn.
/// @param column X coordinate of the pixel.
/// @param row Y coordinate of the pixel.
/// @param color Color of the pixel.
void gfx_putpixel(struct gfx_context_t* ctxt, uint32_t column, uint32_t row, uint32_t color) {
	if (column < ctxt->width && row < ctxt->height)
		ctxt->pixels[ctxt->width * row + column] = color;
}

/// Get a pixel in the specified graphic context.
/// @param ctxt Graphic context where we want to get the pixel.
/// @param x X coordinate of the pixel.
/// @param y Y coordinate of the pixel.
/// @param color Color of the pixel.
/// @return color at position x, y.
uint32_t gfx_getpixel(struct gfx_context_t* ctxt, int x, int y) {
	assert(x >= 0 && x < (int)ctxt->width && y >= 0 && y < (int)ctxt->height);
	return ctxt->pixels[ctxt->width * y + x];
}

/// Clear the specified graphic context.
/// @param ctxt Graphic context to clear.
/// @param color Color to use.
void gfx_clear(struct gfx_context_t* ctxt, uint32_t color) {
	int n = ctxt->width * ctxt->height;
	while (n)
		ctxt->pixels[--n] = color;
	SDL_RenderClear(ctxt->renderer);
}

/// Display the graphic context.
/// @param ctxt Graphic context to clear.
void gfx_present(struct gfx_context_t* ctxt) {
	SDL_UpdateTexture(ctxt->texture, NULL, ctxt->pixels, ctxt->width * sizeof(uint32_t));
	SDL_RenderCopy(ctxt->renderer, ctxt->texture, NULL, NULL);
	SDL_RenderPresent(ctxt->renderer);
}

/// Destroy a graphic window.
/// @param ctxt Graphic context of the window to close.
void gfx_destroy(struct gfx_context_t* ctxt) {
	SDL_ShowCursor(SDL_ENABLE);
	SDL_DestroyTexture(ctxt->texture);
	SDL_DestroyRenderer(ctxt->renderer);
	SDL_DestroyWindow(ctxt->window);
	free(ctxt->pixels);
	ctxt->texture = NULL;
	ctxt->renderer = NULL;
	ctxt->window = NULL;
	ctxt->pixels = NULL;
	TTF_Quit();
	SDL_Quit();
	free(ctxt);
}

/// If a key was pressed, returns its key code (non blocking call).
/// List of key codes: https://wiki.libsdl.org/SDL_Keycode
/// @return the key that was pressed or 0 if none was pressed.
SDL_Keycode gfx_keypressed() {
	SDL_Event event;
	if (SDL_PollEvent(&event)) {
		if (event.type == SDL_KEYDOWN)
			return event.key.keysym.sym;
	}
	return 0;
}

/// Check for quit signals such as: alt-f4, ctrl+c, ctrl+d or ESC
//  @return true if there is a quit signal
bool quit_signal() {
	bool signal = false;
	// process events
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:  // signal when window "X" icon pressed
			signal = true;
			break;
		case SDL_KEYDOWN:
			if ((event.key.keysym.mod & KMOD_CTRL) != 0) {
				SDL_Keycode k = event.key.keysym.sym;
				signal = (k == SDLK_c) || (k == SDLK_d);  // ctrl-c or ctrl-d pressed
			}
			break;
		case SDL_KEYUP:
			// escape pressed
			signal = event.key.keysym.scancode == SDL_SCANCODE_ESCAPE;
			break;
		default:
			break;
		}
	}
	return signal;
}

/// Wait for alt-f4, ctrl+c, ctrl+d or ESC
//  Blocking call.
void wait_for_quit_signal() {
	while (!quit_signal()) {
	}
}

void draw_pixel(struct gfx_context_t* context, int x, int y, int zoom, uint32_t color) {
	for (int ix = 0; ix < zoom; ix++) {
		for (int iy = 0; iy < zoom; iy++) {
			gfx_putpixel(context, x + ix, y + iy, color);
		}
	}
}

// https://stackoverflow.com/questions/22886500/how-to-render-text-in-sdl2
void draw_text_ttf(struct gfx_context_t* ctxt, const char* text, int x, int y, int size, SDL_Color color, const char* font_path) {
	TTF_Font* font = TTF_OpenFont(font_path, size);
	if (!font) {
		fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
		return;
	}

	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	if (!surface) {
		fprintf(stderr, "Failed to render text: %s\n", TTF_GetError());
		TTF_CloseFont(font);
		return;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(ctxt->renderer, surface);
	SDL_FreeSurface(surface);
	if (!texture) {
		fprintf(stderr, "Failed to create texture: %s\n", SDL_GetError());
		TTF_CloseFont(font);
		return;
	}

	SDL_Rect dst = { x, y, 0, 0 };
	SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
	SDL_RenderCopy(ctxt->renderer, texture, NULL, &dst);

	SDL_DestroyTexture(texture);
	TTF_CloseFont(font);
}

void draw_border(struct gfx_context_t* context, int x0, int x1, int y0, int y1, uint32_t wall) {
	for (int ix = x0; ix < x1; ++ix) {
		gfx_putpixel(context, ix, y0, wall);
	}
	for (int ix = x0; ix < x1; ++ix) {
		gfx_putpixel(context, ix, y1 - 1, wall);
	}
	for (int iy = y0; iy < y1; ++iy) {
		gfx_putpixel(context, x0, iy, wall);
	}
	for (int iy = y0; iy < y1; ++iy) {
		gfx_putpixel(context, x1 - 1, iy, wall);
	}
}