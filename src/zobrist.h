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

#ifdef USE_XORSHIFT
extern uint64_t ZobristKeys[32][SQUARE_NB];
extern uint64_t ZobristEnpassKeys[FILE_NB];
extern uint64_t ZobristCastleKeys[SQUARE_NB];
extern uint64_t ZobristTurnKey;

void initZobrist();

static uint64_t seed = 1070372ull;

static uint64_t rand64() {

    // http://vigna.di.unimi.it/ftp/papers/xorshift.pdf

    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;

    return seed * 2685821657736338717ull;
}
#else

#define USE_LOOKUP_TABLE
#ifdef USE_LOOKUP_TABLE
extern uint64_t HashKeys[32][SQUARE_NB];
extern uint64_t HashEnpassKeys[FILE_NB];
extern uint64_t HashCastleKeys[SQUARE_NB];
extern uint64_t HashTurnKey;
void InitHashTables();
#endif

// From MMIX by Donald Knuth.
static inline uint64_t mmix64(uint64_t x) {
    x = 6364136223846793005ULL * x + 1442695040888963407ULL;
    return x;
}
static inline uint32_t HashPK(uint32_t x, uint32_t y) {
    return (uint32_t)mmix64(y << 8 | x);
}

static inline uint64_t HashBoard(uint32_t x, uint32_t y) {
#ifdef USE_LOOKUP_TABLE
    return HashKeys[x][y];
#else
    return mmix64(1ULL << 40 | y << 8 | x);
#endif
}
static inline uint64_t HashBoardEnpass(uint32_t x) {
#ifdef USE_LOOKUP_TABLE
    return HashEnpassKeys[x];
#else
    return mmix64(1ULL << 42 | x);
#endif
}
static inline uint64_t HashBoardCastle(uint32_t x) {
#ifdef USE_LOOKUP_TABLE
    return HashCastleKeys[x];
#else
    return mmix64(1ULL << 44 | x);
#endif
}

#ifndef USE_LOOKUP_TABLE
static const uint64_t HashTurnKey = 16430674600777974095ULL; // mmix64(1ULL << 60)
#endif

static uint64_t seed = 777;
static inline uint64_t rand64() {
    seed = mmix64(seed);
    return seed;
}
#endif