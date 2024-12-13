#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "chess.h"

// precondition: computer always play black

// declare global lock
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int check_board(board_t* board) {
    // array to store threads
    pthread_t threads[BOARD_DIM*BOARD_DIM];
    args_t args_arr[BOARD_DIM*BOARD_DIM];

    // saves the move of greatest value on the entire chessboard
    int* largest = malloc(sizeof(int));
    largest = 0;

    // saves the index of the move of greatest value
    int* largest_index = malloc(sizeof(int));
    largest_index = 0;

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
        args_arr[i] = args;

        // create thread
        pthread_create(&threads[i], NULL, &check_index, (void*) &args_arr[i]);
    }

    // join threads
    for (int i = 0; i < BOARD_DIM*BOARD_DIM; i++) {
        if (pthread_join(threads[i], NULL)) {
            perror("pthread_join failed\n");
        }
    }

    // secondary check on the current value for largest and largest_value
}

void* check_index(void* arguments) {

    // unpack arguments 
    args_t* args = (args_t*) arguments;

    // declare local variable to store index of move of largest value
    int max_value_index = 0;

    // check alignment
    if (args->board->cells[args->index]->alignment == 0) {
        return;
    // if is my piece check for the largest value of piece I can take from opponent
    } else if (args->board->cells[args->index]->alignment == 1){
        max_value_index = check_taking(args->board, args->index, args->largest, args->largest_index);
    // if it's opponent's piece then check what is the most valueable piece it can take from me
    // not sure if this two cases should be separated
    } else {
        max_value_index = check_danger(args->board, args->index, args->largest, args->largest_index);
    }

    // check to see what the current largest is, if it is larger then update
    pthread_mutex_lock(&lock);
    if (calculate_value(max_value_index) > args->largest) {
        args->largest = calculate_value(args->board->cells[max_value_index]->piece);
        args->largest_index = max_value_index;
    }
    pthread_mutex_unlock(&lock);

    return (void*) NULL;
}

/*
 * returns the value of the given piece
*/
int calculate_value(uint32_t piece) {
    switch(piece) {
        case B_PAWN: return 1;
        case W_PAWN: return 1;
        case B_KNIGHT: return 3;
        case W_KNIGHT: return 3;
        case B_BISHOP: return 3;
        case W_BISHOP: return 3;
        case B_ROOK: return 5; 
        case W_ROOK: return 5;
        case B_QUEEN: return 7;
        case W_QUEEN: return 7;
        case B_KING: return 10;
        case W_KING: return 10;
    }
    return -1;
}

int* generate_possible_moves(board_t* board, uint32_t piece, int row, int col) {
    int* moves;
    switch(piece) {
        case B_PAWN:
            moves = malloc(sizeof(int)*4);
            moves = black_pawnmoves(row, col, board);
        case W_PAWN:
            moves = malloc(sizeof(int)*4);
            moves = white_pawnmoves(row, col, board);
        case B_KNIGHT:
            moves = malloc(sizeof(int)*8);
            moves = knightmoves(row, col, board, my_align);
        case W_KNIGHT:
            moves = malloc(sizeof(int)*8);
            moves = knightmoves(row, col, board, my_align);
        case B_BISHOP:
            moves = malloc(sizeof(int)*13);
            moves = bishopmoves(row, col, board, my_align);
        case W_BISHOP:
            moves = malloc(sizeof(int)*13);
            moves = bishopmoves(row, col, board, my_align);
        case B_ROOK || W_ROOK:
            moves = malloc(sizeof(int)*14);
            moves = rookmoves(row, col, board, my_align);
        case B_QUEEN:
            moves = malloc(sizeof(int)*27);
            moves = queenmoves(row, col, board, my_align);
        case W_QUEEN:
            moves = malloc(sizeof(int)*27);
            moves = queenmoves(row, col, board, my_align);
        case B_KING:
            moves = malloc(sizeof(int)*8);
            moves = kingmoves(row, col, board, my_align);
        case W_KING:
            moves = malloc(sizeof(int)*8);
            moves = kingmoves(row, col, board, my_align);
    }
    return moves;
}

/*
 * check all the possible moves of this piece and return the index with the most valuable piece it can take\
 * returns index of the move with largest value
*/
int check_taking(board_t* board, int index, int* largest, int* largest_index) {
    uint32_t piece = board->cells[index]->piece;
    int my_align = board->cells[index]->alignment;
    int row = index / BOARD_DIM;
    int col = index % BOARD_DIM;

    // generate all possible moves
    int* moves = generate_possible_moves(board, piece, row, col);
    int local_largest = -1;
    int local_largest_index = -1;

    // check all possible moves to see if it can take
    for (int i = 0; i < 27; i++) {
        if (moves[i] == NULL) break;
        int piece_value = calculate_value(board->cells[moves[i]]->piece);
        if (piece_value > local_largest) {
            local_largest = piece_value;
            local_largest_index = moves[i];
        }
    }
    
    return local_largest_index;

}

/*
 * given a target, returns the index of one ai's piece that can take the target
 * i think this is not the optimal way to do this but it's possible but I'll wait and see if 
 * someone else comes up with sth better
*/
int check_piece_to_take_target(board_t* board, int target) {
    
    return -1;
}

int* return_random_move(board_t* board, int index) {
    srand(time(NULL));
    int piece = board->cells[index]->piece;
    int alignment = board->cells[index]->alignment;
    int* moves = generate_possible_moves(board, piece, row, col);
    int* pos = malloc(sizeof(int)*2);
    int randpos = randint() % sizeof(moves);

    // generate random int
    while (moves[randpos] == NULL) {
        randpos = randint() % sizeof(moves);
    }

    // if highest value piece is the ai's, move the piece away
    if (alignment == 2) {
        pos[0] = index;
        pos[1] = moves[randpos];
    } else if (alignment == 1) {
        // if the highest value piece is the user's, check which piece can take that piece
        pos[0] = check_piece_to_take_target(board, index);
        pos[1] = index;
    }
}