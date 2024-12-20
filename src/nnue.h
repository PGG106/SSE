#pragma once

//#include "simd.h"
#include "types.h"
#include "shims.h"

// Net arch: (768xINPUT_BUCKETS -> L1_SIZE)x2 -> 1xOUTPUT_BUCKETS
#define NUM_INPUTS 768
#define INPUT_BUCKETS 1
#define L1_SIZE 80
#define OUTPUT_BUCKETS 1

#define FT_QUANT 255
#define L1_QUANT 64
#define NET_SCALE 400

#if defined(USE_SIMD)
#define CHUNK_SIZE (sizeof(vepi16) / sizeof(int16_t))
#else
#define CHUNK_SIZE  1
#endif

typedef size_t NNUEIndices[2];
//using NNUEIndices = std::array<std::size_t, 2>;

struct Network {
    int16_t FTWeights[INPUT_BUCKETS * NUM_INPUTS * L1_SIZE];
    int16_t FTBiases[L1_SIZE];
    int16_t L1Weights[L1_SIZE * 2 * OUTPUT_BUCKETS];
    int16_t L1Biases[OUTPUT_BUCKETS];

    //int16_t *FTWeights;
    //int16_t *FTBiases;
    //int16_t *L1Weights;
    //int16_t *L1Biases;
};

extern struct Network net;

struct Position;

// per pov accumulator
struct Pov_Accumulator {
    //std::array<int16_t, L1_SIZE> values;
    int16_t values[L1_SIZE];
    int pov;
    size_t NNUEAdd[2];
    int NNUEAdd_size;
    size_t NNUESub[2];
    int NNUESub_size;
    bool needsRefresh;
};

void Pov_Accumulator_accumulate(struct Pov_Accumulator* accumulator, struct Position* pos);
int Pov_Accumulator_GetIndex(const struct Pov_Accumulator* accumulator, const int piece, const int square, const int kingSq, bool flip);
void Pov_Accumulator_addSub(struct Pov_Accumulator* accumulator, struct Pov_Accumulator* prev_acc, size_t add, size_t sub);
void Pov_Accumulator_addSubSub(struct Pov_Accumulator* accumulator, struct Pov_Accumulator* prev_acc, size_t add, size_t sub1, size_t sub2);
void Pov_Accumulator_applyUpdate(struct Pov_Accumulator* accumulator, struct Pov_Accumulator* previousPovAccumulator);
void Pov_Accumulator_refresh(struct Pov_Accumulator* accumulator, struct Position* pos);

inline bool Pov_Accumulator_isClean(const struct Pov_Accumulator *accumulator) {
    return accumulator->NNUEAdd_size == 0;
}

// final total accumulator that holds the 2 povs
struct Accumulator {
    struct Pov_Accumulator perspective[2];
};

inline void Accumulator_AppendAddIndex(struct Accumulator *accumulator, int piece, int square, const int wkSq, const int bkSq, bool flip[2]) {
    assert(accumulator->perspective[WHITE].NNUEAdd_size <= 1);
    assert(accumulator->perspective[BLACK].NNUEAdd_size <= 1);
    accumulator->perspective[WHITE].NNUEAdd[accumulator->perspective[WHITE].NNUEAdd_size++] = Pov_Accumulator_GetIndex(&accumulator->perspective[WHITE], piece, square, wkSq, flip[WHITE]);
    accumulator->perspective[BLACK].NNUEAdd[accumulator->perspective[BLACK].NNUEAdd_size++] = Pov_Accumulator_GetIndex(&accumulator->perspective[BLACK], piece, square, bkSq, flip[BLACK]);
}

inline void Accumulator_AppendSubIndex(struct Accumulator* accumulator, int piece, int square, const int wkSq, const int bkSq, bool flip[2]) {
    assert(accumulator->perspective[WHITE].NNUESub_size() <= 1);
    assert(accumulator->perspective[BLACK].NNUESub_size() <= 1);
    accumulator->perspective[WHITE].NNUESub[accumulator->perspective[WHITE].NNUESub_size++] = Pov_Accumulator_GetIndex(&accumulator->perspective[WHITE], piece, square, wkSq, flip[WHITE]);
    accumulator->perspective[BLACK].NNUESub[accumulator->perspective[BLACK].NNUESub_size++] = Pov_Accumulator_GetIndex(&accumulator->perspective[BLACK], piece, square, bkSq, flip[BLACK]);
}

inline void Accumulator_ClearAddIndex(struct Accumulator* accumulator) {
    accumulator->perspective[WHITE].NNUEAdd_size = 0;
    accumulator->perspective[BLACK].NNUEAdd_size = 0;
}

inline void Accumulator_ClearSubIndex(struct Accumulator* accumulator) {
    accumulator->perspective[WHITE].NNUESub_size = 0;
    accumulator->perspective[BLACK].NNUESub_size = 0;
}

void NNUE_init();
void NNUE_accumulate(struct Accumulator* board_accumulator, struct Position* pos);
void NNUE_update(struct Accumulator* acc, struct Position* pos);
int32_t NNUE_ActivateFTAndAffineL1(const int16_t* us, const int16_t* them, const int16_t* weights, const int16_t bias);
int32_t NNUE_output(struct Accumulator* const board_accumulator, const int stm);
