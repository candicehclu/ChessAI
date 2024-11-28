CC := clang
CFLAGS := -g -Wall -Wno-deprecated-declarations -Werror

all: chess

clean:
	rm -f chess

chess: main.c printboard.c chess.c chess.h
	$(CC) $(CFLAGS) -o chess main.c chess.c printboard.c -lncurses