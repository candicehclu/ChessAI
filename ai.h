#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "chess.h"

typedef struct args {
    board_t* board;
    int index;
    int* largest;
    int* largest_index;
} args_t;

int check_board(board_t* board);

void* check_index(void* arguments);

/*
 * returns the value of the given piece
*/
int calculate_value(uint32_t piece);

int* generate_possible_moves(board_t* board, uint32_t piece, int row, int col);

/*
 * check all the possible moves of this piece and return the index with the most valuable piece it can take\
 * returns index of the move with largest value
*/
int check_taking(board_t* board, int index, int* largest, int* largest_index);

int check_piece_to_take_target(board_t* board, int target);

int return_random_move(board_t* board, int index);