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
#include "menu/menu.h"
#include "food/food.h"

#define MAX_FOOD_COUNT 50
#define FOOD_SPAWN_INTERVAL 5000.0 // millisecondes

#define BORDER_OFFSET 16
#define ZOOM 8

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

static double difficulty_to_interval(enum difficulty_level difficulty) {
	switch (difficulty) {
	case EASY: {
		return 100.0;
	}
	case NORMAL: {
		return 60.0;
	}
	case HARD: {
		return 30.0;
	}
	case LEAVE: {
		return 0.0;
	}
	default: {
		return 60.0;
	}
	}
}

static struct gfx_context_t* setup_context(const int width, const int height) {
	if ((width - 2 * BORDER_OFFSET) % ZOOM != 0 ||
		(height - 2 * BORDER_OFFSET) % ZOOM != 0) {
		fprintf(stderr, "Error: screen dimensions must align with zoom (%d).\n", ZOOM);
		return NULL;
	}

	struct gfx_context_t* ctxt = gfx_create("Snake - TP", width, height);
	if (!ctxt) {
		fprintf(stderr, "Graphics initialization failed!\n");
	}
	return ctxt;
}

enum direction get_next_direction(enum direction current_direction) {
	SDL_Keycode key = gfx_keypressed();

	enum direction requested = current_direction;

	switch (key) {
	case SDLK_UP:
	case SDLK_w: {
		requested = up;
		break;
	}
	case SDLK_DOWN:
	case SDLK_s: {
		requested = down;
		break;
	}
	case SDLK_LEFT:
	case SDLK_a: {
		requested = left;
		break;
	}
	case SDLK_RIGHT:
	case SDLK_d: {
		requested = right;
		break;
	}
	default: {
		return current_direction;
	}
	}

	return requested;
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

	struct gfx_context_t* ctxt = setup_context(width, height);
	if (!ctxt) {
		return EXIT_FAILURE;
	}

	bool exit_game = false;
	while (!exit_game) {
		gfx_clear(ctxt, EMPTY);

		enum difficulty_level difficulty = show_start_screen(ctxt);
		if (difficulty == LEAVE) {
			break;
		}

		double snake_move_interval = snake_move_interval = difficulty_to_interval(difficulty);
		srand(time(NULL));

		// Game init
		int x_min = (BORDER_OFFSET / ZOOM) * ZOOM;
		int y_min = (BORDER_OFFSET / ZOOM) * ZOOM;
		int x_max = width - BORDER_OFFSET - ZOOM;
		int y_max = height - BORDER_OFFSET - ZOOM;

		// Place the borders just outside the playable area
		int border_left = x_min - 1;
		int border_right = x_max + ZOOM + 1;
		int border_top = y_min - 1;
		int border_bottom = y_max + ZOOM + 1;
		draw_border(ctxt, border_left, border_right, border_top, border_bottom);

		int playable_width = x_max - x_min;
		int playable_height = y_max - y_min;
		int max_snake_size = (playable_width / ZOOM) * (playable_height / ZOOM);

		struct queue_t* queue = init_snake(x_max, y_max, ZOOM);
		draw_snake_initial(ctxt, queue, ZOOM, SNAKE);

		int food_counter = 1, score = 0;
		spawn_food(ctxt, BORDER_OFFSET, ZOOM, EMPTY, FOOD);

		const double frames_per_second = 60.0;
		const double time_between_frames = 1.0 / frames_per_second * 1e6;

		int last_direction, direction = right;
		struct timespec last_food_time, last_move_time;
		clock_gettime(CLOCK_MONOTONIC, &last_food_time);

		bool first_move = true, done = false;
		while (!done) {
			struct timespec frame_start_time, frame_end_time, current_time;
			clock_gettime(CLOCK_MONOTONIC, &frame_start_time);

			last_direction = direction;
			direction = get_next_direction(direction);

			// Memory leaks occur in gfx_present
			gfx_present(ctxt);
			done = quit_signal();


			bool has_snake_won = queue->size >= max_snake_size;
			if (has_snake_won) {
				printf("You win\n");
				break;
			}

			clock_gettime(CLOCK_MONOTONIC, &current_time);

			// Check if it's time to spawn food
			double time_since_last_food = elapsed_ms(&last_food_time, &current_time);
			bool should_spawn_food = (
				(time_since_last_food >= FOOD_SPAWN_INTERVAL && food_counter < MAX_FOOD_COUNT)
				|| food_counter == 0
				);
			if (should_spawn_food) {
				food_counter++;
				spawn_food(ctxt, BORDER_OFFSET, ZOOM, EMPTY, FOOD);
				clock_gettime(CLOCK_MONOTONIC, &last_food_time);
			}

			// Handles the first move to initialize the timer
			if (first_move) {
				clock_gettime(CLOCK_MONOTONIC, &last_move_time);
				first_move = false;
				continue;
			}

			// Handles the snake movement
			double time_since_last_move = elapsed_ms(&last_move_time, &current_time);
			bool should_move_snake = (time_since_last_move >= snake_move_interval);
			if (should_move_snake) {
				struct coord_t* new_head = new_position(direction, queue->tail, ZOOM);
				bool is_reverse_turn = (last_direction + direction == 3);
				enum collision_type collision = get_collision_type(ctxt, new_head, ZOOM);

				bool hit_wall_or_reverse = (collision == WALL_COLLISION || is_reverse_turn);
				bool hit_self = (collision == SNAKE_COLLISION);
				bool ate_food = (collision == FOOD_COLLISION);
				if (hit_wall_or_reverse) {
					printf("Wall collision or reverse turn detected\n");
					free(new_head);
					break;
				}

				if (hit_self) {
					printf("Snake self-collision detected\n");
					free(new_head);
					break;
				}

				if (ate_food) {
					score += 10;
					printf("Food eaten!\n");
					draw_pixel(ctxt, new_head->x, new_head->y, ZOOM, EMPTY);
					draw_pixel(ctxt, new_head->x, new_head->y, ZOOM, SNAKE);
					queue_enqueue(queue, new_head);
					food_counter--;
				} else {
					move_snake(ctxt, queue, new_head, ZOOM, SNAKE, EMPTY);
				}
				clock_gettime(CLOCK_MONOTONIC, &last_move_time);
			}

			// Handles the FPS limit
			clock_gettime(CLOCK_MONOTONIC, &frame_end_time);
			double frame_duration_ms = elapsed_ms(&frame_start_time, &frame_end_time);
			double sleep_time_for_fps_limit = time_between_frames - frame_duration_ms;

			if (sleep_time_for_fps_limit > 0.0) {
				usleep((int32_t)sleep_time_for_fps_limit);
			}
		}

		queue_destroy(&queue);
		bool play_again = show_end_screen(ctxt, score);
		if (!play_again) {
			exit_game = true;
		}
	}
	gfx_destroy(ctxt);
	return EXIT_SUCCESS;
}
