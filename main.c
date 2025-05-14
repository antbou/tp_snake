#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "gfx/gfx.h"

static void draw_rectangle(struct gfx_context_t* context, int x0, int x1, int y0, int y1) {
	for (int ix = x0; ix < x1; ++ix) {
		gfx_putpixel(context, ix, y0, COLOR_WHITE);
	}
	for (int ix = x0; ix < x1; ++ix) {
		gfx_putpixel(context, ix, y1 - 1, COLOR_WHITE);
	}
	for (int iy = y0; iy < y1; ++iy) {
		gfx_putpixel(context, x0, iy, COLOR_WHITE);
	}
	for (int iy = y0; iy < y1; ++iy) {
		gfx_putpixel(context, x1 - 1, iy, COLOR_WHITE);
	}
}

static void draw_pixel(struct gfx_context_t* context, int x, int y, int zoom) {
	for (int ix = 0; ix < zoom; ix++) {
		for (int iy = 0; iy < zoom; iy++) {
			gfx_putpixel(context, x + ix, y + iy, COLOR_WHITE);
		}
	}
}

int main() {
	int width = 240;
	int height = 135;
	struct gfx_context_t* ctxt = gfx_create("Serpent", width, height);
	if (!ctxt) {
		fprintf(stderr, "Graphics initialization failed!\n");
		return EXIT_FAILURE;
	}
	gfx_clear(ctxt, COLOR_BLACK);
	draw_rectangle(ctxt, 10, 230, 10, 125);
	draw_pixel(ctxt, 40, 50, 4);

	SDL_Keycode key = gfx_keypressed();
	double frames_per_second = 24.0;
	double time_between_frames = 1.0 / frames_per_second * 1e6; // in micro seconds

	bool done = false;
	while (!done) {
		struct timespec start, finish;
		clock_gettime(CLOCK_MONOTONIC, &start);

		key = gfx_keypressed();

		switch (key) {
		case SDLK_UP:
			printf("Up pressed\n");
			break;
		case SDLK_DOWN:
			printf("Down pressed\n");
			break;
		case SDLK_LEFT:
			printf("Left pressed\n");
			break;
		case SDLK_RIGHT:
			printf("Right pressed\n");
			break;
		default:
			break;
		}
		gfx_present(ctxt);
		done = quit_signal();
		clock_gettime(CLOCK_MONOTONIC, &finish);
		double mu_seconds_elapsed = finish.tv_sec - start.tv_sec;
		mu_seconds_elapsed += (finish.tv_nsec - start.tv_nsec) / 1.0e6;

		double remaining_time = time_between_frames - mu_seconds_elapsed;

		if (remaining_time > 0.0) {
			usleep((int32_t)remaining_time);
		}
	}
	gfx_destroy(ctxt);
	return EXIT_SUCCESS;
}
