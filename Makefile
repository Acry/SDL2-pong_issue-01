CC=g++
CFLAGS   = -Wall -Wextra -mtune=native `sdl2-config --cflags`
LDFLAGS  = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx -lm

debug: main.cpp
	$(CC) $(CFLAGS) -g $? -o pong $(LDFLAGS)

clean:
	rm -vfr *~ pong
