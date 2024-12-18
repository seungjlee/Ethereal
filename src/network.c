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

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitboards.h"
#include "board.h"
#include "evaluate.h"
#include "network.h"
#include "thread.h"
#include "types.h"

#include <emmintrin.h>
#include <immintrin.h>

PKNetwork PKNN;

static char *PKWeights[] = {
    #include "weights/pknet_224x32x2.net"
    ""
};

static inline int computePKNetworkIndex(int colour, int piece, int sq) {
    return (64 + 48) * colour
         + (48 * (piece == KING))
         + sq - 8 * (piece == PAWN);
}

void initPKNetwork() {

    for (int i = 0; i < PKNETWORK_LAYER1; i++) {

        char weights[strlen(PKWeights[i]) + 1];
        strcpy(weights, PKWeights[i]);
        strtok(weights, " ");

        for (int j = 0; j < PKNETWORK_INPUTS; j++)
            PKNN.inputWeights[j][i] = atof(strtok(NULL, " "));
        PKNN.inputBiases[i] = atof(strtok(NULL, " "));
    }

    for (int i = 0; i < PKNETWORK_OUTPUTS; i++) {

        char weights[strlen(PKWeights[i + PKNETWORK_LAYER1]) + 1];
        strcpy(weights, PKWeights[i + PKNETWORK_LAYER1]);
        strtok(weights, " ");

        for (int j = 0; j < PKNETWORK_LAYER1; j++)
            PKNN.layer1Weights[j][i] = atof(strtok(NULL, " "));
        PKNN.layer1Biases[i] = atof(strtok(NULL, " "));
    }
}

int computePKNetwork(Board *board) {
    uint64_t pawns = board->pieces[PAWN];
    uint64_t kings = board->pieces[KING];
    uint64_t black = board->colours[BLACK];

    const int avx_step = sizeof(__m256) / sizeof(float);
    __m256 layer1_256[PKNETWORK_LAYER1 / avx_step];
    float* layer1Neurons = (float*)layer1_256;

    const int MAX_PAWNS = 16;
    int pawnIndices[MAX_PAWNS];
    int pawnCount = 0;
    while (pawns) {
        int sq = poplsb(&pawns);
        pawnIndices[pawnCount++] = computePKNetworkIndex(testBit(black, sq), PAWN, sq);
    }
    assert(pawnCount <= MAX_PAWNS);

    int sq1 = poplsb(&kings);
    int sq2 = poplsb(&kings);
    int idx1 = computePKNetworkIndex(testBit(black, sq1), KING, sq1);
    int idx2 = computePKNetworkIndex(testBit(black, sq2), KING, sq2);

    for (int i = 0; i < PKNETWORK_LAYER1 / avx_step; i++) {
        layer1_256[i] = _mm256_add_ps(
            _mm256_add_ps(_mm256_load_ps(&PKNN.inputBiases[i*avx_step]),
                          _mm256_load_ps(&PKNN.inputWeights[idx1][i*avx_step])),
            _mm256_load_ps(&PKNN.inputWeights[idx2][i*avx_step])
        );
        for (int pawnIndex = 0; pawnIndex < pawnCount; pawnIndex++)
            layer1_256[i] = _mm256_add_ps(layer1_256[i], 
                                          _mm256_load_ps(&PKNN.inputWeights[pawnIndices[pawnIndex]][i*avx_step]));
    }

    __m128 output128;
    float* outputNeurons = (float*)&output128;
    const __m128 zeros128 = _mm_setzero_ps();
    outputNeurons[0] = PKNN.layer1Biases[0];
    outputNeurons[1] = PKNN.layer1Biases[1];
    for (int j = 0; j < PKNETWORK_LAYER1; j++) {
        __m128 layer1_128 = _mm_set1_ps(layer1Neurons[j]);
        layer1_128 = _mm_max_ps(layer1_128, zeros128);
        __m128d weights128 = _mm_load_sd((double*)&PKNN.layer1Weights[j][0]);
        output128 = _mm_add_ps(output128, _mm_mul_ps(layer1_128, *((__m128*)&weights128)));
    }

    assert(PKNETWORK_OUTPUTS == PHASE_NB);
    return MakeScore((int) outputNeurons[MG], (int) outputNeurons[EG]);
}
