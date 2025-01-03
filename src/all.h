#pragma once

#ifdef _MSC_VER
#define SMALL
#else
#define SMALL __attribute__((optimize("Os")))
#endif

#if !NOSTDLIB
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <stdall.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <poll.h>

uint64_t GetTimeMs();
#else

typedef long bool;
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef int64_t ssize_t;
typedef uint64_t size_t;

enum {
    stdin = 0,
    stdout = 1,
    stderr = 2
};

#define NULL ((void *)0)
#define true 1
#define false 0

#if !NDEBUG
#define assert(condition)                                                      \
  if (!(condition)) {                                                          \
    printf("Assert failed on line %i: ", __LINE__);                            \
    puts(#condition "\n");                                                     \
    _sys(60, 1, 0, 0);                                                         \
  }
#else
#define assert(condition)
#endif

#define printf(format, ...) _printf(format, (size_t[]){__VA_ARGS__})

struct pollfd {
    int   fd;      // file descriptor to monitor
    short events;  // events of interest
    short revents; // events that occurred
};

#define POLLIN 1

ssize_t _sys(ssize_t call, ssize_t arg1, ssize_t arg2, ssize_t arg3);
void exit(const int returnCode);
int strlen(const char* const restrict string);
void puts(const char* const restrict string);
void puts_nonewline(const char* const restrict string);
void fflush(int fd);
bool strcmp(const char* restrict lhs, const char* restrict rhs);
int atoi(const char* restrict string);
void _printf(const char* format, const size_t* args);
size_t GetTimeMs();
void* mmap(void* addr, size_t len, size_t prot, size_t flags, size_t fd, size_t offset);
void* malloc(size_t len);
ssize_t open(const char* const restrict pathname, const int flags, const int mode);
ssize_t fopen(const char* const restrict pathname, const char* const restrict mode);
char* strstr(const char* haystack, const char* needle);
void* memset(void* ptr, int value, size_t n);
void* memcpy(void* dest, const void* src, size_t n);
double log(double x);
char* fgets(char* string0, int count, int file);
int poll(struct pollfd* fds, unsigned int nfds, int timeout);
#endif

#define NAME "Alexandria-7.1.1"

extern int see_margin[64][2];

static const char* start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

typedef uint64_t Bitboard;
typedef uint16_t TTKey;
typedef uint64_t ZobristKey;
typedef uint32_t Move;
typedef uint16_t PackedMove;

static const Move NOMOVE = 0;
#define MAXPLY 128
static const int MAXDEPTH = MAXPLY;
static const int MATE_SCORE = 32000;
static const int MATE_FOUND = MATE_SCORE - MAXPLY;
static const int SCORE_NONE = 32001;
static const int MAXSCORE = 32670;
static const Bitboard fullCheckmask = 0xFFFFFFFFFFFFFFFF;

// board squares
enum {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1, no_sq
};

// Lookup to get the rank of a square
static const uint8_t get_rank[64] = { 7, 7, 7, 7, 7, 7, 7, 7,
                               6, 6, 6, 6, 6, 6, 6, 6,
                               5, 5, 5, 5, 5, 5, 5, 5,
                               4, 4, 4, 4, 4, 4, 4, 4,
                               3, 3, 3, 3, 3, 3, 3, 3,
                               2, 2, 2, 2, 2, 2, 2, 2,
                               1, 1, 1, 1, 1, 1, 1, 1,
                               0, 0, 0, 0, 0, 0, 0, 0 };

// Lookup to get the file of a square
static uint8_t get_file(const int square) {
    return square % 8;
}

// Lookup to get the diagonal of a square
static const uint8_t get_diagonal[64] = { 14, 13, 12, 11, 10,  9,  8,  7,
                                   13, 12, 11, 10,  9,  8,  7,  6,
                                   12, 11, 10,  9,  8,  7,  6,  5,
                                   11, 10,  9,  8,  7,  6,  5,  4,
                                   10,  9,  8,  7,  6,  5,  4,  3,
                                    9,  8,  7,  6,  5,  4,  3,  2,
                                    8,  7,  6,  5,  4,  3,  2,  1,
                                    7,  6,  5,  4,  3,  2,  1,  0 };

#define get_antidiagonal(sq) (get_rank[sq] + get_file(sq))

// encode pieces
enum {
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK,
    EMPTY = 14
};

// sides to move (colors)
enum {
    WHITE,
    BLACK,
    BOTH
};

enum {
    WKCA = 1,
    WQCA = 2,
    BKCA = 4,
    BQCA = 8
};

enum {
    HFNONE,
    HFUPPER,
    HFLOWER,
    HFEXACT
};

// piece types
enum {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

// Lookup to get the color from a piece
static const int Color[12] = { WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
                            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };

static const int PieceType[12] = { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
                                PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

// Contains the material Values of the pieces
static const int SEEValue[15] = { 100, 422, 422, 642, 1015, 0,
                               100, 422, 422, 642, 1015, 0, 0, 0, 0 };

// generate pawn attacks
Bitboard MaskPawnAttacks(int side, int square);

// generate knight attacks
Bitboard MaskKnightAttacks(int square);

// generate king attacks
Bitboard MaskKingAttacks(int square);

// pawn attacks table [side][square]
extern Bitboard pawn_attacks[2][64];

// knight attacks table [square]
extern Bitboard knight_attacks[64];

// king attacks table [square]
extern Bitboard king_attacks[64];

Bitboard pieceAttacks(int piecetype, int pieceSquare, Bitboard occ);

// set occupancies
Bitboard SetOccupancy(int index, int bits_in_mask, Bitboard attack_mask);

// set/get/pop bit macros
static void set_bit(Bitboard* bitboard, const int square) { *bitboard |= (1ULL << square); }
inline int get_bit(const Bitboard bitboard, const int square) { return bitboard & (1ULL << square); }
inline void pop_bit(Bitboard* bitboard, const int square) { *bitboard &= ~(1ULL << square); }

static int GetLsbIndex(Bitboard bitboard) {
    assert(bitboard);
    return __builtin_ctzll(bitboard);;
}

inline int popLsb(Bitboard* bitboard) {
    assert(bitboard);
    int square = GetLsbIndex(*bitboard);
    *bitboard &= *bitboard - 1;
    return square;
}

inline int CountBits(Bitboard bitboard) {
    return __builtin_popcountll(bitboard);
}

#if !NOSTDLIB

#include "all.h"

#if defined(USE_SIMD)
#include <immintrin.h>
#endif

#if defined(USE_AVX512)
typedef __m512i vepi16;
typedef __m512i vepi32;

inline vepi16  vec_zero_epi16() { return _mm512_setzero_si512(); }
inline vepi32  vec_zero_epi32() { return _mm512_setzero_si512(); }
inline vepi16  vec_set1_epi16(const int16_t n) { return _mm512_set1_epi16(n); }
inline vepi16  vec_loadu_epi(const vepi16* src) { return _mm512_loadu_si512(src); }
inline vepi16  vec_max_epi16(const vepi16 vec0, const vepi16 vec1) { return _mm512_max_epi16(vec0, vec1); }
inline vepi16  vec_min_epi16(const vepi16 vec0, const vepi16 vec1) { return _mm512_min_epi16(vec0, vec1); }
inline vepi16  vec_mullo_epi16(const vepi16 vec0, const vepi16 vec1) { return _mm512_mullo_epi16(vec0, vec1); }
inline vepi32  vec_madd_epi16(const vepi16 vec0, const vepi16 vec1) { return _mm512_madd_epi16(vec0, vec1); }
inline vepi32  vec_add_epi32(const vepi32 vec0, const vepi32 vec1) { return _mm512_add_epi32(vec0, vec1); }
inline int32_t vec_reduce_add_epi32(const vepi32 vec) { return _mm512_reduce_add_epi32(vec); }

#elif defined(USE_AVX2)
typedef __m256i vepi16;
typedef __m256i vepi32;

inline vepi16  vec_zero_epi16() { return _mm256_setzero_si256(); }
inline vepi32  vec_zero_epi32() { return _mm256_setzero_si256(); }
inline vepi16  vec_set1_epi16(const int16_t n) { return _mm256_set1_epi16(n); }
inline vepi16  vec_loadu_epi(const vepi16* src) { return _mm256_loadu_si256(src); }
inline vepi16  vec_max_epi16(const vepi16 vec0, const vepi16 vec1) { return _mm256_max_epi16(vec0, vec1); }
inline vepi16  vec_min_epi16(const vepi16 vec0, const vepi16 vec1) { return _mm256_min_epi16(vec0, vec1); }
inline vepi16  vec_mullo_epi16(const vepi16 vec0, const vepi16 vec1) { return _mm256_mullo_epi16(vec0, vec1); }
inline vepi32  vec_madd_epi16(const vepi16 vec0, const vepi16 vec1) { return _mm256_madd_epi16(vec0, vec1); }
inline vepi32  vec_add_epi32(const vepi32 vec0, const vepi32 vec1) { return _mm256_add_epi32(vec0, vec1); }
inline int32_t vec_reduce_add_epi32(const vepi32 vec) {
    // Split the __m256i into 2 __m128i vectors, and add them together
    __m128i lower128 = _mm256_castsi256_si128(vec);
    __m128i upper128 = _mm256_extracti128_si256(vec, 1);
    __m128i sum128 = _mm_add_epi32(lower128, upper128);

    // Store the high 64 bits of sum128 in the low 64 bits of this vector,
    // then add them together (only the lowest 64 bits are relevant)
    __m128i upper64 = _mm_unpackhi_epi64(sum128, sum128);
    __m128i sum64 = _mm_add_epi32(upper64, sum128);

    // Store the high 32 bits of the relevant part of sum64 in the low 32 bits of the vector,
    // and then add them together (only the lowest 32 bits are relevant)
    __m128i upper32 = _mm_shuffle_epi32(sum64, 1);
    __m128i sum32 = _mm_add_epi32(upper32, sum64);

    // Return the bottom 32 bits of sum32
    return _mm_cvtsi128_si32(sum32);
}
#endif
#endif

// Net arch: (768xINPUT_BUCKETS -> L1_SIZE)x2 -> 1xOUTPUT_BUCKETS
#define NUM_INPUTS 768
#define INPUT_BUCKETS 1
#define L1_SIZE 80
#define OUTPUT_BUCKETS 8

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

inline bool Pov_Accumulator_isClean(const struct Pov_Accumulator* accumulator) {
    return accumulator->NNUEAdd_size == 0;
}

// final total accumulator that holds the 2 povs
struct Accumulator {
    struct Pov_Accumulator perspective[2];
};

inline void Accumulator_AppendAddIndex(struct Accumulator* accumulator, int piece, int square, const int wkSq, const int bkSq, bool flip[2]) {
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
int32_t NNUE_output(struct Accumulator* const board_accumulator, const int stm, const int outputBucket);

struct Position;

struct ScoredMove {
    Move move;
    int score;
};

// move list structure
struct MoveList {
    struct ScoredMove moves[256];
    int count;
};

enum Movetype {
    Quiet, doublePush, KSCastle,
    QSCastle, Capture, enPassant,
    knightPromo = 8, bishopPromo, rookPromo, queenPromo,
    knightCapturePromo, bishopCapturePromo, rookCapturePromo, queenCapturePromo
};

inline Move encode_move(const int source, const int target, const int piece, const int movetype) {
    return (source) | (target << 6) | (movetype << 12) | (piece << 16);
}

static int From(const Move move) { return move & 0x3F; }
static int To(const Move move) { return ((move & 0xFC0) >> 6); }
static int FromTo(const Move move) { return move & 0xFFF; }
inline int Piece(const Move move) { return ((move & 0xF0000) >> 16); }
static int PieceTo(const Move move) { return (Piece(move) << 6) | To(move); }
inline int PieceTypeTo(const Move move) { return (PieceType[Piece(move)] << 6) | To(move); }
inline int GetMovetype(const Move move) { return ((move & 0xF000) >> 12); }
static int getPromotedPiecetype(const Move move) { return (GetMovetype(move) & 3) + 1; }
inline bool isEnpassant(const Move move) { return GetMovetype(move) == enPassant; }
inline bool isDP(const Move move) { return GetMovetype(move) == doublePush; }
inline bool isCastle(const Move move) {
    return (GetMovetype(move) == KSCastle) || (GetMovetype(move) == QSCastle);
}
inline bool isCapture(const Move move) { return GetMovetype(move) & Capture; }
inline bool isQuiet(const Move move) { return !isCapture(move); }
static bool isPromo(const Move move) { return GetMovetype(move) & 8; }
// Shorthand for captures + any promotion no matter if quiet or not 
static bool isTactical(const Move move) { return isCapture(move) || isPromo(move); }

struct SearchInfo;

// Parse a move from algebraic notation to the engine's internal encoding
Move ParseMove(const char* move_string, struct Position* pos);
// parse UCI "position" command
void ParsePosition(const char* command, struct Position* pos);

// parse UCI "go" command
bool ParseGo(const char* const line, struct SearchInfo* info, struct Position* pos);

// main UCI loop
void UciLoop();

#ifdef __GNUC__
#define PACK(__Declaration__) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK(__Declaration__) \
    __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#endif

struct BoardState {
    int castlePerm;
    int capture;
    int enPas;
    int fiftyMove;
    int plyFromNull;
    Bitboard checkers;
    Bitboard checkMask;
    Bitboard pinned;
}; // stores a move and the state of the game before that move is made
// for rollback purposes

struct Position {
    int16_t pieces[64]; // array that stores for every square of the board what piece is on it

    int side; // what side has to move

    struct BoardState state;
    int hisPly; // total number of halfmoves
    // unique  hashkey  that encodes a board position
    ZobristKey posKey;
    ZobristKey pawnKey;
    ZobristKey whiteNonPawnKey;
    ZobristKey blackNonPawnKey;
    // stores the state of the board  rollback purposes
    int historyStackHead;
    struct BoardState history[MAXPLY];

    // Stores the zobrist keys of all the positions played in the game + the current search instance, used for 3-fold
    ZobristKey played_positions[2048];
    int played_positions_size;
    // std::vector<ZobristKey> played_positions = {}; // TODO: Consider restoring vector-like functionality

    // Occupancies bitboards based on piece and side
    Bitboard bitboards[12];
    Bitboard occupancies[2];

    struct Accumulator accumStack[MAXPLY];
    int accumStackHead;
};

// Retrieve a generic piece (useful when we don't know what type of piece we are dealing with
inline Bitboard Position_GetPieceColorBB(struct Position const* const position, const int piecetype, const int color) {
    return position->bitboards[piecetype + color * 6];
}

inline struct Accumulator* Position_AccumulatorTop(struct Position* position) {
    assert(position->accumStackHead <= MAXPLY);
    return &position->accumStack[position->accumStackHead - 1];
}

inline Bitboard Position_Occupancy(const struct Position* const pos, const int occupancySide) {
    assert(occupancySide >= WHITE && occupancySide <= BOTH);
    if (occupancySide == BOTH)
        return pos->occupancies[WHITE] | pos->occupancies[BLACK];
    else
        return pos->occupancies[occupancySide];
}

inline int Position_PieceCount(const struct Position* const pos) {
    return CountBits(Position_Occupancy(pos, BOTH));
}

static int Position_PieceOn(const struct Position* const pos, const int square) {
    assert(square >= 0 && square <= 63);
    return pos->pieces[square];
}

inline ZobristKey Position_getPoskey(const struct Position* const pos) {
    return pos->posKey;
}

inline int Position_get50MrCounter(const struct Position* const pos) {
    return pos->state.fiftyMove;
}

static int Position_getCastlingPerm(const struct Position* const pos) {
    return pos->state.castlePerm;
}

static int Position_getEpSquare(const struct Position* const pos) {
    return pos->state.enPas;
}

inline int Position_getPlyFromNull(const struct Position* const pos) {
    return pos->state.plyFromNull;
}

static Bitboard Position_getCheckers(const struct Position* const pos) {
    return pos->state.checkers;
}

inline Bitboard Position_getCheckmask(const struct Position* const pos) {
    return pos->state.checkMask;
}

inline Bitboard Position_getPinnedMask(const struct Position* const pos) {
    return pos->state.pinned;
}

inline int Position_getCapturedPiece(const struct Position* const pos) {
    return pos->history[pos->historyStackHead].capture;
}

inline void Position_ChangeSide(struct Position* const pos) {
    pos->side ^= 1;
}

extern Bitboard SQUARES_BETWEEN_BB[64][64];

// Hold the data from the uci input to set search parameters and some search data to populate the uci output
struct SearchInfo {
    // search start time
    uint64_t starttime;
    // search time initial lower bound if present
    uint64_t stoptimeBaseOpt;
    // search time scaled lower bound if present
    uint64_t stoptimeOpt;
    // search time upper bound if present
    uint64_t stoptimeMax;
    // max depth to reach for depth limited searches
    int depth;
    int seldepth;
    // types of search limits
    bool timeset;

    uint64_t nodes;

    bool stopped;
};

// castling rights update constants
extern const int castling_rights[64];

// convert squares to coordinates
extern const char* const square_to_coordinates[64];

void ResetBoard(struct Position* pos);
void ResetInfo(struct SearchInfo* info);
ZobristKey GeneratePosKey(const struct Position* const pos);
ZobristKey GeneratePawnKey(const struct Position* pos);
ZobristKey GenerateNonPawnKey(const struct Position* pos, int side);

Bitboard RayBetween(int square1, int square2);
void UpdatePinsAndCheckers(struct Position* pos, const int side);

// Get on what square of the board the king of color c resides
int KingSQ(const struct Position* pos, const int c);

// Parse a string of moves in coordinate format and plays them
void parse_moves(const char* moves, struct Position* pos);

// Retrieve a generic piece (useful when we don't know what type of piece we are dealing with
Bitboard GetPieceBB(const struct Position* pos, const int piecetype);

// parse FEN string
void ParseFen(const char* command, struct Position* pos);

// Return a piece based on the type and the color
int GetPiece(const int piecetype, const int color);

// Returns the piece_type of a piece
int GetPieceType(const int piece);

// Returns true if side has at least one piece on the board that isn't a pawn, false otherwise
bool BoardHasNonPawns(const struct Position* pos, const int side);

ZobristKey keyAfter(const struct Position* pos, const Move move);

void saveBoardState(struct Position* pos);

void restorePreviousBoardState(struct Position* pos);

// starts a bench for alexandria, searching a set of positions up to a set depth
void StartBench(int depth);

// if we don't have enough material to mate consider the position a draw
bool MaterialDraw(const struct Position* pos);

int ScaleMaterial(const struct Position* pos, int eval);
int EvalPositionRaw(struct Position* pos);

// position evaluation
int EvalPosition(struct Position* pos);

extern const Bitboard file_bbs[8];
extern const Bitboard rank_bbs[8];
extern const Bitboard diagonal_bbs[15];
extern const Bitboard antidiagonal_bbs[15];

Bitboard ReverseBits(Bitboard bitboard);
Bitboard MaskedSlide(const Bitboard allPieces, const Bitboard pieceBitboard, const Bitboard mask);
// get bishop attacks
Bitboard GetBishopAttacks(int square, Bitboard occupancy);
// get rook attacks
Bitboard GetRookAttacks(int square, Bitboard occupancy);
// get queen attacks
Bitboard GetQueenAttacks(const int square, Bitboard occupancy);

// print move (for UCI purposes)
void PrintMove(const Move move);
bool next_token(const char* str, int* index, char* token);

void ClearPiece(const bool UPDATE, const int piece, const int from, struct Position* pos);

void AddPiece(const bool UPDATE, const int piece, const int to, struct Position* pos);

void UpdateCastlingPerms(struct Position* pos, int source_square, int target_square);

void MakeMove(const bool UPDATE, const Move move, struct Position* pos);
// Reverts the previously played move
void UnmakeMove(const Move move, struct Position* pos);
// makes a null move (a move that doesn't move any piece)
void MakeNullMove(struct Position* pos);
// Reverts the previously played null move
void TakeNullMove(struct Position* pos);

struct MoveList;

enum MovegenType {
    MOVEGEN_NOISY = 0b01,
    MOVEGEN_QUIET = 0b10,
    MOVEGEN_ALL = 0b11
};

// is the square given in input attacked by the current given side
bool IsSquareAttacked(const struct Position* pos, const int square, const int side);

// function that adds a (not yet scored) move to a move list
void AddMoveNonScored(const Move move, struct MoveList* list);

// function that adds an (already-scored) move to a move list
void AddMoveScored(const Move move, const int score, struct MoveList* list);

// Check for move pseudo-legality
bool IsPseudoLegal(const struct Position* pos, const Move move);

// Check for move legality
bool IsLegal(struct Position* pos, const Move move);

// generate moves
void GenerateMoves(struct MoveList* move_list, struct Position* pos, enum MovegenType type);

// convert ASCII character pieces to encoded constants
int char_pieces(const char ch);

// Map promoted piece to the corresponding ASCII character
char promoted_pieces(const int piece);

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
    struct TTBucket* pTable;
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

struct Position;
struct SearchData;
struct SearchStack;
struct MoveList;

const static int HH_MAX = 8192;
const static int CH_MAX = 16384;
const static int CAPTHIST_MAX = 16384;
const static int CORRHIST_WEIGHT_SCALE = 1024;
const static int CORRHIST_GRAIN = 256;
#define CORRHIST_SIZE 16384
const static int CORRHIST_MAX = 16384;

// Functions used to update the history heuristics
void UpdateHistories(const struct Position* pos, struct SearchData* sd, struct SearchStack* ss, const int depth, const Move bestMove, const struct MoveList* quietMoves, const struct MoveList* noisyMoves, const bool rootNode);

// Fuction that returns the history bonus
int history_bonus(const int depth);

// Getters for the history heuristics
int GetHHScore(const struct Position* pos, const struct SearchData* sd, const Move move);
int GetCHScore(const struct SearchStack* ss, const Move move);
int GetSingleCHScore(const struct SearchStack* ss, const Move move, const int offset);
int GetCapthistScore(const struct Position* pos, const struct SearchData* sd, const Move move);
int GetHistoryScore(const struct Position* pos, const struct SearchData* sd, const Move move, const struct SearchStack* ss);
// Clean all the history tables
void CleanHistories(struct SearchData* sd);
// Updates history heuristics for a single move
void updateHHScore(const struct Position* pos, struct SearchData* sd, const Move move, int bonus);
void updateCHScore(struct SearchStack* ss, const Move move, const int bonus);
void updateCapthistScore(const struct Position* pos, struct SearchData* sd, const Move move, int bonus);

// Corrhist stuff
void updateCorrHistScore(const struct Position* pos, struct SearchData* sd, const struct SearchStack* ss, const int depth, const int diff);
int  adjustEvalWithCorrHist(const struct Position* pos, const struct SearchData* sd, const int rawEval);

struct SearchStack {
    // don't init. search will init before entering the negamax method
    int16_t staticEval;
    Move excludedMove;
    Move move;
    int ply;
    Move searchKiller;
    int doubleExtensions;
    short(*contHistEntry)[6 * 64];
};

struct SearchData {
    short searchHistory[2][64 * 64];
    short captHist[12 * 64][6];
    short pawnCorrHist[2][CORRHIST_SIZE];
    short whiteNonPawnCorrHist[2][CORRHIST_SIZE];
    short blackNonPawnCorrHist[2][CORRHIST_SIZE];
    short contHist[2][6 * 64][6 * 64];
};

// a collection of all the data a thread needs to conduct a search
struct ThreadData {
    struct Position pos;
    struct SearchData sd;
    struct SearchInfo info;
    int RootDepth;
    int nmpPlies;
    uint64_t nodeSpentTable[64 * 64];
    bool pondering;
};

extern Move return_bestmove;

void init_thread_data(struct ThreadData* td);

// ClearForSearch handles the cleaning of the thread data from a clean state
void ClearForSearch(struct ThreadData* td);

// Starts the search process, this is ideally the point where you can start a multithreaded search
void RootSearch(int depth, struct ThreadData* td);

// SearchPosition is the actual function that handles the search, it sets up the variables needed for the search , calls the Negamax function and handles the console output
void SearchPosition(int start_depth, int final_depth, struct ThreadData* td);

// Sets up aspiration windows and starts a Negamax search
int AspirationWindowSearch(int prev_eval, int depth, struct ThreadData* td);

int Negamax(int alpha, int beta, int depth, const bool cutNode, struct ThreadData* td, struct SearchStack* ss);

int Quiescence(int alpha, int beta, struct ThreadData* td, struct SearchStack* ss);

// inspired by the Weiss engine
bool SEE(const struct Position* pos, const int move, const int threshold);

// Checks if the current position is a draw
bool IsDraw(struct Position* pos);

bool IsRepetition(const struct Position* pos); // remove this later, make static
bool Is50MrDraw(struct Position* pos); // remove this later, make static

enum {
    PICK_TT,
    GEN_NOISY,
    PICK_GOOD_NOISY,
    PICK_KILLER,
    GEN_QUIETS,
    PICK_QUIETS,
    GEN_BAD_NOISY,
    PICK_BAD_NOISY
};

enum MovepickerType {
    SEARCH,
    QSEARCH
};

struct Movepicker {
    enum MovepickerType movepickerType;
    struct Position* pos;
    struct SearchData* sd;
    struct SearchStack* ss;
    struct MoveList moveList;
    struct MoveList badCaptureList;
    Move ttMove;
    Move killer;
    int idx;
    int stage;
};

void InitMP(struct Movepicker* mp, struct Position* pos, struct SearchData* sd, struct SearchStack* ss, const Move ttMove, const enum MovepickerType movepickerType, const bool rootNode);
Move NextMove(struct Movepicker* mp, const bool skip);

void Optimum(struct SearchInfo* info, int time, int inc);
bool StopEarly(const struct SearchInfo* info);
bool TimeOver(const struct SearchInfo* info);
void ScaleTm(struct ThreadData* td);

extern Bitboard PieceKeys[12][64];
extern Bitboard enpassant_keys[64];
extern Bitboard SideKey;
extern Bitboard CastleKeys[16];

void InitNewGame(struct ThreadData* td);

// init slider piece's attack tables
void InitAttackTables();

void InitAll();
