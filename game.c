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

  // malloc for command
  char* command = malloc(sizeof(char)*100);

  fgets(command, 100, stdin);

  while (true) {

    // ask for input
    addstr("Insert command or move: ")
    refresh();
    fgets(command, 100, stdin);

    // check for commands
    if (strcmp(command, "quit") == 0) break;
    if (strcmp(command, "save") == 0) save_game(board);
    if (strcmp(command, "resume") == 0) {
      print_saved_games();
      // do sth else
    }    
    // if it is a valid move, process input string and call move_piece
    // else print error
  }
  endwin();

  // free everything
  for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
    free(board->cells[i]);
  }
  free(board);
  return 0;
}

void save_game(board_t* board) {
  
}

void print_saved_games() {

}

void resume_game(FILE* file) {
  // start a new game, initialize the board as indicated in the saved file
}