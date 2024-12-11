#include "chess.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <unistd.h>

/**
 * Global Variables for reference, definining piece order
 * Piece Order: Rook - Knight - Bishop - Queen - King - Bishop - Knight - Rook
 */
char32_t b_pieces[] = {B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING, B_BISHOP, B_KNIGHT, B_ROOK};
char pieces_letter[] = {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'};
char32_t w_pieces[] = {W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING, W_BISHOP, W_KNIGHT, W_ROOK};
char32_t even_row[] = {LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE, DARK_SPACE,
                       LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE, DARK_SPACE};
char32_t odd_row[] = {DARK_SPACE, LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE,
                      DARK_SPACE, LIGHT_SPACE, DARK_SPACE, LIGHT_SPACE};

/**
 * Initializes board to starting state
 *
 * ALIGNMENT KEY:
 * 0 = EMPTY
 * 1 = BLACK PIECE
 * 2 = WHITE PIECE
 */
void initialize(board_t* board) {
  // First row: Black Non-Pawn Pieces
  for (int i = 0; i < 8; i++) {
    board->cells[i]->piece = b_pieces[i];
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


void init_board(board_t* board) {
  // malloc for all cells
  for (int i = 0; i < BOARD_DIM * BOARD_DIM; i++) {
    board->cells[i] = malloc(sizeof(node_t));
  }

  // FIrst row: Black pieces
  for (int i = 0; i < 8; i++) {
    board->cells[i]->piece = b_pieces[i];
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
      board->cells[i]->alignment = 0;
    } else {
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
    board->cells[i]->piece = w_pieces[i - 56];
    board->cells[i]->alignment = 2;
  }
}

/**
 * Helper Function: Making a move (ASSUMES IT IS VALID)
 */
void move_piece(board_t* board, int startpos, int endpos) {
  // Moves the Piece
  board->cells[endpos]->piece = board->cells[startpos]->piece;
  board->cells[endpos]->alignment = board->cells[startpos]->alignment;

  // Resets the original space to an empty node
  board->cells[startpos]->alignment = 0;
  if ((startpos / 8) % 2 == 0) {
    board->cells[startpos]->piece = even_row[startpos % 8];
  } else {
    board->cells[startpos]->piece = odd_row[startpos % 8];
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
int* rookmoves(int row, int col, board_t* board, int myalign) {
  // A rook will always have 14 available spaces to move to (NO TOWERING bc i genuinely cannot be
  // bothered lmao)
  int* moves = malloc(sizeof(int) * 14);
  size_t counter = 0;

  // Rightward Movements in its own row
  for (int i = col + 1; i < 8; i++) {
    int pos = (row * 8) + i;
    if (board->cells[pos]->alignment == myalign) {
      break;
    } else if (board->cells[pos]->alignment != 0) {
      moves[counter] = pos;
      counter++;
      break;
    } else {
      moves[counter] = i;
      counter++;
    }
  }

  // Leftward Movements in its own row
  for (int i = col - 1; i >= 0; i--) {
    int pos = (row * 8) + i;
    if (board->cells[pos]->alignment == myalign) {
      break;
    } else if (board->cells[pos]->alignment != 0) {
      moves[counter] = pos;
      counter++;
      break;
    } else {
      moves[counter] = i;
      counter++;
    }
  }

  // Upward Movements across rows
  for (int i = row - 1; i >= 0; i--) {
    int pos = (i * 8) + col;
    if (board->cells[pos]->alignment == myalign) {
      break;
    } else if (board->cells[pos]->alignment != 0) {
      moves[counter] = pos;
      counter++;
      break;
    } else {
      moves[counter] = i;
      counter++;
    }
  }

  // Downward Movements across rows
  for (int i = row + 1; i < 8; i++) {
    int pos = (i * 8) + col;
    if (board->cells[pos]->alignment == myalign) {
      break;
    } else if (board->cells[pos]->alignment != 0) {
      moves[counter] = pos;
      counter++;
      break;
    } else {
      moves[counter] = i;
      counter++;
    }
  }

  return moves;
}

int* knightmoves(int row, int col, board_t* board, int myalign) {
  // A knight will have a maximum of 8 available spaces to move to
  int* moves = malloc(sizeof(int) * 8);
  size_t counter = 0;

  // Arrays for row and column of every possible move, rows[i] and cols[i] is one possible move
  int rows[] = {row - 2, row - 1, row + 1, row + 2, row + 2, row + 1, row - 1, row - 2};
  int cols[] = {col + 1, col + 2, col + 2, col + 1, col - 1, col - 2, col - 2, col - 1};

  for (int i = 0; i < 8; i++) {
    if (rows[i] >= 0 && rows[i] <= 7 && cols[i] >= 0 && cols[i] <= 7) {
      int endpos = (rows[i] * 8) + cols[i];
      if (board->cells[endpos]->alignment != myalign) {
        moves[counter] = endpos;
        counter++;
      }
    }
  }

  return moves;
}

int* bishopmoves(int row, int col, board_t* board, int myalign) {
  // A bishop will have a maximum of 13 available spaces to move to
  int* moves = malloc(sizeof(int) * 13);
  size_t counter = 0;

  // UP + RIGHT Movements
  for (int i = 1; i < 7; i++) {
    int newrow = row - i;
    int newcol = col + i;
    if (newrow >= 0 && newrow <= 7 && newcol >= 0 && newcol <= 7) {
      int endpos = (newrow * 8) + newcol;
      if (board->cells[endpos]->alignment == myalign) {
        break;
      } else if (board->cells[endpos]->alignment != 0) {
        moves[counter] = endpos;
        counter++;
        break;
      } else {
        moves[counter] = endpos;
        counter++;
      }
    } else {
      break;
    }
  }

  // UP + LEFT Movements
  for (int i = 1; i < 7; i++) {
    int newrow = row - i;
    int newcol = col - i;
    if (newrow >= 0 && newrow <= 7 && newcol >= 0 && newcol <= 7) {
      int endpos = (newrow * 8) + newcol;
      if (board->cells[endpos]->alignment == myalign) {
        break;
      } else if (board->cells[endpos]->alignment != 0) {
        moves[counter] = endpos;
        counter++;
        break;
      } else {
        moves[counter] = endpos;
        counter++;
      }
    } else {
      break;
    }
  }

  // DOWN + RIGHT Movements
  for (int i = 1; i < 7; i++) {
    int newrow = row + i;
    int newcol = col + i;
    if (newrow >= 0 && newrow <= 7 && newcol >= 0 && newcol <= 7) {
      int endpos = (newrow * 8) + newcol;
      if (board->cells[endpos]->alignment == myalign) {
        break;
      } else if (board->cells[endpos]->alignment != 0) {
        moves[counter] = endpos;
        counter++;
        break;
      } else {
        moves[counter] = endpos;
        counter++;
      }
    } else {
      break;
    }
  }

  // DOWN + LEFT Movements
  for (int i = 1; i < 7; i++) {
    int newrow = row + i;
    int newcol = col - i;
    if (newrow >= 0 && newrow <= 7 && newcol >= 0 && newcol <= 7) {
      int endpos = (newrow * 8) + newcol;
      if (board->cells[endpos]->alignment == myalign) {
        break;
      } else if (board->cells[endpos]->alignment != 0) {
        moves[counter] = endpos;
        counter++;
        break;
      } else {
        moves[counter] = endpos;
        counter++;
      }
    } else {
      break;
    }
  }

  return moves;
}

int* queenmoves(int row, int col, board_t* board, int myalign) {
  // A queen will have a maximum of 27 available spaces to move to
  int* moves = malloc(sizeof(int) * 27);

  int* straightmoves = rookmoves(row, col, board, myalign);
  int* diagonalmoves = bishopmoves(row, col, board, myalign);

  memcpy(moves, straightmoves, 14 * sizeof(int));
  memcpy(moves, diagonalmoves, 13 * sizeof(int));

  return moves;
}

int* kingmoves(int row, int col, board_t* board, int myalign) {
  // A King will have a maximum of 8 available spaces to move to
  int* moves = malloc(sizeof(int) * 8);
  int counter = 0;

  int rows[] = {row - 1, row - 1, row, row + 1, row + 1, row + 1, row, row - 1};
  int cols[] = {col, col + 1, col + 1, col + 1, col, col - 1, col - 1, col - 1};

  for (int i = 0; i < 8; i++) {
    if (rows[i] >= 0 && rows[i] <= 7 && cols[i] >= 0 && cols[i] <= 7) {
      int endpos = (rows[i] * 8) + cols[i];
      if (board->cells[endpos]->alignment != myalign) {
        moves[counter] = endpos;
        counter++;
      }
    }
  }

  return moves;
}

int* white_pawnmoves(int row, int col, board_t* board) {
  // A pawn will have a maximum of 4 available spaces to move to
  int* moves = malloc(sizeof(int) * 4);
  int counter = 0;

  // Check if it is in first row
  if (row == 6) {
    int doublemove = ((row - 2) * 8) + col;
    if (board->cells[doublemove]->alignment == 0) {
      moves[counter] = doublemove;
      counter++;
    }
  }

  // Check if there are pieces diagonally in front of it

  // Up-Left
  if (row - 1 >= 0 && row - 1 <= 7 && col - 1 >= 0 && col - 1 <= 7) {
    int upleft = ((row - 1) * 8) + (col - 1);
    if (board->cells[upleft]->alignment == 1) {
      moves[counter] = upleft;
      counter++;
    }
  }

  // Up-Right
  if (row - 1 >= 0 && row - 1 <= 7 && col + 1 >= 0 && col + 1 <= 7) {
    int upleft = ((row - 1) * 8) + (col + 1);
    if (board->cells[upleft]->alignment == 1) {
      moves[counter] = upleft;
      counter++;
    }
  }

  // Normal case
  if (row - 1 >= 0) {
    int normalmove = ((row - 1) * 8) + col;
    if (board->cells[normalmove]->alignment == 0) {
      moves[counter] = ((row - 1) * 8) + col;
      counter++;
    }
  }

  return moves;
}

int* black_pawnmoves(int row, int col, board_t* board) {
  // A pawn will have a maximum of 4 available spaces to move to
  int* moves = malloc(sizeof(int) * 4);
  int counter = 0;

  // Check if it is in first row
  if (row == 1) {
    int doublemove = ((row + 2) * 8) + col;
    if (board->cells[doublemove] == 0) {
      moves[counter] = doublemove;
      counter++;
    }
  }

  // Check if there are pieces diagonally in front of it

  // Up-Left
  if (row + 1 >= 0 && row + 1 <= 7 && col - 1 >= 0 && col - 1 <= 7) {
    int upleft = ((row + 1) * 8) + (col - 1);
    if (board->cells[upleft]->alignment == 2) {
      moves[counter] = upleft;
      counter++;
    }
  }

  // Up-Right
  if (row + 1 >= 0 && row + 1 <= 7 && col + 1 >= 0 && col + 1 <= 7) {
    int upleft = ((row + 1) * 8) + (col + 1);
    if (board->cells[upleft]->alignment == 2) {
      moves[counter] = upleft;
      counter++;
    }
  }

  // Normal case
  if (row + 1 <= 7) {
    int normalmove = ((row + 1) * 8) + col;
    if (board->cells[normalmove]->alignment == 0) {
      moves[counter] = normalmove;
      counter++;
    }
  }

  return moves;
}

int validate_move(board_t* board, int startpos, int endpos, int myalign) {
  char32_t mypiece = board->cells[startpos]->piece;
  int* moves = NULL;

  if (board->cells[startpos]->alignment != myalign) {  // If alignment does not match up
    return -1;
  }

  if (myalign == 1) {  // Your piece is black
    switch (mypiece) {
      case B_BISHOP:
        moves = bishopmoves(startpos / 8, startpos % 8, board, myalign);
        break;
      case B_KING:
        moves = kingmoves(startpos / 8, startpos % 8, board, myalign);
        break;
      case B_KNIGHT:
        moves = knightmoves(startpos / 8, startpos % 8, board, myalign);
        break;
      case B_PAWN:
        moves = black_pawnmoves(startpos / 8, startpos % 8, board);
        break;
      case B_QUEEN:
        moves = queenmoves(startpos / 8, startpos % 8, board, myalign);
        break;
      case B_ROOK:
        moves = rookmoves(startpos / 8, startpos % 8, board, myalign);
        break;
    }
  } else if (myalign == 2) {  // Your piece is white
    switch (mypiece) {
      case W_BISHOP:
        moves = bishopmoves(startpos / 8, startpos % 8, board, myalign);
        break;
      case W_KING:
        moves = kingmoves(startpos / 8, startpos % 8, board, myalign);
        break;
      case W_KNIGHT:
        moves = knightmoves(startpos / 8, startpos % 8, board, myalign);
        break;
      case W_PAWN:
        moves = white_pawnmoves(startpos / 8, startpos % 8, board);
        break;
      case W_QUEEN:
        moves = queenmoves(startpos / 8, startpos % 8, board, myalign);
        break;
      case W_ROOK:
        moves = rookmoves(startpos / 8, startpos % 8, board, myalign);
        break;
    }
  }

  for (int i = 0; i < 27; i++) {  // Check if prompted endpos is within validmoves
    if (moves[i] == 0) {          // Reached end of list, break out of loop
      break;
    }
    if (endpos == moves[i]) {               // Move found
      move_piece(board, startpos, endpos);  // Make the move
      return 1;                             // Return 1 for success
    }
  }

  // If nothing returned, failed
  return -1;
}