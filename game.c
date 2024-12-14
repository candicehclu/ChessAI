#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <dirent.h>
#include <unistd.h>

// #include <curses.h>
// #include <ncurses.h>
#include <ncursesw/curses.h>

#include "chess.h"
#include "printboard.h"
#include "scheduler.h"

WINDOW* win;

int has_game(char* name);
int save_game(board_t* board, char* name);
int print_games();
//char32_t get_piece_unicode(char* piece_str);
int resume_game(board_t* board, char* name);
int command_to_move(char* command, int* pos, board_t* board, int alignment);

int main() {
  // task_t user_move_task;
  // task_t ai_move_task;
  // // create tasks for each task in the game
  // task_create(&user_move_task, user_move);
  // task_create(&ai_move_task, ai_move);

  setlocale(LC_ALL, "");

  // initialize the board
  board_t* board = malloc(sizeof(board_t));
  init_board(board);

  // initialize the window
  win = initscr();
  cbreak();
  clear();

  // print the board to the window
  printboard(board, win);

  // malloc memory for the user input
  char* command = malloc(sizeof(char) * 100);
  int* pos = malloc(sizeof(char) * 3 * 2);

  while (true) {
    move(10, 0);
    clrtobot();
    // ask the user for a move or a command
    addstr("input move or command: ");
    refresh();
    getstr(command);

    // the user inputs the command to quit the game
    if (strcmp(command, "quit") == 0) break;
    // the user inputs the command to save the game
    else if (strcmp(command, "save") == 0) {
      do {
        // ask the user for the name to save the game as
        addstr("input name (less than 20 characters) to save game as: ");
        refresh();
        getstr(command);
        if (strcmp(command, "quit") == 0) break;
      } while (save_game(board, command) == 1);
      // until the game is successfully saved
    }
    // the user inputs the command to resume a saved game
    else if (strcmp(command, "resume") == 0) {
      // print the names of the saved game(s)
      if (print_games() == 1) break;
      do {
        // ask the user input for the name of the saved game to resume
        addstr("input name of game to resume: ");
        refresh();
        getstr(command);
        if (strcmp(command, "quit") == 0) break;
      } while (resume_game(board, command) == 1);
      // until the saved game is successfully resumed
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
  for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
    free(board->cells[i]);
  }
  free(board);
  free(command);
  free(pos);

  return 0;
}

/*
 * check if the './games' directory has a 'name' textfile
 * return 0 if the directory does not have the game, 1 otherwise
 */
int has_game(char* name) {
  // create the file path
  // 8 for "./games/" and 4 for ".txt"
  char* path = malloc(sizeof(char) * (8 + strlen(name) + 4));
  strncpy(path, "./games/", 9);
  strcat(path, name);
  strcat(path, ".txt");

  int exist = 0;

  // the directory has and can access the game
  if (access(path, F_OK) == 0) exist = 1;

  free(path);
  return exist;
}

/*
 * save the 'board' in a 'name' textfile in the './games' directory
 * return 0 if successful, 1 otherwise
 */
int save_game(board_t* board, char* name) {
  // check if the name is invalid (i.e., is already taken)
  if (has_game(name) == 1) {
    addstr("name is taken\n");
    refresh();
    return 1;
  }

  // create the file path
  // 8 for "./games/" and 4 for ".txt"
  char* path = malloc(sizeof(char) * (8 + strlen(name) + 4));
  strncpy(path, "./games/", 9);
  strcat(path, name);
  strcat(path, ".txt");

  // open the file in write mode
  FILE* file = fopen(path, "w");
  if(file == NULL) {
    addstr("failed to open file to save game\n");
    refresh();
    return 1;
  }

  // write the board to the file, writing the information of the cells line-by-line
  for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
    fprintf(file, "%d", board->cells[i]->alignment);
    fprintf(file, " ");
    fprintf(file, "%u", board->cells[i]->piece);
    fprintf(file, "\n");
  }

  // close the file
  if (fclose(file) != 0) {
    addstr("failed to close file to save game\n");
    refresh();
    return 1;
  }

  addstr("game is successfully saved\n");
  refresh();

  free(path);
  return 0;
}

/*
 * print the names of the games saved in the './games' directory
 * return 0 if succesful, 1 otherwise
 */
int print_games() {
  // open the './games' directory
  DIR* dir = opendir("./games");
  if(dir == NULL) {
    addstr("failed to open directory for saved game files\n");
    refresh();
    return 1;
  }
  addstr("\n");
  addstr("names of saved games: ");
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
    refresh();
    return 1;
  }

  return 0;
}

