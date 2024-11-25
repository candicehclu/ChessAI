#include<ncurses.h>
#include<stdlib.h>

#define BOARDSIZE 8

//--------------------------------------------------------
// FUNCTION PROTOTYPES
//--------------------------------------------------------
void printing();
void moving_and_sleeping();
void colouring();

//--------------------------------------------------------
// FUNCTION main
//--------------------------------------------------------
int main(void)
{
    initscr();

    // colouring();

    moving_and_sleeping();

    addstr("\npress any key to exit...");
    refresh();

    getch();
    endwin();

    return EXIT_SUCCESS;
} 

void moving_and_sleeping()
{
    int row = 5;
    int col = 0;

    curs_set(0);

    for(char c = 65; c <= 90; c++)
    {
        move(row++, col++);
        addch(c);
        refresh();
        napms(100);
    }

    row = 5;
    col = 3;

    for(char c = 97; c <= 122; c++)
    {
        mvaddch(row++, col++, c);
        refresh();
        napms(100);
    }

    curs_set(1);

    addch('\n');
}

char piece = 'K';

void colouring()
{
    if(has_colors())
    {
        if(start_color() == OK)
        {
            init_pair(1, COLOR_BLACK, COLOR_WHITE);
            init_pair(2, COLOR_WHITE, COLOR_BLACK);

            int curset = 2;

            // attrset(COLOR_PAIR(1));
            // addstr("       \n   K   \n       \n\n");
            for (int i = 0; i < BOARDSIZE; i++) {
                for (int j = 0; j < BOARDSIZE; j++) {
                    attrset(COLOR_PAIR(curset));
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