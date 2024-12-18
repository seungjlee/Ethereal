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

#include <setjmp.h>
#include <stdbool.h>
#include <stdint.h>

#include "board.h"
#include "movepicker.h"
#include "network.h"
#include "search.h"
#include "transposition.h"
#include "types.h"

//#include "nnue/types.h"

enum {
    STACK_OFFSET = 4,
    STACK_SIZE = MAX_PLY + STACK_OFFSET
};

struct NodeState {

    int eval;          // Static evaluation of the Node
    int movedPiece;    // Moving piece, otherwise UB
    int dextensions;   // Number of Double Extensions
    bool tactical;     // Cached moveIsTactical()
    uint16_t move;     // Move applied at the Node
    uint16_t excluded; // Excluded move during Singular Extensions
    MovePicker mp;     // Move Picker at each ply

    // Fast reference for future use for History lookups
    int16_t (*continuations)[CONT_NB][PIECE_NB][SQUARE_NB];
};

struct Thread {

    Board board;
    Limits *limits;
    TimeManager *tm;
    PVariation pvs[MAX_PLY];

#ifdef ENABLE_MULTI_PV
    PVariation mpvs[MAX_MOVES];

    int multiPV;
    uint16_t bestMoves[MAX_MOVES];
#endif

    uint64_t nodes, tbhits;
    int depth, seldepth, height, completed;

    void *nnue;

    Undo undoStack[STACK_SIZE];
    NodeState *states, nodeStates[STACK_SIZE];

    ALIGN64 PKTable pktable;
    ALIGN64 KillerTable killers;
    ALIGN64 CounterMoveTable cmtable;
    ALIGN64 HistoryTable history;
    ALIGN64 CaptureHistoryTable chistory;
    ALIGN64 ContinuationTable continuation;

    int index, nthreads;
    Thread *threads;
#ifdef ENABLE_MULTITHREAD
    jmp_buf jbuffer;
#endif
};


Thread* createThreadPool(int nthreads);
void deleteThreadPool(Thread *threads);

void resetThreadPool(Thread *threads);

static inline uint64_t nodesSearchedThreadPool(Thread *threads) {
#ifdef ENABLE_MULTITHREAD
    // Sum up the node counters across each Thread. Threads have
    // their own node counters to avoid true sharing the cache
    uint64_t nodes = 0ull;

    for (int i = 0; i < threads->nthreads; i++)
        nodes += threads->threads[i].nodes;

    return nodes;
#else
    return threads->threads[0].nodes;
#endif
}

static inline uint64_t tbhitsThreadPool(Thread *threads) {
#ifdef ENABLE_MULTITHREAD
    // Sum up the tbhit counters across each Thread. Threads have
    // their own tbhit counters to avoid true sharing the cache
    uint64_t tbhits = 0ull;

    for (int i = 0; i < threads->nthreads; i++)
        tbhits += threads->threads[i].tbhits;

    return tbhits;
#else
    return threads->threads[0].tbhits;
#endif
}

static inline void newSearchThreadPool(Thread *threads, Board *board, Limits *limits, TimeManager *tm) {
    // Initialize each Thread in the Thread Pool. We need a reference
    // to the UCI seach parameters, access to the timing information,
    // somewhere to store the results of each iteration by the main, and
    // our own copy of the board. Also, we reset the seach statistics
    for (int i = 0; i < threads->nthreads; i++) {

        threads[i].limits = limits;
        threads[i].tm     = tm;
        threads[i].height = 0;
        threads[i].nodes  = 0ull;
        threads[i].tbhits = 0ull;

        memcpy(&threads[i].board, board, sizeof(Board));
        threads[i].board.thread = &threads[i];

        memset(threads[i].nodeStates, 0, sizeof(NodeState) * STACK_SIZE);
        // nnue_reset_evaluator(threads[i].nnue);
    }
}
