/*
  Ethereal is a UCI chess playing engine authored by Andrew Grant.
  <https://github.com/AndyGrant/Ethereal>     <andrew@grantnet.us>

  Ethereal is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Ethereal is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <stdint.h>

#include "bitboards.h"
#include "thread.h"
#include "types.h"

enum {
    NONE_MOVE = 0, NULL_MOVE = 11,

    NORMAL_MOVE = 0 << 12, CASTLE_MOVE    = 1 << 12,
    ENPASS_MOVE = 2 << 12, PROMOTION_MOVE = 3 << 12,

    PROMOTE_TO_KNIGHT = 0 << 14, PROMOTE_TO_BISHOP = 1 << 14,
    PROMOTE_TO_ROOK   = 2 << 14, PROMOTE_TO_QUEEN  = 3 << 14,

    KNIGHT_PROMO_MOVE = PROMOTION_MOVE | PROMOTE_TO_KNIGHT,
    BISHOP_PROMO_MOVE = PROMOTION_MOVE | PROMOTE_TO_BISHOP,
    ROOK_PROMO_MOVE   = PROMOTION_MOVE | PROMOTE_TO_ROOK,
    QUEEN_PROMO_MOVE  = PROMOTION_MOVE | PROMOTE_TO_QUEEN
};

static inline int castleKingTo(int king, int rook) {
    return square(rankOf(king), (rook > king) ? 6 : 2);
}

static inline int castleRookTo(int king, int rook) {
    return square(rankOf(king), (rook > king) ? 5 : 3);
}

int apply(Thread *thread, Board *board, uint16_t move);
void applyLegal(Thread *thread, Board *board, uint16_t move);
void applyMove(Board *board, uint16_t move, Undo *undo);
void applyNormalMove(Board *board, uint16_t move, Undo *undo);
void applyCastleMove(Board *board, uint16_t move, Undo *undo);
void applyEnpassMove(Board *board, uint16_t move, Undo *undo);
void applyPromotionMove(Board *board, uint16_t move, Undo *undo);
void applyNullMove(Board *board, Undo *undo);

void revertMove(Board *board, uint16_t move, Undo *undo);

int moveEstimatedValue(Board *board, uint16_t move);
int moveBestCaseValue(Board *board);
int moveIsLegal(Board *board, uint16_t move);
int moveIsPseudoLegal(Board *board, uint16_t move);
int moveWasLegal(Board *board);

void printMove(uint16_t move, int chess960);
void moveToString(uint16_t move, char *str, int chess960);

#define MoveFrom(move)         (((move) >> 0) & 63)
#define MoveTo(move)           (((move) >> 6) & 63)
#define MoveType(move)         ((move) & (3 << 12))
#define MovePromoType(move)    ((move) & (3 << 14))
#define MovePromoPiece(move)   (1 + ((move) >> 14))
#define MoveMake(from,to,flag) ((from) | ((to) << 6) | (flag))

#ifdef ENABLE_MULTI_PV
static inline int moveExaminedByMultiPV(Thread *thread, uint16_t move) {
    // Check to see if this move was already selected as the
    // best move in a previous iteration of this search depth
    for (int i = 0; i < thread->multiPV; i++)
        if (thread->bestMoves[i] == move)
            return 1;

    return 0;
}
#endif

static inline int moveIsTactical(Board *board, uint16_t move) {

    // We can use a simple bit trick since we assert that only
    // the enpass and promotion moves will ever have the 13th bit,
    // (ie 2 << 12) set. We use (2 << 12) in order to match move.h
    assert(ENPASS_MOVE & PROMOTION_MOVE & (2 << 12));
    assert(!((NORMAL_MOVE | CASTLE_MOVE) & (2 << 12)));

    // Check for captures, promotions, or enpassant. Castle moves may appear to be
    // tactical, since the King may move to its own square, or the rooks square
    return (board->squares[MoveTo(move)] != EMPTY && MoveType(move) != CASTLE_MOVE)
        || (move & ENPASS_MOVE & PROMOTION_MOVE);
}

static inline void revertNullMove(Board *board, Undo *undo) {
    // Revert information which is hard to recompute
    board->hash            = undo->hash;
    board->threats         = undo->threats;
    board->epSquare        = undo->epSquare;
    board->halfMoveCounter = undo->halfMoveCounter;

    // NULL moves simply swap the turn only
    board->turn = !board->turn;
    board->numMoves--;
    board->fullMoveCounter--;
}

static inline void revert(Thread *thread, Board *board, uint16_t move) {
    ASSERT_PRINT_INT(thread->height > 0, thread->height);
    if (move == NULL_MOVE)
        revertNullMove(board, &thread->undoStack[--thread->height]);
    else
        revertMove(board, move, &thread->undoStack[--thread->height]);
}

static inline int moveIsInRootMoves(Thread *thread, uint16_t move) {
    // We do two things: 1) Check to make sure we are not using a move which
    // has been flagged as excluded thanks to Syzygy probing. 2) Check to see
    // if we are doing a "go searchmoves <>"  command, in which case we have
    // to limit our search to the provided moves.
    for (int i = 0; i < MAX_MOVES; i++)
        if (move == thread->limits->excludedMoves[i])
            return 0;

    if (!thread->limits->limitedByMoves)
        return 1;

    for (int i = 0; i < MAX_MOVES; i++)
        if (move == thread->limits->searchMoves[i])
            return 1;

    return 0;
}
