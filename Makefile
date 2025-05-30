CC      = gcc -std=gnu11
CFLAGS  = -Wall -Wextra -pedantic -g
LDLIBS  = -lSDL2
LDFLAGS = -fsanitize=address -fsanitize=leak -fsanitize=undefined

.PHONY: clean run all

all: main

main: main.o gfx.o snake.o queue.o coord.o menu.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS) $(LDFLAGS)

main.o: main.c gfx/gfx.c
	$(CC) $(CFLAGS) $< -c

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

run: main
	./main

clean:
	rm -f main *.o gfx/*.o queue/*.o
