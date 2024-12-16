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
#include "types.h"

int genAllNoisyMoves(Board *board, uint16_t *moves);
int genAllQuietMoves(Board *board, uint16_t *moves);

static inline int genAllLegalMoves(Board *board, uint16_t *moves) {

    Undo undo[1];
    int size = 0, pseudo = 0;
    uint16_t pseudoMoves[MAX_MOVES];

    // Call genAllNoisyMoves() & genAllNoisyMoves()
    pseudo  = genAllNoisyMoves(board, pseudoMoves);
    pseudo += genAllQuietMoves(board, pseudoMoves + pseudo);
    ASSERT_PRINT_INT((size_t)pseudo <= sizeof(pseudoMoves)/sizeof(pseudoMoves[0]), pseudo);

    // Check each move for legality before copying
    for (int i = 0; i < pseudo; i++) {
        applyMove(board, pseudoMoves[i], undo);
        if (moveWasLegal(board)) moves[size++] = pseudoMoves[i];
        revertMove(board, pseudoMoves[i], undo);
    }

    return size;
}
