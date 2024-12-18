#include <ctype.h>
#include <dirent.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #incldue <time.h>
#include <unistd.h>
#include <wchar.h>

// #include <curses.h>
// #include <ncurses.h>
#include <ncursesw/curses.h>

#include "ai.h"
#include "chess.h"
#include "printboard.h"

int has_game(char* name);
int save_game(board_t* board, char* name);
int print_games();
char32_t get_piece_unicode(char piece_str, int alignement);
int resume_game(board_t* board, char* name);
int move_command(char* command, board_t* board, int alignment);
void help_message();
void endgame_message();

WINDOW* win;
board_t* board;

int main() {
  setlocale(LC_ALL, "");

  // initialize the window
  win = initscr();
  if (win == NULL) {
    fprintf(stderr, "error initializing ncurses\n");
    return 1;
  }
  cbreak();
  clear();

  // initialize the board
  board = malloc(sizeof(board_t));
  init_board(board);

  // malloc memory for the user input
  char* command = malloc(sizeof(char) * 100);

  // print the introduction message to the window
  addstr("\nLET'S PLAY CHEZZ\n");
  refresh();
  help_message();
  move(0, 0);
  clrtobot();

  // print the board to the window
  printboard(board, win);

  while (true) {
    move(10, 0);
    clrtobot();
    // ask the user for a move or a command
    addstr("input move or command: ");
    refresh();
    getstr(command);

    // the user inputs the command to quit the game
    if (strcmp(command, "quit") == 0) break;
    // the user inputs the command to view the help message
    else if (strcmp(command, "help") == 0) {
      help_message();
    }
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
    } else {
      int result = move_command(command, board, 2);
      if (result == 0) {
        addstr("AI is thinking really hard!");
        refresh();
        move(0, 0);
        printboard(board, win);
        sleep(1);
        check_board(board);
      } else if (result == 2) {
        init_board(board);
      }
    }
    move(0, 0);
    printboard(board, win);
  }

  // // display the end of game message
  // end_game();

  // clean up the window
  delwin(win);
  endwin();

  // free dynamically allocated memory
  for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
    free(board->cells[i]);
  }
  free(board);
  free(command);

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
    sleep(1);
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
  if (file == NULL) {
    addstr("failed to open file to save game\n");
    refresh();
    sleep(1);
    return 1;
  }

  // write the board to the file, writing the information of the cells line-by-line
  for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
    fprintf(file, "%d ", board->cells[i]->alignment);
    switch (board->cells[i]->piece) {
      case B_ROOK:
        fprintf(file, "%s", "R\n");
        break;
      case W_ROOK:
        fprintf(file, "%s", "R\n");
        break;
      case B_KNIGHT:
        fprintf(file, "%s", "N\n");
        break;
      case W_KNIGHT:
        fprintf(file, "%s", "N\n");
        break;
      case B_BISHOP:
        fprintf(file, "%s", "B\n");
        break;
      case W_BISHOP:
        fprintf(file, "%s", "B\n");
        break;
      case B_QUEEN:
        fprintf(file, "%s", "Q\n");
        break;
      case W_QUEEN:
        fprintf(file, "%s", "Q\n");
        break;
      case B_KING:
        fprintf(file, "%s", "K\n");
        break;
      case W_KING:
        fprintf(file, "%s", "K\n");
        break;
      case B_PAWN:
        fprintf(file, "%s", "P\n");
        break;
      case W_PAWN:
        fprintf(file, "%s", "P\n");
        break;
      default:
        fprintf(file, "0\n");
        break;
    }
  }

  // close the file
  if (fclose(file) != 0) {
    addstr("failed to close file to save game\n");
    refresh();
    sleep(1);
    return 1;
  }

  addstr("game is successfully saved\n");
  refresh();
  sleep(1);

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
  if (dir == NULL) {
    addstr("failed to open directory for saved game files\n");
    refresh();
    sleep(1);
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
    sleep(1);
    return 1;
  }

  return 0;
}

/*
 * return the unicode value of the 'piece_str'
 */
char32_t get_piece_unicode(char piece_str, int alignment) {
  if (alignment == 1) {
    switch (piece_str) {
      case 'R':
        return B_ROOK;
      case 'N':
        return B_KNIGHT;
      case 'B':
        return B_BISHOP;
      case 'Q':
        return B_QUEEN;
      case 'K':
        return B_KING;
      case 'P':
        return B_PAWN;
    }
  } else if (alignment == 2) {
    switch (piece_str) {
      case 'R':
        return W_ROOK;
      case 'N':
        return W_KNIGHT;
      case 'B':
        return W_BISHOP;
      case 'Q':
        return W_QUEEN;
      case 'K':
        return W_KING;
      case 'P':
        return W_PAWN;
    }
  }
  return (char32_t)0;
}

/*
 * update the board to that of the 'name' saved game in the './games' directory
 * return 0 if succesful, 1 otherwise
 */
