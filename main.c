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

#define MAX_FOOD_COUNT 50
#define FOOD_SPAWN_INTERVAL 5000.0 // millisecondes
#define SNAKE_MOVE_INTERVAL 70.0  // millisecondes

#define BORDER_OFFSET 16
#define ZOOM 8

enum screen_color_state {
	EMPTY = COLOR_BLACK,
	SNAKE = COLOR_WHITE,
	FOOD = COLOR_RED,
	WALL = COLOR_BLUE
};

enum collision_type {
	NO_COLLISION,
	WALL_COLLISION,
	SNAKE_COLLISION,
	FOOD_COLLISION
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
		draw_pixel(context, current->x, current->y, ZOOM, SNAKE);
		current = current->next;
	}
}

static void draw_food(struct gfx_context_t* context, struct coord_t* food) {
	draw_pixel(context, food->x, food->y, ZOOM, FOOD);
}


struct coord_t* generate_food_coord(struct gfx_context_t* context) {
	// Align the minimum coordinates to the ZOOM grid
	const int x_min = ((BORDER_OFFSET + ZOOM - 1) / ZOOM) * ZOOM;
	const int y_min = ((BORDER_OFFSET + ZOOM - 1) / ZOOM) * ZOOM;
	const int x_max = context->width - BORDER_OFFSET;
	const int y_max = context->height - BORDER_OFFSET;

	int x_range = (x_max - x_min) / ZOOM;
	int y_range = (y_max - y_min) / ZOOM;

	if (x_range <= 0 || y_range <= 0) {
		fprintf(stderr, "Error: playable area too small to place food.\n");
		return NULL;
	}

	struct coord_t* food = coord_init(x_min, y_min);
	if (!food) {
		return NULL;
	}

	do {
		food->x = (rand() % x_range) * ZOOM + x_min;
		food->y = (rand() % y_range) * ZOOM + y_min;
	} while (gfx_getpixel(context, food->x, food->y) != EMPTY);

	return food;
}

static void move_snake(struct gfx_context_t* context, struct queue_t* queue, struct coord_t* new_pos) {
	draw_pixel(context, new_pos->x, new_pos->y, ZOOM, SNAKE);
	queue_enqueue(queue, new_pos);

	// Get tail position before removing it
	struct coord_t* old_tail = queue->head;
	int tail_x = old_tail->x;
	int tail_y = old_tail->y;

	// Remove tail
	queue_dequeue(queue);
	draw_pixel(context, tail_x, tail_y, ZOOM, EMPTY);
}

/**
 * Determines the type of collision based on the pixel content
 * at the given coordinate in the context.
 *
 * @param context the graphics context
 * @param pos the position to evaluate
 * @return the corresponding collision type (wall, snake, food, or none)
 */
static enum collision_type get_collision_type(struct gfx_context_t* context, struct coord_t* pos) {
	const int zoom = ZOOM;
	for (int ix = 0; ix < zoom; ix++) {
		for (int iy = 0; iy < zoom; iy++) {
			uint32_t pixel = gfx_getpixel(context, pos->x + ix, pos->y + iy);
			switch (pixel) {
			case WALL:
				return WALL_COLLISION;
			case SNAKE:
				return SNAKE_COLLISION;
			case FOOD:
				return FOOD_COLLISION;
			default:
				break;
			}
		}
	}
	return NO_COLLISION;
}


/**
 * Compute the time elapsed between two timespec structs in milliseconds.
 *
 * - tv_sec is in seconds → multiplied by 1000.0 to convert to milliseconds.
 * - tv_nsec is in nanoseconds → divided by 1.0e6 to convert to milliseconds.
 *
 * Example:
 *   2 sec and 500,000,000 ns → 2000 + 500 = 2500 ms
 */
double elapsed_ms(struct timespec* start, struct timespec* end) {
	return (end->tv_sec - start->tv_sec) * 1000.0 +          // seconds → ms
		(end->tv_nsec - start->tv_nsec) / 1.0e6;            // nanoseconds → ms
}

