#include <ncurses.h>
#include <stdlib.h>
// #include <ncursesw/ncurses.h>
#include <curses.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>

#include "chess.h"

#define BOARD_DIM 8

char pieces_letter[] = {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'};

/**
 * Initializes board to starting state
 *
 * ALIGNMENT KEY:
 * 0 = EMPTY
 * 1 = BLACK PIECE
 * 2 = WHITE PIECE
 */

void simpleprinting() {
  if (has_colors()) {
    if (start_color() == OK) {

      // reset colors, set yellow as grey
      init_color(COLOR_GREEN, 300, 464, 302);
      init_color(COLOR_YELLOW, 900, 900, 900);

      // black piece on white
      init_pair(1, COLOR_BLACK, COLOR_YELLOW);
      // black piece on green
      init_pair(2, COLOR_BLACK, COLOR_GREEN);
      // white piece on white
      init_pair(3, COLOR_WHITE, COLOR_YELLOW);
      // white piece on green
      init_pair(4, COLOR_WHITE, COLOR_GREEN);

      int curset = 1;

      for (int i = 1; i < 5; i++) {
        attrset(COLOR_PAIR(i));
        char to_print[] = {'s', 'e', 't', i, '\n', '\0'};
        addstr(to_print);
        attroff(COLOR_PAIR(curset));
      }

      for (int i = 0; i < 32; i++) {
        // change line if i is multiple of 8
        if (i % 8 == 0) {
          attrset(COLOR_PAIR(curset));
          addstr("\n K ");
          attroff(COLOR_PAIR(curset));
          continue;
        }
        if (curset == 1) curset = 2; else curset = 1;
        attrset(COLOR_PAIR(curset));
        addstr(" K ");
        attroff(COLOR_PAIR(curset));
      }

      curset = 3;
      for (int i = 32; i < BOARD_DIM*BOARD_DIM; i++) {
        // change line if i is multiple of 8
        if (i % 8 == 0) {
          attrset(COLOR_PAIR(curset));
          addstr("\n K ");
          attroff(COLOR_PAIR(curset));
          continue;
        }
        if (curset == 3) curset = 4; else curset = 3;
        attrset(COLOR_PAIR(curset));
        addstr(" K ");
        attroff(COLOR_PAIR(curset));
      }
      refresh();
    } else {
      addstr("Cannot start colours\n");
      refresh();
    }
  } else {
    addstr("Not colour capable\n");
    refresh();
  }
}

void printboard(board_t* board) {
  if (has_colors()) {
    if (start_color() == OK) {

      // reset colors, set yellow as grey
      init_color(COLOR_GREEN, 300, 464, 302);
      init_color(COLOR_YELLOW, 900, 900, 900);

      // black piece on white
      init_pair(1, COLOR_BLACK, COLOR_YELLOW);
      // black piece on green
      init_pair(2, COLOR_BLACK, COLOR_GREEN);
      // white piece on white
      init_pair(3, COLOR_WHITE, COLOR_YELLOW);
      // white piece on green
      init_pair(4, COLOR_WHITE, COLOR_GREEN);

      int curset;

      for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
        // change line if i is multiple of 8
        if (i % 8 == 0) {
          attrset(COLOR_PAIR(2));
          addstr("\n");
          attroff(COLOR_PAIR(2));
          refresh();
        }

        // if there is nothing, print nothing
        if (board->cells[i]->alignment == 0) {
          if (i % 2 == 0) {
            curset = 2;
          } else
            curset = 1;
          attrset(COLOR_PAIR(curset));
          addstr("   ");
          attroff(COLOR_PAIR(curset));
          continue;
        }

        // if there is something, check and set correct color
        if (board->cells[i]->alignment == 1) {
          if (i % 2 == 0)
            curset = 2;
          else
            curset = 1;
        } else if (i % 2 == 0)
          curset = 4;
        else
          curset = 3;
        attrset(COLOR_PAIR(curset));
        char to_print[] = {' ', board->cells[i]->piece_letter, ' ', '\0'};
        addstr(to_print);
        attroff(COLOR_PAIR(curset));
      }
    } else {
      addstr("Cannot start colours\n");
      refresh();
    }
  } else {
    addstr("Not colour capable\n");
    refresh();
  }
}

void init_board(board_t* board) {

  printf("starting initialization\n");

  // malloc for all cells
  for (int i = 0; i < BOARD_DIM*BOARD_DIM; i++) {
    board->cells[i] = malloc(sizeof(node_t));
  }

  for (int i = 0; i < 8; i++) {
    board->cells[i]->piece_letter = pieces_letter[i];
    board->cells[i]->alignment = 1;
  }

  printf("first row done\n");

  // Second row: Black Pawn Pieces
  for (int i = 8; i < 16; i++) {
    board->cells[i]->piece_letter = 'P';
    board->cells[i]->alignment = 1;
  }

  printf("second row done\n");

  // Non-Piece Nodes
  for (int i = 16; i < 48; i++) {
    if ((i / 8) % 2 == 0) {
      board->cells[i]->alignment = 0;
    } else {
      board->cells[i]->alignment = 0;
    }
  }

  // Seventh row: White Pawn Pieces
  for (int i = 48; i < 56; i++) {
    board->cells[i]->piece_letter = 'P';
    board->cells[i]->alignment = 2;
  }

  // Eighth row: White Non-Pawn Pieces
  for (int i = 56; i < 64; i++) {
    board->cells[i]->piece_letter = pieces_letter[63 - i];
    board->cells[i]->alignment = 2;
  }
}

int main(void) {
  board_t* board = malloc(sizeof(board_t));
  // initialize(board);
  init_board(board);

  initscr();

  printboard(board);
  // simpleprinting();

  addstr("\nMove!");
  refresh();

  char* command = malloc(sizeof(char) * 100);
  // while (true) {
  //   fgets(command, 100, stdin);
  //   if (strcmp(command, "quit") == 0) break;
  // }
  getchar();
  endwin();

  free(board);

  return EXIT_SUCCESS;
}

// wide char stuff

// wchar_t wstr[] = { 0x00002656, L'\0' };
// mvaddstr(0, 0, wstr);

// setlocale(LC_ALL, "");
// initscr();
// wchar_t wstr[] = { 9474, L'\0' };
// mvaddwstr(0, 0, wstr);
// refresh();
// getch();
// endwin();
