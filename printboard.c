// #include <curses.h>
#include <locale.h>
//#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <ncursesw/curses.h>

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

void printboard(board_t* board, WINDOW* win) {
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
      wrefresh(win);
      clrtoeol();
      refresh();
    } else {
      addstr("Cannot start colours\n");
      wrefresh(win);
      refresh();
    }
  } else {
    addstr("Not colour capable\n");
    wrefresh(win);
    refresh();
  }
}

// int main(void) {
//   setlocale(LC_ALL, "");
//   board_t* board = malloc(sizeof(board_t));
//   init_board(board);

//   initscr();
//   cbreak();
//   noecho();
//   clear();

//   printboard(board);

//   addstr("\nInsert move: ");
//   refresh();
//   getch();
//   endwin();

//   // free everything
//   for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
//     free(board->cells[i]);
//   }
//   free(board);
//   return 0;
// }