int main(void) {
	const int width = 1920;
	const int height = 1080;

	if ((width - 2 * BORDER_OFFSET) % ZOOM != 0 ||
		(height - 2 * BORDER_OFFSET) % ZOOM != 0) {
		fprintf(stderr, "Error: screen dimensions must align with zoom (%d).\n", ZOOM);
		return EXIT_FAILURE;
	}

	struct gfx_context_t* ctxt = gfx_create("Snake - TP", width, height);
	if (!ctxt) {
		fprintf(stderr, "Graphics initialization failed!\n");
		return EXIT_FAILURE;
	}

	srand(time(NULL));
	gfx_clear(ctxt, EMPTY);

	int x_min = (BORDER_OFFSET / ZOOM) * ZOOM;
	int x_max = (width - BORDER_OFFSET) / ZOOM * ZOOM;
	int y_min = (BORDER_OFFSET / ZOOM) * ZOOM;
	int y_max = (height - BORDER_OFFSET) / ZOOM * ZOOM;

	int border_left = x_min + ZOOM - 1;
	int border_right = x_max - ZOOM;
	int border_top = y_min + ZOOM - 1;
	int border_bottom = y_max - ZOOM;
	draw_border(ctxt, border_left, border_right, border_top, border_bottom);



	int playable_width = x_max - x_min;
	int playable_height = y_max - y_min;
	int max_snake_size = (playable_width / ZOOM) * (playable_height / ZOOM);

	int last_direction, direction = right;
	struct queue_t* queue = init_snake(x_max, y_max, ZOOM);
	// Draw initial snake
	draw_snake_initial(ctxt, queue);

	struct coord_t* new_food = generate_food_coord(ctxt);
	draw_food(ctxt, new_food);
	free(new_food);

	int food_counter = 1;

	const double frames_per_second = 60.0;
	const double time_between_frames = 1.0 / frames_per_second * 1e6;

	bool done = false;

	struct timespec last_food_time;
	clock_gettime(CLOCK_MONOTONIC, &last_food_time);

	static struct timespec last_move_time;
	static bool first_move = true;
	while (!done) {
		struct timespec frame_start_time, frame_end_time, current_time;
		clock_gettime(CLOCK_MONOTONIC, &frame_start_time);

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
		bool has_snake_won = queue->size >= max_snake_size;

		if (has_snake_won) {
			printf("You win\n");
			break;
		}
		clock_gettime(CLOCK_MONOTONIC, &current_time);
		double time_since_last_food = elapsed_ms(&last_food_time, &current_time);
		if ((time_since_last_food >= FOOD_SPAWN_INTERVAL && food_counter < MAX_FOOD_COUNT) || food_counter == 0) {
			food_counter++;
			struct coord_t* new_food = generate_food_coord(ctxt);
			draw_food(ctxt, new_food);
			free(new_food);

			clock_gettime(CLOCK_MONOTONIC, &last_food_time);
		}

		if (first_move) {
			clock_gettime(CLOCK_MONOTONIC, &last_move_time);
			first_move = false;
			continue;
		}

		double time_since_last_move = elapsed_ms(&last_move_time, &current_time);
		if (time_since_last_move >= SNAKE_MOVE_INTERVAL) {
			bool is_reverse_turn = (last_direction + direction == 3);
			struct coord_t* new_head = new_position(direction, queue->tail, ZOOM);

			enum collision_type collision = get_collision_type(ctxt, new_head);
			if (collision == WALL_COLLISION || is_reverse_turn) {
				printf("Wall collision or reverse turn detected\n");
				break;
			}

			if (collision == SNAKE_COLLISION) {
				printf("Snake self-collision detected\n");
				break;
			}

			if (collision == FOOD_COLLISION) {
				printf("Food eaten!\n");
				draw_pixel(ctxt, new_head->x, new_head->y, ZOOM, EMPTY);
				draw_pixel(ctxt, new_head->x, new_head->y, ZOOM, SNAKE);
				queue_enqueue(queue, new_head);
				food_counter--;
			} else {
				move_snake(ctxt, queue, new_head);
			}
			clock_gettime(CLOCK_MONOTONIC, &last_move_time);
		}

		clock_gettime(CLOCK_MONOTONIC, &frame_end_time);
		double frame_duration_ms = elapsed_ms(&frame_start_time, &frame_end_time);
		double sleep_time_for_fps_limit = time_between_frames - frame_duration_ms;

		if (sleep_time_for_fps_limit > 0.0) {
			usleep((int32_t)sleep_time_for_fps_limit);
		}
	}

	queue_destroy(&queue);
	gfx_destroy(ctxt);
	return EXIT_SUCCESS;
}