// /*
//  * return the unicode value of the 'piece_str'
//  */
// char32_t get_piece_unicode(char32_t piece_str) {
//   if (strcmp(piece_str, "265C") == 0) return B_ROOK;
//   if (strcmp(piece_str, "265E") == 0) return B_KNIGHT;
//   if (strcmp(piece_str, "265D") == 0) return B_BISHOP;
//   if (strcmp(piece_str, "265B") == 0) return B_QUEEN;
//   if (strcmp(piece_str, "265A") == 0) return B_KING;
//   if (strcmp(piece_str, "265F") == 0) return B_PAWN;
//   if (strcmp(piece_str, "2656") == 0) return W_ROOK;
//   if (strcmp(piece_str, "2658") == 0) return W_KNIGHT;
//   if (strcmp(piece_str, "2657") == 0) return W_BISHOP;
//   if (strcmp(piece_str, "2655") == 0) return W_QUEEN;
//   if (strcmp(piece_str, "2654") == 0) return W_KING;
//   if (strcmp(piece_str, "2659") == 0) return W_PAWN;
//   // if (strcmp(piece_str, "0x0000265C") == 0) return B_ROOK;
//   // if (strcmp(piece_str, "0x0000265E") == 0) return B_KNIGHT;
//   // if (strcmp(piece_str, "0x0000265D") == 0) return B_BISHOP;
//   // if (strcmp(piece_str, "0x0000265B") == 0) return B_QUEEN;
//   // if (strcmp(piece_str, "0x0000265A") == 0) return B_KING;
//   // if (strcmp(piece_str, "0x0000265F") == 0) return B_PAWN;
//   // if (strcmp(piece_str, "0x00002656") == 0) return W_ROOK;
//   // if (strcmp(piece_str, "0x00002658") == 0) return W_KNIGHT;
//   // if (strcmp(piece_str, "0x00002657") == 0) return W_BISHOP;
//   // if (strcmp(piece_str, "0x00002655") == 0) return W_QUEEN;
//   // if (strcmp(piece_str, "0x00002654") == 0) return W_KING;
//   // if (strcmp(piece_str, "0x00002659") == 0) return W_PAWN;
//   return (char32_t)0;
// }

/*
 * update the board to that of the 'name' saved game in the './games' directory
 * return 0 if succesful, 1 otherwise
 */
int resume_game(board_t* board, char* name) {
  // check if the name is invalid (i.e., does not exist)
  if (has_game(name) == 0) {
    addstr("name does not exist\n");
    refresh();
    return 1;
  }

  // create the file path
  // 8 for "./games/" and 4 for ".txt"
  char* path = malloc(sizeof(char) * (8 + strlen(name) + 4));
  strncpy(path, "./games/", 9);
  strcat(path, name);
  strcat(path, ".txt");

  // open the file in read mode
  FILE* file = fopen(path, "r");
  if(file == NULL) {
    addstr("failed to open file to resume game\n");
    refresh();
    return 1;
  }

  char* line = malloc(sizeof(char) * 21);
  int alignment;
  int piece;
  //size_t size;
  // update the board cell-by-cell
  for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
    // read the file line-by-line
    if (fgets(line, 21, file) != NULL) {
      if (sscanf(line, "%d %u", &alignment, &piece) == 2) {
        //printf("%d %u   ", alignment, piece);
        // board->cells[i]->alignment = alignment;
        // board->cells[i]->piece = get_piece_unicode(piece);
      } else {
        addstr("failed to read file to resume game\n");
        refresh();
        return 1;
      }
    } else {
      addstr("failed to read file to resume game\n");
      refresh();
      return 1;
    }
    //getline(&line, &size, file);
    // printf("%d ", atoi(strsep(&line, " ")));
    // printf("%s   ", strsep(&line, " "));
    // // update the alignment of the cell
    // board->cells[i]->alignment = atoi(strsep(&line, " "));
    // // update the piece of the cell
    // board->cells[i]->piece = get_unicode(strsep(&line, " "));
    // char* info[2];
    // info[0] = strsep(&line, " ");
    // info[1] = strsep(&line, " ");
    // board->cells[i]->alignment = atoi(info[0]);
    // board->cells[i]->piece = get_unicode(info[1]);
  }
  free(line);

  // close the file
  if (fclose(file) != 0) {
    addstr("failed to close file to resume game\n");
    refresh();
    return 1;
  }

  free(path);
  return 0;
}

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
