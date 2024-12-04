#pragma once

#include "position.h"
#include "all.h"

#define ENTRIES_PER_BUCKET 3

// 10 bytes:
// 2 for move
// 2 for score
// 2 for eval
// 2 for key
// 1 for depth
// 1 for age + bound + PV
PACK(struct TTEntry {
    PackedMove move;
    int16_t score;
    int16_t eval;
    TTKey ttKey;
    uint8_t depth;
    uint8_t ageBoundPV; // lower 2 bits is bound, 3rd bit is PV, next 5 is age
});

// Packs the 10-byte entries into 32-byte buckets
// 3 entries per bucket with 2 bytes of padding
struct TTBucket {
    struct TTEntry entries[ENTRIES_PER_BUCKET];
    uint16_t padding;
} __attribute__((aligned(32)));

#if !NOSTDLIB
static_assert(sizeof(struct TTEntry) == 10);
static_assert(sizeof(struct TTBucket) == 32);
#endif

struct TTable {
    struct TTBucket *pTable;
    uint64_t numBuckets;
    size_t paddedSize;
    uint8_t age;
};

extern struct TTable TT;

static const uint8_t MAX_AGE = 1 << 5; // must be power of 2
static const uint8_t AGE_MASK = MAX_AGE - 1;

void* AlignedMalloc(size_t size, size_t alignment);

void AlignedFree(void* src);

void TTEntry_init(struct TTEntry* const tte);
void TTBucket_init(struct TTBucket* const ttb);

void ClearTT();
// Initialize an TT of size MB
void InitTT(uint64_t MB);

bool ProbeTTEntry(const ZobristKey posKey, struct TTEntry* tte);

void StoreTTEntry(const ZobristKey key, const PackedMove move, int score, int eval, const int bound, const int depth, const bool pv, const bool wasPV);

uint64_t Index(const ZobristKey posKey);

void TTPrefetch(const ZobristKey posKey);

int ScoreToTT(int score, int ply);

int ScoreFromTT(int score, int ply);

PackedMove MoveToTT(Move move);

Move MoveFromTT(struct Position* pos, PackedMove packed_move);

uint8_t BoundFromTT(uint8_t ageBoundPV);

bool FormerPV(uint8_t ageBoundPV);

uint8_t AgeFromTT(uint8_t ageBoundPV);

uint8_t PackToTT(uint8_t bound, bool wasPV, uint8_t age);

void UpdateTableAge();
