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
#include <dirent.h>

#include "chess.h"
#include "printboard.h"
#include "scheduler.h"

WINDOW* win;
bool running;

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
  if (result != 1) {
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
  strncpy(path, "./games/", 9);
  // strcat(path, "games/");
  strcat(path, name);
  strcat(path, ".txt");

  // printf("%s\n", path);

  // check if the file already exists
  if (access(path, F_OK) == 0) {
    addstr("File already exists\n");
    wrefresh(win);
    refresh();
    return 1;
  }

  // else save it
  FILE* file = fopen(path, "w");

  if (access(path, F_OK) == 0) {
    printf("File already exists\n");
  }

  if(file == NULL) {
    addstr("fopen failed\n");
    wrefresh(win);
    refresh();
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
    addstr("fclose fails\n");
    wrefresh(win);
    refresh();
    return 1;
  }
  addstr("Your game is successfully saved\n");
  wrefresh(win);
  refresh();

  // add game name to separate file
  file = fopen("./saved_games.txt", "a");
  if(file == NULL) {
    addstr("fopen failed\n");
    wrefresh(win);
    refresh();
    return 1;
  }

  // write game name
  fprintf(file, "%s", name);
  fprintf(file, "\n");

  // close game name file
  if (fclose(file) != 0) {
    addstr("fclose fails\n");
    wrefresh(win);
    refresh();
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

/*
 * print the names of the saved games to the UI
 * return 0 if succesful, 1 otherwise
 */
int print_saved_games() {
  // open the directory ./games which stores the saved game files
  DIR* dir = opendir("./games");
  if(dir == NULL) {
    addstr("failed to open directory for saved game files\n");
    // wrefresh(win);
    refresh();
    return 1;
  }
  addstr("\n");
  addstr("follwing are names of saved games: ");
  addstr("\n");

  // read the entries in the directory one-by-one
  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)) {
      // extract the file names without the file extensions
      char* file_name = entry->d_name;
      char* game_name = strsep(&file_name, ".");
      addstr(game_name);
      addstr("\n");
    }
  }
  addstr("\n");

  // close the directory
  if (closedir(dir) == -1) {
    addstr("failed to close directory for saved game files\n");
    // wrefresh(win);
    refresh();
    return 1;
  }

  return 0;
}

/*
 * validate a game name for resuming
 * return 0 if successful, 1 otherwise
 */
int validate_game_name(char* game) {
  int valid = 1;

  // the directory ./games stores the files of the saved games
  DIR* dir = opendir("./games");
  if(dir == NULL) {
    addstr("failed to open directory for saved game files\n");
    // wrefresh(win);
    refresh();
    return 1;
  }

  // read the entries in the directory one-by-one
  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)) {
      // extract the file names without the file extensions
      char* file_name = entry->d_name;
      char* game_name = strsep(&file_name, ".");
      // compare the file names
      if (strcmp(game_name, game) == 1) {
        valid = 0;
        break;
      }
    }
  }

  // close the directory
  if (closedir(dir) == -1) {
    addstr("failed to close directory for saved game files\n");
    // wrefresh(win);
    refresh();
    return 1;
  }

  if (valid == 0) {
    addstr("valid game\n");
    refresh();
  } else {
    addstr("invalid game\n");
    refresh();
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
    addstr("fopen failed\n");
    wrefresh(win);
    refresh();
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

/*
 * make the move for user
*/
void user_move(char* command, int* pos, board_t* board, int alignment) {
  command_to_move(command, pos, board, alignment);
}

/*
 * runs the ai to evaluate next move and make the move
*/
void ai_move() {
  // let ai generate move and move
}

int main() {

  // task_t user_move_task;
  // task_t ai_move_task;

  // // Create tasks for each task in the game
  // task_create(&user_move_task, user_move);
  // task_create(&ai_move_task, ai_move);

  setlocale(LC_ALL, "");
  board_t* board = malloc(sizeof(board_t));
  init_board(board);

  win = initscr();
  cbreak();
  // noecho();
  clear();

  running = true;
  printboard(board, win);

  // malloc for command
  char* command = malloc(sizeof(char) * 100);
  int* pos = malloc(sizeof(char) * 3 * 2);

  // fgets(command, 100, stdin);

  while (true) {
    move(10, 0);
    clrtobot();
    // ask for user input for move or for command
    addstr("input move or command: ");
    // wrefresh(win);
    refresh();
    getstr(command);

    // command to quit game
    if (strcmp(command, "quit") == 0) break;
    // command to save game
    else if (strcmp(command, "save") == 0) {
      do {
        // ask for user input for game name
        addstr("input name (less than 20 characters) to save game as: ");
        refresh();
        getstr(command);
        if (strcmp(command, "quit") == 0) break;
      }
      while (save_game(board, command) == 1);
    }
    // command to resume saved game
    else if (strcmp(command, "resume") == 0) {
      // print names of saved games
      if (print_saved_games() == 1) break;
      do {
        // ask for user input for game name
        addstr("input name of game to resume: ");
        refresh();
        getstr(command);
        if (strcmp(command, "quit") == 0) break;
      } while(validate_game_name(command) == 1);
    }
    else {
    // if it is a valid move, process input string and move piece
      command_to_move(command, pos, board, 2);
    }
    move(0, 0);
    printboard(board, win);
    sleep(0.5);
    // else error
  }

  endwin();

  // free dynamically allocated memory
  free(command);
  free(pos);
  for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
    free(board->cells[i]);
  }
  free(board);

  return 0;
}
