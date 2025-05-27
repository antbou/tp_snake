// Enable POSIX features clock_gettime for precise time measurement: https://stackoverflow.com/questions/48332332/what-does-define-posix-source-mean
#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "gfx/gfx.h"
#include "snake/snake.h"
#include "queue/queue.h"
#include "coord/coord.h"

#define MAX_FOOD_COUNT 5
#define FOOD_SPAWN_INTERVAL 5000.0

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

static void draw_food(struct gfx_context_t* context, struct coord_t* food) {
	draw_pixel(context, food->x, food->y, 4, FOOD);
}


struct coord_t* generate_food_coord(struct gfx_context_t* context) {
	const int x_min = 12, x_max = context->width - 12;
	const int y_min = 12, y_max = context->height - 12;
	const int zoom = 4;

	struct coord_t* food = coord_init(x_min, y_min);
	if (!food) {
		return NULL;
	}

	do {
		food->x = (rand() % ((x_max - x_min) / zoom)) * zoom + x_min;
		food->y = (rand() % ((y_max - y_min) / zoom)) * zoom + y_min;
	} while (gfx_getpixel(context, food->x, food->y) != EMPTY);

	return food;
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

static bool is_food_detected(struct gfx_context_t* context, struct coord_t* pos) {
	const int zoom = 4;
	for (int ix = 0; ix < zoom; ix++) {
		for (int iy = 0; iy < zoom; iy++) {
			if (gfx_getpixel(context, pos->x + ix, pos->y + iy) == FOOD) {
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

	srand(time(NULL));
	gfx_clear(ctxt, EMPTY);
	draw_border(ctxt, 8, width - 8, 8, height - 8);

	struct queue_t* queue = init_snake(width, height);
	int last_direction, direction = right;

	// Draw initial snake
	draw_snake_initial(ctxt, queue);

	struct coord_t* food = generate_food_coord(ctxt);
	draw_food(ctxt, food);
	int food_counter = 1;

	const double frames_per_second = 24.0;
	const double time_between_frames = 1.0 / frames_per_second * 1e6;

	bool done = false;

	struct timespec start, finish;
	clock_gettime(CLOCK_MONOTONIC, &start);

	struct timespec last_food_spawn_time;
	clock_gettime(CLOCK_MONOTONIC, &last_food_spawn_time);

	while (!done) {
		last_direction = direction;

		switch (gfx_keypressed()) {
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

		gfx_present(ctxt);
		done = quit_signal();

		struct coord_t* new_head = new_position(direction, queue->tail);
		bool is_reverse_turn = (last_direction + direction == 3);
		if (is_wall_detected(ctxt, new_head) || is_reverse_turn) {
			break;
		}
		if (is_food_detected(ctxt, new_head)) {
			printf("Food eaten!\n");

			coord_remove_at(&food, new_head->x, new_head->y);
			draw_pixel(ctxt, new_head->x, new_head->y, 4, SNAKE);
			queue_enqueue(queue, new_head);
		} else {
			move_snake(ctxt, queue, new_head);
		}

		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);

		double time_since_last_food = (now.tv_sec - last_food_spawn_time.tv_sec) * 1000.0;
		time_since_last_food += (now.tv_nsec - last_food_spawn_time.tv_nsec) / 1.0e6;

		if (time_since_last_food >= FOOD_SPAWN_INTERVAL && food_counter < MAX_FOOD_COUNT) {
			food_counter++;
			struct coord_t* new_food = generate_food_coord(ctxt);
			draw_food(ctxt, new_food);
			coord_add(&food, new_food);
			clock_gettime(CLOCK_MONOTONIC, &last_food_spawn_time);
		}

		clock_gettime(CLOCK_MONOTONIC, &finish);
		double mu_seconds_elapsed = finish.tv_sec - start.tv_sec;
		mu_seconds_elapsed += (finish.tv_nsec - start.tv_nsec) / 1.0e6;

		double remaining_time = time_between_frames - mu_seconds_elapsed;
		if (remaining_time > 0.0) {
			usleep((int32_t)remaining_time);
		}

		clock_gettime(CLOCK_MONOTONIC, &start);
	}

	coord_destroy(food);
	queue_destroy(&queue);
	gfx_destroy(ctxt);
	return EXIT_SUCCESS;
}
