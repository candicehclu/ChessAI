#include <curses.h>
#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "chess.h"

/*
 * to compile:
 * gcc -Wall $(pkg-config --cflags ncursesw) -o printboard printboard.c $(pkg-config --libs ncursesw)
 */

int colorset(int alignment, int background) {
  if (alignment == 1) {
    if (background == 1)
      return 2;
    else
      return 1;
  }
  if (background == 1)
    return 4;
  else
    return 3;
}

void printboard(board_t* board) {
  if (has_colors()) {
    if (start_color() == OK) {
      // reset colors, set yellow as grey
      init_color(COLOR_GREEN, 300, 564, 302);
      init_color(COLOR_YELLOW, 700, 700, 700);

      // black piece on white
      init_pair(1, COLOR_BLACK, COLOR_YELLOW);
      // black piece on green
      init_pair(2, COLOR_BLACK, COLOR_GREEN);
      // // white piece on white
      init_pair(3, COLOR_WHITE, COLOR_YELLOW);
      // // white piece on green
      init_pair(4, COLOR_WHITE, COLOR_GREEN);

      int curset;
      int row;
      int col;
      int background;

      for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
        
        // calculations for determining cell color
        row = i / 8;
        col = i % 8;
        background = (int)(row % 2) + (int)(col % 2);
        if (curset == 0) curset = 2;

        // get appropriate color settings
        curset = colorset(board->cells[i]->alignment, background);

        // change line and print location marker if we are on a new line
        if (i % 8 == 0) {
          attrset(COLOR_PAIR(0));
          addstr("\n ");
          addch((char)(56 - row));
          addch(' ');
          attroff(COLOR_PAIR(0));
        }

        // if there is nothing, print nothing
        if (board->cells[i]->alignment == 0) {
          attrset(COLOR_PAIR(curset));
          addstr("   ");
          attroff(COLOR_PAIR(curset));
          continue;
        }

        // print piece if there is a piece
        attrset(COLOR_PAIR(curset));
        wint_t piece = (wint_t)board->cells[i]->piece;
        cchar_t cc;
        setcchar(&cc, (wchar_t*)&piece, 0, 0, NULL);
        addch(' ');
        add_wch(&cc);
        addch(' ');
        attroff(COLOR_PAIR(curset));
      }
      // print bottom location markers
      addstr("\n    A  B  C  D  E  F  G  H\n");
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

void init_board(board_t* board) {
  // malloc for all cells
  for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
    board->cells[i] = malloc(sizeof(node_t));
  }

  // FIrst row: Black pieces
  for (int i = 0; i < 8; i++) {
    board->cells[i]->piece = b_pieces[i];
    board->cells[i]->alignment = 1;
  }

  // Second row: Black Pawn Pieces
  for (int i = 8; i < 16; i++) {
    board->cells[i]->piece = B_PAWN;
    board->cells[i]->alignment = 1;
  }

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
    board->cells[i]->piece = W_PAWN;
    board->cells[i]->alignment = 2;
  }

  // Eighth row: White Non-Pawn Pieces
  for (int i = 56; i < 64; i++) {
    board->cells[i]->piece = w_pieces[i - 56];
    board->cells[i]->alignment = 2;
  }
}

int main(void) {
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