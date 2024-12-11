#include <stdio.h>
// #include <ncurses.h>
#include <stdlib.h>
// #include <curses.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>
#include <unistd.h>
#include <ncursesw/curses.h>

#include "chess.h"
#include "printboard.h"

WINDOW* win;

/*
 * return 0 if move command is valid, else return 1
*/
int command_to_move(char* command, int* pos, board_t* board, int alignment) {

  char* pos_str[2];

  // check that strlen is 6
  if (strlen(command) != 5) {
    addstr("Invalid input length\n");
    refresh();
    return 1;
  }

  // get rid of \n
  // command[strlen(command) - 1] = '\0';

  //check if third character is space, otherwise say wrong format

  // string split
  pos_str[0] = strsep(&command, " ");
  pos_str[1] = strsep(&command, " ");

  // check if if both position strings composes of an upper case letter and an int
  if (!(isalpha(pos_str[0][0]) && isalpha(pos_str[1][0]) && isdigit(pos_str[0][1]) && isdigit(pos_str[1][1]))) {
    addstr("Invalid move\n");
    refresh();
    return 1;
  }

  // make sure both are upper
  pos_str[0][0] = toupper(pos_str[0][0]);
  pos_str[1][0] = toupper(pos_str[1][0]);

  // convert
  int start_col = (int)(pos_str[0][0]) - (int)'A';
  int start_row = (8 - ((int)(pos_str[0][1]) - '0'));

  int end_col = (int)(pos_str[1][0]) - (int)'A';
  int end_row = (8 - ((int)(pos_str[1][1]) - '0'));

  pos[0] = start_row * BOARD_DIM + start_col;
  pos[1] = end_row * BOARD_DIM + end_col;

  int result = validate_move(board, pos[0], pos[1], 2);
  if (result == 1) {
    addstr("YAYYYY!\n");
    wrefresh(win);
    refresh();
  } else {
    addstr("NAYYY!\n");
    wrefresh(win);
    refresh();
  }
  return 0;
}

int save_game(board_t* board, char* name) {
  // string append for file path
  int name_length = strlen(name);
  char* path = malloc(sizeof(char)*(11 + name_length));
  path = name;
  strcat(path, "games/");
  strcat(path, name);
  strcat(path, ".txt");

  // check if the file already exists
  if (access(path, F_OK) == 0) {
    printf("File already exists\n");
    return 1;
  }

  // else save it
  FILE* file = fopen(path, "w");
  if(file == NULL) {
    perror("fopen failed\n");
    return 1;
  }

  // print the info of the board, each cell is printed on its own line
  for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
    // char* cell_info = malloc(sizeof(int) + sizeof(char) + sizeof(char32_t) + sizeof(char)*2);
    fprintf(file, "%d", board->cells[i]->alignment);
    fprintf(file, " ");
    fprintf(file, "%u", board->cells[i]->piece);
    fprintf(file, "\n");
  }

  // close file
  if (fclose(file) != 0) {
    perror("fclose fails\n");
    return 1;
  }
  printf("Your game is successfully saved\n");

  // add game name to separate file
  file = fopen("./saved_games.txt", "a");
  if(file == NULL) {
    perror("fopen failed\n");
    return 1;
  }

  // write game name
  fprintf(file, "%s", name);
  fprintf(file, "\n");

  // close game name file
  if (fclose(file) != 0) {
    perror("fclose fails\n");
    return 1;
  }
  return 0;
}

int print_saved_games() {
  FILE* file = fopen("./saved_games.txt", "a");
  if(file == NULL) {
    perror("fopen failed\n");
    return 1;
  }

  // name length should be less or equal to 20
  char* line = malloc(sizeof(char)*21);
  size_t size;
  // ssize_t read;
  while (getline(&line, &size, file) != -1) {
    printf("%s\n", line);
  }

  // close game name file
  if (fclose(file) != 0) {
    perror("fclose fails\n");
    return 1;
  }
  return 0;
}

