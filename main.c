#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <curses.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>

#include "chess.h"
#include "printboard.h"

int main() {
  setlocale(LC_ALL, "");
  board_t* board = malloc(sizeof(board_t));
  init_board(board);

  initscr();
  cbreak();
  noecho();
  clear();

  printboard(board);

  addstr("\nInsert move: ");
  refresh();
  getch();
  endwin();

  // free everything
  for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
    free(board->cells[i]);
  }
  free(board);
  return 0;
}