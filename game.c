#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <curses.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>

#include "chess.h"
#include "printboard.h"

// A2 A3

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
  int* pos = malloc(sizeof(char)* 3 * 2)

  fgets(command, 100, stdin);

  while (true) {

    // ask for input
    addstr("Insert command or move: ");
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

/*
 * return 0 if move command is valid, else return 1
*/
int command_to_move(char* command, int* pos) {

  char* token;
  char* pos_str[2];

  // check that strlen is 6
  if (strlen(command) != 6) {
    printf("Invalid move\n");
    return 1;
  }

  // get rid of \n
  command[strlen(command) - 1] = '\0';

  // string split
  pos_str[0] = strsep(&move, " ");
  pos_str[1] = strsep(&move, " ");

  // check if if both position strings composes of an upper case letter and an int
  if (!(isalpha(pos_str[0][0]) && isalpha(pos_str[1][0]) && isdigit(pos_str[0][1]) && isdigit(pos_str[1][1]))) {
    printf("Invalid move\n");
    return 1;
  }

  // make sure both are upper
  pos_str[0][0] = toupper(pos_str[0][0]);
  pos_str[1][0] = toupper(pos_str[1][0]);

  // convert
  int start_col = (int)(pos_str[0][0]) - (int)'A';
  int start_row = (8 - ((int)(pos_str[0][1]) - '0'));

  printf("startpos: %s, endpos: %s\n", pos[0], pos[1]);
  return 0;
}



void save_game(board_t* board) {
  return;
}

void print_saved_games() {
  return;
}

void resume_game(FILE* file) {
  return;
  // start a new game, initialize the board as indicated in the saved file
}
