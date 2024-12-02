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
 * Global Variables for reference, definining piece order
 * Piece Order: Rook - Knight - Bishop - Queen - King - Bishop - Knight - Rook
 */
char32_t b_pieces[] = {B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING, B_BISHOP, B_KNIGHT, B_ROOK};
char pieces_letter[] = {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'};
char32_t w_pieces[] = {W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING, W_BISHOP, W_KNIGHT, W_ROOK};
char32_t even_row[] = {LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE, DARK_SPACE};
char32_t odd_row[] = {DARK_SPACE, LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE};


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
 * Initializes board to starting state
 * 
 * ALIGNMENT KEY:
 * 0 = EMPTY
 * 1 = BLACK PIECE
 * 2 = WHITE PIECE
 */
void initialize (board_t* board);

/** 
 * Helper Function: Making a move (ASSUMES IT IS VALID)
 */
void move_piece (board_t* board, int startpos, int endpos);

/**
 * Helper Functions: Calculates Valid Moves for Every Piece
 * Params: Starting row, Starting Column
 * Return: Array of integers indicating possible positions to move to
 */
int* rookmoves (int row, int col);

int* knightmoves (int row, int col);

int* bishopmoves (int row, int col);

int* queenmoves (int row, int col);

int* kingmoves (int row, int col);

int* white_pawnmoves (int row, int col);

int* black_pawnmoves (int row, int col);
