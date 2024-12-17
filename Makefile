CC := gcc
CFLAGS := -Wall -g
CPACK := pkg-config --libs=ncursesw 

all: chess

clean:
	rm -f chess

chess: game.c printboard.c chess.c chess.h ai.c ai.h
	$(CC) $(CFLAGS) `pkg-config --cflags ncursesw` -o chess game.c printboard.c chess.c ai.c `pkg-config --libs ncursesw`