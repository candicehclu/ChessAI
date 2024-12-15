#include "ai.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "chess.h"

// precondition: computer always play black

// declare global lock
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int largest = 0;
int global_start = -1;
int global_end = -1;

int check_board(board_t* board) {
  // array to store threads
  pthread_t threads[BOARD_DIM * BOARD_DIM];
  args_t args_arr[BOARD_DIM * BOARD_DIM];

  // saves the move of greatest value on the entire chessboard
  largest = 0;

  // saves the index of the move of greatest value
  global_start = -1;
  global_end = -1;

  // create one thread for each cell
  for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
    // initialize args
    args_t args;
    args.board = board;
    args.index = i;
    args_arr[i] = args;

    // Maybe we should malloc if it seg faults
    //  create thread
    if (pthread_create(&threads[i], NULL, &check_index, (void*)&args_arr[i]) != 0) {
      perror("thread creation error");
      exit(EXIT_FAILURE);
    }
  }

  // join threads
  for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
    if (pthread_join(threads[i], NULL)) {
      perror("pthread_join failed\n");
			exit(EXIT_FAILURE);
    }
  }

  // secondary check on the current value for largest and largest_value
  move_piece(board, global_start, global_end);
	return 0;
}

// scans the board, returns the negative version of the most valuable piece
int board_scan(board_t* board, int my_align, int opp_align) {
  int max = 0;
  for (int i = 0; i < 64; i++) {
    node_t* node = board->cells[i];
    if (node->alignment == opp_align) {
      int* moves = generate_possible_moves(board, node->piece, i / 8, i % 8);
      for (int i = 0; i < 28; i++) {
        if (moves[i] == 0 && moves[i + 1] == 0) {
          break;
        }
        if (board->cells[moves[i]]->alignment == my_align) {
          if (calculate_value(board->cells[moves[i]]->piece) > max) {
            max = calculate_value(board->cells[moves[i]]->piece);
          }
        }
      }
    }
  }
  return (-1 * max);
}

// It simulates a piece moving to an ending position, scans it again, returns a new score
int simulate_move(board_t* board, int startpos, int endpos) {
  // Malloc space
  board_t* board_copy = malloc(sizeof(board_t));

  for (int i = 0; i < 64; i++) {
    board_copy->cells[i] = malloc(sizeof(node_t));
  }

  // Assign values
  for (int i = 0; i < 64; i++) {
    board_copy->cells[i]->alignment = board->cells[i]->alignment;
    board_copy->cells[i]->piece = board->cells[i]->piece;
  }

  // Move Piece and simulate scan
  move_piece(board_copy, startpos, endpos);
  int newscore = board_scan(board_copy, 1, 2);

  // Free unused memory
  for (int i = 0; i < 64; i++) {
    free(board_copy->cells[i]);
  }
  free(board_copy);

  return newscore;
}

void* check_index(void* arguments) {
  // unpack arguments
  args_t* args = (args_t*)arguments;
  board_t* board = args->board;
  int index = args->index;

  // declare local variable to store index of move of largest value
  int score = args->score;
  int max_value_index = -1;

  // check alignment
  if (board->cells[index]->alignment == 1) {
    // Generate possible moves for my piece
    int* moves = generate_possible_moves(board, board->cells[index]->piece, index / 8, index % 8);
    for (int i = 0; i < 28; i++) {  // For every move possible
      int endpos = moves[i];
      int newscore = simulate_move(board, index, endpos);
      if (newscore > score) {
        score = newscore;
        max_value_index = moves[i];
      }
    }
  } else {
    // if it's not my piece do nothing
    return NULL;
  }

  // Put a lock on the global score and global best move, modify it if local score is better
  pthread_mutex_lock(&lock);
  if (score > largest && max_value_index != -1) {
    largest = score;
    global_start = index;
    global_end = max_value_index;
  }
  pthread_mutex_unlock(&lock);

  return (void*)NULL;
}

/*
 * returns the value of the given piece
 */
