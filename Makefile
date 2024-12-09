CC := gcc
CFLAGS := -g -Wall -Wno-deprecated-declarations 
CPACK := pkg-config --libs=ncursesw 

all: chess

clean:
	rm -f chess

chess: game.c printboard.c chess.c chess.h
	$(CC) $(CFLAGS) `pkg-config --cflags ncursesw` -o chess game.c chess.c printboard.c -lncurses `pkg-config --libs ncursesw`
	