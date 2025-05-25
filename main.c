// Enable POSIX features clock_gettime for precise time measurement: https://stackoverflow.com/questions/48332332/what-does-define-posix-source-mean
#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "gfx/gfx.h"
#include "snake/snake.h"
#include "queue/queue.h"
#include "coord/coord.h"

#define max_food_count 5
#define food_spawn_interval 1000000 // in microseconds

enum screen_color_state {
	EMPTY = COLOR_BLACK,
	SNAKE = COLOR_WHITE,
	FOOD = COLOR_RED,
	WALL = COLOR_BLUE
};

static void draw_border(struct gfx_context_t* context, int x0, int x1, int y0, int y1) {
	for (int ix = x0; ix < x1; ++ix) {
		gfx_putpixel(context, ix, y0, WALL);
	}
	for (int ix = x0; ix < x1; ++ix) {
		gfx_putpixel(context, ix, y1 - 1, WALL);
	}
	for (int iy = y0; iy < y1; ++iy) {
		gfx_putpixel(context, x0, iy, WALL);
	}
	for (int iy = y0; iy < y1; ++iy) {
		gfx_putpixel(context, x1 - 1, iy, WALL);
	}
}

static void draw_pixel(struct gfx_context_t* context, int x, int y, int zoom, uint32_t color) {
	for (int ix = 0; ix < zoom; ix++) {
		for (int iy = 0; iy < zoom; iy++) {
			gfx_putpixel(context, x + ix, y + iy, color);
		}
	}
}

static void draw_snake_initial(struct gfx_context_t* context, struct queue_t* queue) {
	struct coord_t* current = queue->head;
	while (current != NULL) {
		draw_pixel(context, current->x, current->y, 4, SNAKE);
		current = current->next;
	}
}

static void move_snake(struct gfx_context_t* context, struct queue_t* queue, struct coord_t* new_pos) {
	draw_pixel(context, new_pos->x, new_pos->y, 4, SNAKE);
	queue_enqueue(queue, new_pos);

	// Get tail position before removing it
	struct coord_t* old_tail = queue->head;
	int tail_x = old_tail->x;
	int tail_y = old_tail->y;

	// Remove tail
	queue_dequeue(queue);
	draw_pixel(context, tail_x, tail_y, 4, EMPTY);
}

static bool is_wall_detected(struct gfx_context_t* context, struct coord_t* new_pos) {
	const int zoom = 4;
	for (int ix = 0; ix < zoom; ix++) {
		for (int iy = 0; iy < zoom; iy++) {
			if (gfx_getpixel(context, new_pos->x + ix, new_pos->y + iy) == WALL) {
				return true;
			}
		}
	}
	return false;
}

int main(void) {
	const int width = 1920;
	const int height = 1080;
	struct gfx_context_t* ctxt = gfx_create("Snake - TP", width, height);
	if (!ctxt) {
		fprintf(stderr, "Graphics initialization failed!\n");
		return EXIT_FAILURE;
	}
	gfx_clear(ctxt, EMPTY);
	// l'écran de jeu est entouré d'une paroi
	draw_border(ctxt, 10, width - 10, 10, height - 10);

	// le serpent démarre avec une longueur de 3
	struct queue_t* queue = init_snake(width, height);
	// le serpent démarre en étant orienté vers la droite au milieu de l'écran de jeu;

	// Draw initial snake
	draw_snake_initial(ctxt, queue);

	SDL_Keycode key = gfx_keypressed();
	double frames_per_second = 24.0;
	double time_between_frames = 1.0 / frames_per_second * 1e6; // in micro seconds

	int direction = right;
	int last_direction = last_direction;

	bool done = false;
	// la partie se termine par une victoire si le serpent occupe tout l'écran
	while (!done) {
		last_direction = direction;
		struct timespec start, finish;
		clock_gettime(CLOCK_MONOTONIC, &start);

		key = gfx_keypressed();

		switch (key) {
		case SDLK_UP:
			direction = up;
			printf("Up pressed\n");
			break;
		case SDLK_DOWN:
			direction = down;
			printf("Down pressed\n");
			break;
		case SDLK_LEFT:
			direction = left;
			printf("Left pressed\n");
			break;
		case SDLK_RIGHT:
			direction = right;
			printf("Right pressed\n");
			break;
		default:
			break;
		}

		// il y a un nombre maximal, N2, de nourriture qui apparait à des endroits aléatoires de la carte toutes les M3 secondes ;
		gfx_present(ctxt);
		done = quit_signal();

		// Move and update snake display
		struct coord_t* new_head = new_position(direction, queue->tail);
		if (is_wall_detected(ctxt, new_head)) {
			break;
		}
		// demi tour immédiat
		if (last_direction + direction == 3) {
			break;
		}
		move_snake(ctxt, queue, new_head);

		clock_gettime(CLOCK_MONOTONIC, &finish);
		double mu_seconds_elapsed = finish.tv_sec - start.tv_sec;
		mu_seconds_elapsed += (finish.tv_nsec - start.tv_nsec) / 1.0e6;
		double remaining_time = time_between_frames - mu_seconds_elapsed;
		if (remaining_time > 0.0) {
			usleep((int32_t)remaining_time);
		}
	}

	queue_destroy(&queue);
	gfx_destroy(ctxt);
	return EXIT_SUCCESS;
}
