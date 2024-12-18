#include <ncurses.h>
#include <stdlib.h>
#include <curses.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>

#include "chess.h"

/*
 * Returns appropriate color setting
*/
int colorset(int alignment, int background);

/*
 * Prints a board that represents pieces as unicode characters
*/
void printboard(board_t* board, WINDOW* win);

/*
 * initializes game board
*/
void init_board(board_t* board);

/*
 * winning animation
*/
void winning_animate();

/*
 * losing animation
*/
void losing_animate();