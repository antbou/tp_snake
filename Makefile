CC      = gcc -std=gnu11
CFLAGS  = -Wall -Wextra -pedantic -g
LDLIBS  = -lSDL2 -lSDL2_ttf
LDFLAGS = -fsanitize=address -fsanitize=leak -fsanitize=undefined

.PHONY: clean run

main: main.o gfx.o snake.o queue.o coord.o menu.o food.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS) $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c $<

gfx.o: gfx/gfx.c gfx/gfx.h
	$(CC) $(CFLAGS) $< -c

snake.o: snake/snake.c snake/snake.h queue/queue.h coord/coord.h
	$(CC) $(CFLAGS) $< -c

queue.o: queue/queue.c queue/queue.h
	$(CC) $(CFLAGS) $< -c

coord.o: coord/coord.c coord/coord.h
	$(CC) $(CFLAGS) $< -c

menu.o: menu/menu.c menu/menu.h gfx/gfx.h
	$(CC) $(CFLAGS) $< -c

food.o: food/food.c food/food.h gfx/gfx.h
	$(CC) $(CFLAGS) $< -c

run: main
	./main 3 30

clean:
	rm -f main *.o
