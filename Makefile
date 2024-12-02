CC := gcc
CFLAGS := -g -Wall -Wno-deprecated-declarations -Werror pkg-config ncursesw
CPACK := pkg-config --libs=ncursesw 

all: chess

clean:
	rm -f chess

chess: game.c printboard.c chess.c chess.h
	$(CC) $(CFLAGS) -o chess game.c chess.c printboard.c -lncurses $(CPACK)