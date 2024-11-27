#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <uchar.h>

#define BOARD_DIM 8
#define BLOCK SIZE 32

/** 
 * Piece Definitions 
 */
#define B_ROOK 0x0000265C
#define B_KNIGHT 0x0000265E
#define B_BISHOP 0x0000265D
#define B_QUEEN 0x0000265B
#define B_KING 0x0000265A
#define B_PAWN 0x0000265F

#define W_ROOK 0x00002656
#define W_KNIGHT 0x00002658
#define W_BISHOP 0x00002657
#define W_QUEEN 0x00002655
#define W_KING 0x00002654
#define W_PAWN 0x00002659

#define DARK_SPACE 0x000025A0
#define LIGHT_SPACE 0x000025A1

/**
 * Defining the chessboard structure
 */
typedef struct node {
  int row;
  int col;
  char32_t piece;
  int alignment;
} node_t;

typedef struct board {
  node_t* cells[BOARD_DIM * BOARD_DIM];
} board_t;

/**
 * Global Variables for reference, definining piece order
 * Piece Order: Rook - Knight - Bishop - Queen - King - Bishop - Knight - Rook
 */
char32_t b_pieces[] = {B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING, B_BISHOP, B_KNIGHT, B_ROOK};
char32_t w_pieces[] = {W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING, W_BISHOP, W_KNIGHT, W_ROOK};
char32_t even_row[] = {LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE, DARK_SPACE};
char32_t odd_row[] = {DARK_SPACE, LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE};

/**
 * Initializes board to starting state
 * 
 * ALIGNMENT KEY:
 * 0 = EMPTY
 * 1 = BLACK PIECE
 * 2 = WHITE PIECE
 */
void initialize (board_t* board) {
  // First row: Black Non-Pawn Pieces
  for (int i = 0; i < 8; i++) {
    board->cells[i]->piece =  b_pieces[i];
    board->cells[i]->alignment = 1;
  }
  // Second row: Black Pawn Pieces
  for (int i = 8; i < 16; i++) {
    board->cells[i]->piece = B_PAWN;
    board->cells[i]->alignment = 1;
  }

  // Non-Piece Nodes
  for (int i = 16; i < 48; i++) {
    if ((i / 8) % 2 == 0) {
      board->cells[i]->piece = even_row[i % 8];
      board->cells[i]->alignment = 0;
    } else {
      board->cells[i]->piece = odd_row[i % 8];
      board->cells[i]->alignment = 0;
    }
  }

  // Seventh row: White Pawn Pieces
  for (int i = 48; i < 56; i++) {
    board->cells[i]->piece = W_PAWN;
    board->cells[i]->alignment = 2;
  }

  // Eighth row: White Non-Pawn Pieces
  for (int i = 56; i < 64; i++) {
    board->cells[i]->piece = w_pieces[i];
    board->cells[i]->alignment = 2;
  }
}

/** 
 * Helper Function: Making a move (ASSUMES IT IS VALID)
 */
void move (board_t* board, int startpos, int endpos) {
  // Moves the Piece
  board->cells[endpos]->piece = board->cells[startpos]->piece;
  board->cells[endpos]->alignment = board->cells[startpos]->alignment;

  // Resets the original space to an empty node
  board->cells[startpos]->alignment = 0;
  if ((startpos / 8) % 2 == 0) {
    board->cells[startpos]->piece = even_row[i % 8];
  } else {
    board->cells[startpos]->piece = odd_row[i % 8];
  }

  // Edge Case: If it is a pawn moving to the other edge
  if (board->cells[endpos]->piece == W_PAWN && (endpos / 8) == 0) {
    board->cells[endpos]->piece = W_QUEEN;
  } else if (board->cells[endpos]->piece == B_PAWN && (endpos / 8) == 7) {
    board->cells[endpos]->piece = B_QUEEN;
  }
}

/**
 * Helper Functions: Calculates Valid Moves for Every Piece
 * Params: Starting row, Starting Column
 * Return: Array of integers indicating possible positions to move to
 */
int* rookmoves (int row, int col) {
  // Define Starting Position
  int start = (row * 8) + col;

  // A rook will always have 14 available spaces to move to (NO TOWERING bc i genuinely cannot be bothered lmao)
  int* moves = malloc(sizeof(int) * 14);
  size_t counter = 0;

  // Horizontal Movements in its own row
  for (int i = row * 8; i < (row * 8) + 8; i++) {
    if (i != start) {
      moves[counter] = i;
      counter++;
    }
  }

  // Vertical Movements across rows
  for (int i = col; i < 64; i += 8) {
    if (i != start) {
      moves[counter] = i;
      counter++;
    }
  }

  return moves;
}

int* knightmoves (int row, int col) {
  // Define Starting Position
  int start = (row * 8) + col;

  // A knight will have a maximum of 8 available spaces to move to
  int* moves = malloc(sizeof(int) * 8);
  size_t counter = 0;

  // Arrays for row and column of every possible move, rows[i] and cols[i] is one possible move
  int rows[] = {row - 2, row - 1, row + 1, row + 2, row + 2, row + 1, row - 1, row - 2};
  int cols[] = {col + 1, col + 2, col + 2, col + 1, col - 1, col - 2, col - 2, col - 1};

  for (int i = 0; i < 8; i++) {
    int endpos = (rows[i] * 8) + cols[i];
    if (endpos >= 0 && endpos < 64) {
      moves[counter] = endpos;
      counter++;
    }
  }

  return moves;
}

int* bishopmoves (int row, int col) {
  // Define Starting Position
  int start = (row * 8) + col;

  // A bishop will have a maximum of 13 available spaces to move to
  int* moves = malloc(sizeof(int) * 13);
  size_t counter = 0;

  // Direction 1 Movements
  for (int i = -7; i < 7; i++) {
    int endpos = ((row + i) * 8) + (col - i);
    if (endpos >= 0 && endpos < 64 && endpos != start) {
      moves[counter] = endpos;
      counter++;
    }
  }

  // Direction 1 Movements
  for (int i = -7; i < 7; i++) {
    int endpos = ((row + i) * 8) + (col + i);
    if (endpos >= 0 && endpos < 64 && endpos != start) {
      moves[counter] = endpos;
      counter++;
    }
  }

  return moves;
}

int* queenmoves (int row, int col) {
  // A queen will have a maximum of 27 available spaces to move to
  int* moves = malloc(sizeof(int) * 27);

  int* straightmoves = rookmoves(row, col);
  int* diagonalmoves = bishopmoves(row, col);

  memcpy(moves, straightmoves, 14 * sizeof(int));
  memcpy(moves, diagonalmoves, 13 * sizeof(int));

  return moves;
}

int* kingmoves (int row, int col) {
  // A King will have a maximum of 8 available spaces to move to
  
}

int* white_pawnmoves (int row, int col) {
  // Check if there are pieces diagonally in front of it

}

int* black_pawnmoves (int row, int col) {
  // Check if there are pieces diagonally in front of it
}
