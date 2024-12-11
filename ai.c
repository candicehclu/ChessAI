#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "chess.h"

// precondition: computer always play black

typedef struct args {
    board_t* board;
    int index;
    int* largest;
    int* largest_index;
} args_t;

int check_board(board_t* board) {
    // array to store threads
    pthread_t threads[BOARD_DIM*BOARD_DIM];
    args_t args[BOARD_DIM*BOARD_DIM];

    // declare lock
    pthread_mutex_t* lock;
    pthread_mutex_init(lock, NULL);

    int* largest = malloc(sizeof(int));
    int* largest_index = malloc(sizeof(int));

    // create one thread for each cell
    for(int i = 0; i < BOARD_DIM*BOARD_DIM; i++) {
        // initialize thread
        pthread_t thread;
        threads[i] = thread;

        // initialize args
        args_t args;
        args.board = board;
        args.index = i;
        args.largest = largest;
        args.largest_index = 0;
        args[i] = args;

        // create thread
        pthread_create(&thread[i], NULL, &check_index, (void*) &args);
    }

    // join threads
    for (int i = 0; i < BOARD_DIM*BOARD_DIM; i++) {
        if (pthread_join(threads[i], NULL)) {
            perror("pthread_join failed\n");
        }
    }

    // secondary check on the current value for largest and largest_value
}

void* check_index(int index, int* largest, int* largest_index) {

    // check alignment
    int max_value = 0;
    if (args.board->cells[index]->alignment == 0) {
        return;
    // if is my piece check for the largest value of piece I can take from opponent
    } else if (args.board->cells[index]->alignment == 1){
        max_value = check_taking(board, index, largest, largest_index);
    // if it's opponent's piece then check what is the most valueable piece it can take from me
    // not sure if this two cases should be separated
    } else {
        max_value = check_danger(board, index, largest, largest_index);
    }

    // check to see what the current largest is, if it is larger then update
    pthread_mutex_lock(&lock);
    if (max_value > *largest) {
        *largest = max_value;
        *largest_index = index;
    }
    pthread_mutex_unlock(&lock);

    return (void*) NULL;
}

/*
 * returns the value of the given piece
*/
int calculate_value(uint32_t piece) {
    switch(piece) {
        case B_PAWN || W_PAWN:
            return 1;
        case B_KNIGHT || W_KNIGHT:
            return 3;
        case B_BISHOP || W_BISHOP:
            return 3;
        case B_ROOK || W_ROOK:
            return 5;
        case B_QUEEN || W_QUEEN:
            return 7;
        case B_KING || W_KING:
            return 10;
    }
    return 0;
}

/*
 * check all the possible moves of this piece and return the index with the most valuable piece it can take\
 * needs to return two things: the piece
*/
int check_taking(board_t* board, int index, int* largest, int* largest_inde) {
    uint32_t piece = board->cells[index]->piece;
    int my_align = board->cells[index]->alignment;
    int row = index / BOARD_DIM;
    int col = index % BOARD_DIM;

    // generate all possible moves
    switch(piece) {
        case B_PAWN || W_PAWN:
            int* moves = malloc(sizeof(int)*4);
            if (my_align == 1) moves = black_pawnmoves(row, col, board);
            else moves = white_pawnmoves(row, col, board);
        case B_KNIGHT || W_KNIGHT:
            int* moves = malloc(sizeof(int)*8);
            moves = knightmoves(row, col, board, my_align);
        case B_BISHOP || W_BISHOP:
            int* moves = malloc(sizeof(int)*13);
            moves = bishopmoves(row, col, board, my_align);
        case B_ROOK || W_ROOK:
            int* moves = malloc(sizeof(int)*14);
            moves = rookmoves(row, col, board, my_align);
        case B_QUEEN || W_QUEEN:
            int* moves = malloc(sizeof(int)*27);
            moves = queenmoves(row, col, board, my_align);
        case B_KING || W_KING:
            int* moves = malloc(sizeof(int)*8);
            moves = kingmoves(row, col, board, my_align);
    }

    // check all possible moves to see if it can take



}