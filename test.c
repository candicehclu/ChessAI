#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int command_to_move(char* command, int* pos) {
  char* token;
  char* pos_str[2];

  // check that strlen is 6
  if (strlen(command) != 6) {
    printf("Invalid size of input\n");
    return 1;
  }

  // get rid of \n
  command[strlen(command) - 1] = '\0';

  // string split
  pos_str[0] = strsep(&command, " ");
  pos_str[1] = strsep(&command, " ");

  // check if if both position strings composes of an upper case letter and an int
  if (!(isalpha(pos_str[0][0]) && isalpha(pos_str[1][0]) && isdigit(pos_str[0][1]) &&
        isdigit(pos_str[1][1]))) {
    printf("Invalid position\n");
    return 1;
  }

  // make sure both are upper
  pos_str[0][0] = toupper(pos_str[0][0]);
  pos_str[1][0] = toupper(pos_str[1][0]);

  // convert
  int start_col = (int)(pos_str[0][0]) - (int)'A';
  int start_row = (int)(8 - ((int)(pos_str[0][1]) - '0'));
  int end_col = (int)(pos_str[1][0]) - (int)'A';
  int end_row = (int)(8 - ((int)(pos_str[1][1]) - '0'));

  if (!(start_col >= 0 && start_col < 8 && 
        end_col >= 0 && end_col < 8 && 
        start_row >= 0 && start_row < 8 && 
        end_row >= 0 && end_row < 8)) {
    printf("Invalid position 2\n");
    return 1;
  }

  // convert 2
  pos[0] = 8*start_row + start_col;
  pos[1] = 8*end_row + end_col;
//   printf("startpos: %d: %d, endpos: %d: %d\n", start_col, start_row, end_col, end_row);

  printf("startpos: %d, endpos: %d\n", pos[0], pos[1]);
  return 0;
}


int main(void) {
  int pos[2];
  char* str = "A1 C0\n";

  char* move = malloc(sizeof(char) * 7);
  char* token;

  for (int i = 0; i < 7; i++) {
    move[i] = str[i];
    if (i == 7) move[i] = '\0';
    // if (move[i] == '\n') move[i] = '\0';
  }
    int result;
  result = command_to_move(move, pos);
  if (result == 0) printf("works!\n");

//   for (int i = 0; token = strsep(&move, " "); i++) {
//     pos[i] = token;
//   }
  // printf("startpos: %s, endpos: %s\n", pos[0], pos[1]);
  return 0;
}
