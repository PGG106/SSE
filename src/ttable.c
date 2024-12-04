#include "all.h"

struct TTable TT;

#include "all.h"

void* AlignedMalloc(size_t size, size_t alignment) {
    return malloc(size);
}

void ClearTT() {
    for (uint64_t i = 0; i < TT.paddedSize / sizeof(struct TTBucket); ++i) {
        TTBucket_init(&TT.pTable[i]);
    }
    TT.age = 1;
}

void InitTT(uint64_t MB) {
    const uint64_t ONE_KB = 1024;
    const uint64_t ONE_MB = ONE_KB * 1024;
    const uint64_t hashSize = ONE_MB * MB;
    TT.numBuckets = (hashSize / sizeof(struct TTBucket)) - 3;

    // We align to 2MB on Linux (huge pages), otherwise assume that 4KB is the page size
#if defined(USE_MADVISE)
    const uint64_t alignment = 2 * ONE_MB;
#else
    const uint64_t alignment = 4 * ONE_KB;
#endif

    // Pad the TT by using a ceil div and a multiply to get the size to be a multiple of `alignment`
    TT.paddedSize = (TT.numBuckets * sizeof(struct TTBucket) + alignment - 1) / alignment * alignment;
    TT.pTable = (struct TTBucket*)(AlignedMalloc(TT.paddedSize, alignment));

    // On linux we request huge pages to make use of the 2MB alignment
#if defined(USE_MADVISE)
    madvise(TT.pTable, TT.paddedSize, MADV_HUGEPAGE);
#endif

    ClearTT();
}

void TTEntry_init(struct TTEntry* const tte) {
    tte->move = NOMOVE;
    tte->score = SCORE_NONE;
    tte->eval = SCORE_NONE;
    tte->ttKey = 0;
    tte->depth = 0;
    tte->ageBoundPV = HFNONE;
}

void TTBucket_init(struct TTBucket* const ttb) {
    for (int i = 0; i < ENTRIES_PER_BUCKET; i++) {
        TTEntry_init(&ttb->entries[i]);
    }
}

bool ProbeTTEntry(const ZobristKey posKey, struct TTEntry* tte) {
    const uint64_t index = Index(posKey);
    struct TTBucket* bucket = &TT.pTable[index];
    for (int i = 0; i < ENTRIES_PER_BUCKET; i++) {
        *tte = bucket->entries[i];
        if (tte->ttKey == (TTKey)posKey) {
            return true;
        }
    }
    return false;
}

void StoreTTEntry(const ZobristKey key, const PackedMove move, int score, int eval, const int bound, const int depth, const bool pv, const bool wasPV) {
    // Calculate index based on the position key and get the entry that already fills that index
    const uint64_t index = Index(key);
    const TTKey key16 = (TTKey)key;
    const uint8_t TTAge = TT.age;
    struct TTBucket* bucket = &TT.pTable[index];
    struct TTEntry* tte = &bucket->entries[0];
    for (int i = 0; i < ENTRIES_PER_BUCKET; i++) {
        struct TTEntry* entry = &bucket->entries[i];

        if (entry->ttKey == key16) {
            tte = entry;
            break;
        }

        if (tte->depth - ((MAX_AGE + TTAge - AgeFromTT(tte->ageBoundPV)) & AGE_MASK) * 4
    > entry->depth - ((MAX_AGE + TTAge - AgeFromTT(entry->ageBoundPV)) & AGE_MASK) * 4) {
            tte = entry;
        }
    }

    // Replacement strategy taken from Stockfish
    // Preserve any existing move for the same position
    if (move || key16 != tte->ttKey)
        tte->move = move;

    // Overwrite less valuable entries (cheapest checks first)
    if (bound == HFEXACT
        || key16 != tte->ttKey
        || depth + 5 + 2 * pv > tte->depth
        || AgeFromTT(tte->ageBoundPV) != TTAge) {
        tte->ttKey = key16;
        tte->ageBoundPV = PackToTT(bound, wasPV, TTAge);
        tte->score = (int16_t)score;
        tte->eval = (int16_t)eval;
        tte->depth = (uint8_t)depth;
    }
}

uint64_t Index(const ZobristKey posKey) {
#ifdef __SIZEOF_INT128__
    return (uint64_t)((((__uint128_t)(posKey) * (__uint128_t)(TT.numBuckets)) >> 64));
#else
    // Workaround to use the correct bits when indexing the TT on a platform with no int128 support, code provided by Nanopixel
    uint64_t xlo = (uint32_t)posKey;
    uint64_t xhi = posKey >> 32;
    uint64_t nlo = (uint32_t)TT.numBuckets;
    uint64_t nhi = TT.numBuckets >> 32;
    uint64_t c1 = (xlo * nlo) >> 32;
    uint64_t c2 = (xhi * nlo) + c1;
    uint64_t c3 = (xlo * nhi) + (uint32_t)c2;

    return xhi * nhi + (c2 >> 32) + (c3 >> 32);
#endif
}

// prefetches the data in the given address in l1/2 cache in a non blocking way.
void prefetch(const void* addr) {
#if defined(__INTEL_COMPILER) || defined(_MSC_VER)
    _mm_prefetch((char*)addr, _MM_HINT_T0);
#else
    __builtin_prefetch(addr);
#endif
}

void TTPrefetch(const ZobristKey posKey) {
    prefetch(&TT.pTable[Index(posKey)].entries[0]);
}


int ScoreToTT(int score, int ply) {
    if (score > MATE_FOUND)
        score += ply;
    else if (score < -MATE_FOUND)
        score -= ply;

    return score;
}

int ScoreFromTT(int score, int ply) {
    if (score > MATE_FOUND)
        score -= ply;
    else if (score < -MATE_FOUND)
        score += ply;

    return score;
}

PackedMove MoveToTT(Move move) {
    return (move & 0xffff);
}

Move MoveFromTT(struct Position* pos, PackedMove packed_move) {
    // It's important to preserve a move being null even it's being unpacked
    if (packed_move == NOMOVE)
        return NOMOVE;

    const int piece = Position_PieceOn(pos, From(packed_move));
    return packed_move | (piece << 16);
}

uint8_t BoundFromTT(uint8_t ageBoundPV) {
    return ageBoundPV & 0b00000011;
}

bool FormerPV(uint8_t ageBoundPV) {
    return ageBoundPV & 0b00000100;
}

uint8_t AgeFromTT(uint8_t ageBoundPV) {
    return (ageBoundPV & 0b11111000) >> 3;
}

uint8_t PackToTT(uint8_t bound, bool wasPV, uint8_t age) {
    return (uint8_t)(bound + (wasPV << 2) + (age << 3));
}

void UpdateTableAge() {
    TT.age = (TT.age + 1) & AGE_MASK;
}