char32_t get_unicode(char* piece_str) {
  if (strcmp(piece_str, "0x0000265C") == 0) return B_ROOK;
  if (strcmp(piece_str, "0x0000265E") == 0) return B_KNIGHT;
  if (strcmp(piece_str, "0x0000265D") == 0) return B_BISHOP;
  if (strcmp(piece_str, "0x0000265B") == 0) return B_QUEEN;
  if (strcmp(piece_str, "0x0000265A") == 0) return B_KING;
  if (strcmp(piece_str, "0x0000265F") == 0) return B_PAWN;
  if (strcmp(piece_str, "0x00002656") == 0) return W_ROOK;
  if (strcmp(piece_str, "0x00002658") == 0) return W_KNIGHT;
  if (strcmp(piece_str, "0x00002657") == 0) return W_BISHOP;
  if (strcmp(piece_str, "0x00002655") == 0) return W_QUEEN;
  if (strcmp(piece_str, "0x00002654") == 0) return W_KING;
  if (strcmp(piece_str, "0x00002659") == 0) return W_PAWN;
  return (char32_t)0;
}

int validate_game_name(char* game) {
  int valid = 0;
  
  // read names of currently saved games
  FILE* file = fopen("./saved_games.txt", "a");
  if(file == NULL) {
    perror("fopen failed\n");
    return 1;
  }

  // name length should be less or equal to 20
  char* line = malloc(sizeof(char)*21);
  size_t size;
  // ssize_t read;
  while (getline(&line, &size, file) != -1) {
    if (strcmp(line, game) == 0) valid = 1;
  }

  if (fclose(file) != 0) {
    perror("fclose fails\n");
    return 1;
  }

  return valid;
}

/*
 * (assumes game is valid) start a new game, initialize the board as indicated in the saved file (tells scheduler to start)
*/
int resume_game(char* game_name, board_t* board) {

  // process name string
  int name_length = strlen(game_name);
  char* path = malloc(sizeof(char)*(11 + name_length));
  path = game_name;
  strcat(path, "games/");
  strcat(path, game_name);
  strcat(path, ".txt");

  // open path in read-only mode
  FILE* file = fopen(path, "r");
  if(file == NULL) {
    perror("fopen failed\n");
    return 1;
  }

  // update board
  for(int i = 0; i < BOARD_DIM*BOARD_DIM; i++) {
    char* line = malloc(sizeof(char)*21);
    size_t size;
    // ssize_t read;

    // getline
    getline(&line, &size, file);
    char* info[2];

    // strsep for cell info
    info[0] = strsep(&line, " ");
    info[1] = strsep(&line, " ");

    // update cell
    board->cells[i]->alignment = atoi(info[0]);
    board->cells[i]->piece = get_unicode(info[1]);
  }
  return 0;
}

int main() {
  setlocale(LC_ALL, "");
  board_t* board = malloc(sizeof(board_t));
  init_board(board);

  win = initscr();
  cbreak();
  // noecho();
  clear();

  printboard(board, win);

  // malloc for command
  char* command = malloc(sizeof(char)*100);
  int* pos = malloc(sizeof(char)* 3 * 2);

  // fgets(command, 100, stdin);

  while (true) {

    // ask for input
    addstr("Insert command or move: ");
    wrefresh(win);
    refresh();
    getstr(command);

    // check for commands
    if (strcmp(command, "quit") == 0) break;
    if (strcmp(command, "save") == 0) {
      // prompt user to enter game name
      printf("Game name to save (less than 20 characters): ");
      fgets(command, 20, stdin);
      save_game(board, command); // ask user to make the file name to be less than 20 chracters
    }
    if (strcmp(command, "resume") == 0) {
      print_saved_games();
      // do sth else
    }    
    // if it is a valid move, process input string and call move_piece
    command_to_move(command, pos, board, 2);
    move(0, 0);
    printboard(board, win);
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
