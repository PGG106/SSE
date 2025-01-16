#pragma once

#include "position.h"
#include "types.h"

// 10 bytes:
// 2 for move
// 2 for score
// 2 for key
// 1 for depth
// 1 for age + bound + PV
PACK(struct TTEntry {
    PackedMove move;
    int16_t score;
    TTKey ttKey;
    uint8_t depth;
    uint8_t ageBoundPV; // lower 2 bits is bound, 3rd bit is PV, next 5 is age
});

#if !NOSTDLIB
static_assert(sizeof(struct TTEntry) == 8);
#endif

struct TTable {
    struct TTEntry *pTable;
    uint64_t numEntries;
    size_t paddedSize;
    uint8_t age;
};

extern struct TTable TT;

static const uint8_t MAX_AGE = 1 << 5; // must be power of 2
static const uint8_t AGE_MASK = MAX_AGE - 1;

void* AlignedMalloc(size_t size, size_t alignment);

void TTEntry_init(struct TTEntry* const tte);

void ClearTT();
// Initialize an TT of size MB
void InitTT(uint64_t MB);

bool ProbeTTEntry(const ZobristKey posKey, struct TTEntry* tte);

void StoreTTEntry(const ZobristKey key, const PackedMove move, int score, const int bound, const int depth, const bool pv, const bool wasPV);

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