int resume_game(board_t* board, char* name) {
  // check if the name is invalid (i.e., does not exist)
  if (has_game(name) == 0) {
    addstr("name does not exist\n");
    refresh();
    sleep(1);
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
  if (file == NULL) {
    addstr("failed to open file to resume game\n");
    refresh();
    return 1;
  }

  char* line = malloc(sizeof(char) * 21);
  int alignment;
  char piece;
  // update the board cell-by-cell
  for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
    // read the file line-by-line
    if (fgets(line, 21, file) != NULL) {
      if (sscanf(line, "%d %c", &alignment, &piece) == 2) {
        // update the alignment
        board->cells[i]->alignment = alignment;
        // update the piece
        board->cells[i]->piece = get_piece_unicode(piece, alignment);
      } else {
        addstr("failed to read file to resume game\n");
        refresh();
        sleep(1);
        return 1;
      }
    } else {
      addstr("failed to read file to resume game\n");
      refresh();
      sleep(1);
      return 1;
    }
  }
  free(line);

  // close the file
  if (fclose(file) != 0) {
    addstr("failed to close file to resume game\n");
    refresh();
    sleep(1);
    return 1;
  }

  free(path);
  return 0;
}

/*
 * complete the move and update the board
 * return 0 if successful, 2 if user wins, otherwise 1
 */
int move_command(char* command, board_t* board, int alignment) {
  // check if the move command is invalid (i.e., input length)
  if (strlen(command) != 5) {
    addstr("invalid input length for move\n");
    refresh();
    sleep(1);
    return 1;
  }

  // get the start position
  char* start = strsep(&command, " ");
  if (command == NULL) {
    addstr("invalid move (use letters and numbers)\n");
    refresh();
    sleep(1);
    return 1;
  }
  // get the end position
  char* end = strsep(&command, " ");

  // check if the move command is invalid (i.e., the positions do not consist of a letter and an
  // integer)
  if (!(isalpha(start[0]) && isalpha(end[0]) && isdigit(start[1]) && isdigit(end[1]))) {
    addstr("invalid move (use letters and numbers)\n");
    refresh();
    sleep(1);
    return 1;
  }

  // capitalize both letters
  start[0] = toupper(start[0]);
  end[0] = toupper(end[0]);

  // checj if the move command is invalid (i.e., the letter values are invalid)
  if (start[0] > 'H' || start[0] < 'A' || end[0] > 'H' || end[0] < 'A') {
    addstr("invalid move (letters are invalid)\n");
    refresh();
    sleep(1);
    return 1;
  }

  // check if the move command is invalid (i.e., the integer values are invalid)
  if (start[1] > '8' || start[1] < '1' || end[1] > '8' || end[1] < '1') {
    addstr("invalid move (numbers are invalid)\n");
    refresh();
    sleep(1);
    return 1;
  }

  // convert the positions to indices
  int start_col = (int)(start[0]) - (int)'A';
  int start_row = (8 - ((int)(start[1]) - '0'));
  int end_col = (int)(end[0]) - (int)'A';
  int end_row = (8 - ((int)(end[1]) - '0'));

  int start_int = start_row * BOARD_DIM + start_col;
  int end_int = end_row * BOARD_DIM + end_col;

  // check for castle
  // first scenario: long path
  if (start_int == 60 && end_int == 56) {
    // first check if middle is empty
    if (board->cells[57]->alignment == 0 && board->cells[58]->alignment == 0 &&
        board->cells[59]->alignment == 0) {
      // then check if startpos and endpos are correct pieces
      if (board->cells[start_int]->piece == W_KING && board->cells[end_int]->piece == W_ROOK) {
        castle(board, end_int);
        wrefresh(win);
        refresh();
        return 0;
      }
    }
  }

  // second scenario: short path
  if (start_int == 60 && end_int == 63) {
    // first check if middle is empty
    if (board->cells[61]->alignment == 0 && board->cells[62]->alignment == 0) {
      // then check if startpos and endpos are correct pieces
      if (board->cells[start_int]->piece == W_KING && board->cells[end_int]->piece == W_ROOK) {
        castle(board, end_int);
        wrefresh(win);
        refresh();
        return 0;
      }
    }
  }

  // validate the move
  int result = validate_move(board, start_int, end_int, 2);
  // the move is valid
  if (result == 0) {
    wrefresh(win);
    refresh();
  } else if (result == 2) {
    clrtobot();
    move(0, 0);
    printboard(board, win);
    winning_animate(win);
    init_board(board);
    return 2;
  } else {  // the move is invalid
    addstr("invalid chess move\n");
    refresh();
    sleep(1);
    return 1;
  }
  return 0;
}

/*
 * display the help message
 */
void help_message() {
  addstr("\nthe following are the valid commands:\n");
  addstr("\t\"save\": to save the current game\n");
  addstr("\t\"resume\": to resume a previously saved game\n");
  addstr("\t\"quit\": to quit the current game or the current command\n");
  addstr("\t\"help\": to view this message again\n");
  addstr("otherwise, input a move (e.g., \"B1 C3\") \n");
  refresh();
  getchar();
}

/*
 * display the end of game message
 */
void endgame_message() {
  // TODO
  addstr("game over\n");
  refresh();
}
