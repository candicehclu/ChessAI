// #include<ncurses.h>
#include <stdlib.h>
#include <ncursesw/ncurses.h>
#include <curses.h>
#include <locale.h>
#include <wchar.h>

#define BOARDSIZE 8

//--------------------------------------------------------
// FUNCTION PROTOTYPES
//--------------------------------------------------------
void printing();
void moving_and_sleeping();
void printboard();

//--------------------------------------------------------
// FUNCTION main
//--------------------------------------------------------
int main(void)
{ 
    setlocale(LC_ALL, "");
    // initscr();
    // wchar_t wstr[] = { 9474, L'\0' };
    // mvaddwstr(0, 0, wstr);
    // refresh();
    // getch();
    // endwin();

    initscr();

    printboard();

    // moving_and_sleeping();

    addstr("\nMove!");
    refresh();
    while (true) getch();
    endwin();

    return EXIT_SUCCESS;
} 

char piece = 'K';

void printboard()
{
    if(has_colors())
    {
        if(start_color() == OK)
        {
            init_pair(1, COLOR_BLACK, COLOR_WHITE);
            init_pair(2, COLOR_WHITE, COLOR_GREEN);

            int curset = 2;

            // attrset(COLOR_PAIR(1));
            // addstr("       \n   K   \n       \n\n");
            for (int i = 0; i < BOARDSIZE; i++) {
                for (int j = 0; j < BOARDSIZE; j++) {
                    attrset(COLOR_PAIR(curset));
                    // wchar_t wstr[] = { 0x00002656, L'\0' };
                    // mvaddstr(0, 0, wstr);
                    addstr(" K ");
                    attroff(COLOR_PAIR(curset));
                    if (curset == 1) {
                        curset = 2;
                    } else {
                        curset = 1;
                    }
                }
                attrset(COLOR_PAIR(2));
                addstr("\n");
                attroff(COLOR_PAIR(2));
                refresh();
                if (curset == 1) {
                    curset = 2;
                } else {
                    curset = 1;
                }
            }
        }
        else
        {
            addstr("Cannot start colours\n");
            refresh();
        }
    }
    else
    {
        addstr("Not colour capable\n");
        refresh();
    }
} 