int calculate_value(uint32_t piece) {
  switch (piece) {
    case B_PAWN:
      return 1;
    case W_PAWN:
      return 1;
    case B_KNIGHT:
      return 3;
    case W_KNIGHT:
      return 3;
    case B_BISHOP:
      return 3;
    case W_BISHOP:
      return 3;
    case B_ROOK:
      return 5;
    case W_ROOK:
      return 5;
    case B_QUEEN:
      return 9;
    case W_QUEEN:
      return 9;
    case B_KING:
      return 1000;
    case W_KING:
      return 1000;
  }
  return -1;
}

int* generate_possible_moves(board_t* board, uint32_t piece, int row, int col) {
  int* moves;
  switch (piece) {
    case B_PAWN:
      moves = malloc(sizeof(int) * 4);
      moves = black_pawnmoves(row, col, board);
      break;
    case W_PAWN:
      moves = malloc(sizeof(int) * 4);
      moves = white_pawnmoves(row, col, board);
      break;
    case B_KNIGHT:
      moves = malloc(sizeof(int) * 8);
      moves = knightmoves(row, col, board, 1);
      break;
    case W_KNIGHT:
      moves = malloc(sizeof(int) * 8);
      moves = knightmoves(row, col, board, 1);
      break;
    case B_BISHOP:
      moves = malloc(sizeof(int) * 13);
      moves = bishopmoves(row, col, board, 1);
      break;
    case W_BISHOP:
      moves = malloc(sizeof(int) * 13);
      moves = bishopmoves(row, col, board, 1);
      break;
    case B_ROOK || W_ROOK:
      moves = malloc(sizeof(int) * 14);
      moves = rookmoves(row, col, board, 1);
      break;
    case B_QUEEN:
      moves = malloc(sizeof(int) * 27);
      moves = queenmoves(row, col, board, 1);
      break;
    case W_QUEEN:
      moves = malloc(sizeof(int) * 27);
      moves = queenmoves(row, col, board, 1);
      break;
    case B_KING:
      moves = malloc(sizeof(int) * 8);
      moves = kingmoves(row, col, board, 1);
      break;
    case W_KING:
      moves = malloc(sizeof(int) * 8);
      moves = kingmoves(row, col, board, 1);
      break;
  }
  return moves;
}

/*
 * check all the possible moves of this piece and return the index with the most valuable piece it
 * can take\ returns index of the move with largest value
//  */
// int check_taking(board_t* board, int index, int* largest, int* largest_index) {
//   uint32_t piece = board->cells[index]->piece;
//   int my_align = board->cells[index]->alignment;
//   int row = index / BOARD_DIM;
//   int col = index % BOARD_DIM;

//   // generate all possible moves
//   int* moves = generate_possible_moves(board, piece, row, col);
//   int local_largest = -1;
//   int local_largest_index = -1;

//   // check all possible moves to see if it can take
//   for (int i = 0; i < 27; i++) {
//     if (moves[i] == NULL) break;
//     int piece_value = calculate_value(board->cells[moves[i]]->piece);
//     if (piece_value > local_largest) {
//       local_largest = piece_value;
//       local_largest_index = moves[i];
//     }
//   }

//   return local_largest_index;
// }

/*
 * given a target, returns the index of one ai's piece that can take the target
 * i think this is not the optimal way to do this but it's possible but I'll wait and see if
 * someone else comes up with sth better
//  */
// int check_piece_to_take_target(board_t* board, int target) {
//   return -1;
// }

// int* return_random_move(board_t* board, int index) {
//   srand(time(NULL));
//   int piece = board->cells[index]->piece;
//   int alignment = board->cells[index]->alignment;
//   int* moves = generate_possible_moves(board, piece, row, col);
//   int* pos = malloc(sizeof(int) * 2);
//   int randpos = randint() % sizeof(moves);

//   // generate random int
//   while (moves[randpos] == NULL) {
//     randpos = randint() % sizeof(moves);
//   }

//   // if highest value piece is the ai's, move the piece away
//   if (alignment == 2) {
//     pos[0] = index;
//     pos[1] = moves[randpos];
//   } else if (alignment == 1) {
//     // if the highest value piece is the user's, check which piece can take that piece
//     pos[0] = check_piece_to_take_target(board, index);
//     pos[1] = index;
//   }
// }