#pragma once

#if !NOSTDLIB
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/mman.h>

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
double log(double x);
int read(int fd, void* data, int count);
char* fgets(char* string0, int count, int file);

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
extern const uint8_t get_rank[64];

// Lookup to get the file of a square
uint8_t get_file(const int square);

// Lookup to get the diagonal of a square
extern const uint8_t get_diagonal[64];

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
extern const int Color[12];
extern const int PieceType[12];
// Contains the material Values of the pieces
extern const int SEEValue[15];

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

// starts a bench for alexandria, searching a set of positions up to a set depth
void StartBench(int depth);

// set/get/pop bit macros
void set_bit(Bitboard* bitboard, const int square);
int get_bit(const Bitboard bitboard, const int square);
void pop_bit(Bitboard* bitboard, const int square);
int GetLsbIndex(Bitboard bitboard);
int popLsb(Bitboard* bitboard);
int CountBits(Bitboard bitboard);

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

Move encode_move(const int source, const int target, const int piece, const int movetype);

int From(const Move move);
int To(const Move move);
int FromTo(const Move move);
int Piece(const Move move);
int PieceTo(const Move move);
int PieceTypeTo(const Move move);
int GetMovetype(const Move move);
int getPromotedPiecetype(const Move move);
bool isEnpassant(const Move move);
bool isDP(const Move move);
bool isCastle(const Move move);
bool isCapture(const Move move);
bool isQuiet(const Move move);
bool isPromo(const Move move);
// Shorthand for captures + any promotion no matter if quiet or not 
bool isTactical(const Move move);

struct Position;
struct SearchInfo;

// Parse a move from algebraic notation to the engine's internal encoding
Move ParseMove(const char* move_string, struct Position* pos);
// parse UCI "position" command
void ParsePosition(const char* command, struct Position* pos);

// parse UCI "go" command
bool ParseGo(const char* const line, struct SearchInfo* info, struct Position* pos);

// main UCI loop
void UciLoop();

// Net arch: (768xINPUT_BUCKETS -> L1_SIZE)x2 -> 1xOUTPUT_BUCKETS
#define NUM_INPUTS 768
#define INPUT_BUCKETS 1
#define L1_SIZE 48
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
    //int16_t FTWeights[INPUT_BUCKETS * NUM_INPUTS * L1_SIZE];
    //int16_t FTBiases[L1_SIZE];
    //int16_t L1Weights[L1_SIZE * 2 * OUTPUT_BUCKETS];
    //int16_t L1Biases[OUTPUT_BUCKETS];

    int16_t* FTWeights;
    int16_t* FTBiases;
    int16_t* L1Weights;
    int16_t* L1Biases;
};

extern struct Network net;

#if !NOSTDLIB

#include "all.h"

#if defined(USE_SIMD)
#include <immintrin.h>
#endif

#if defined(USE_AVX512)
typedef __m512i vepi16;
typedef __m512i vepi32;

vepi16  vec_zero_epi16();
vepi32  vec_zero_epi32();
vepi16  vec_set1_epi16(const int16_t n);
vepi16  vec_loadu_epi(const vepi16* src);
vepi16  vec_max_epi16(const vepi16 vec0, const vepi16 vec1);
vepi16  vec_min_epi16(const vepi16 vec0, const vepi16 vec1);
vepi16  vec_mullo_epi16(const vepi16 vec0, const vepi16 vec1);
vepi32  vec_madd_epi16(const vepi16 vec0, const vepi16 vec1);
vepi32  vec_add_epi32(const vepi32 vec0, const vepi32 vec1);
int32_t vec_reduce_add_epi32(const vepi32 vec);

#elif defined(USE_AVX2)
typedef __m256i vepi16;
typedef __m256i vepi32;

vepi16  vec_zero_epi16();
vepi32  vec_zero_epi32();
vepi16  vec_set1_epi16(const int16_t n);
vepi16  vec_loadu_epi(const vepi16* src);
vepi16  vec_max_epi16(const vepi16 vec0, const vepi16 vec1);
vepi16  vec_min_epi16(const vepi16 vec0, const vepi16 vec1);
vepi16  vec_mullo_epi16(const vepi16 vec0, const vepi16 vec1);
vepi32  vec_madd_epi16(const vepi16 vec0, const vepi16 vec1);
vepi32  vec_add_epi32(const vepi32 vec0, const vepi32 vec1);
int32_t vec_reduce_add_epi32(const vepi32 vec);
#endif
#endif

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
bool Pov_Accumulator_isClean(const struct Pov_Accumulator* accumulator);

// final total accumulator that holds the 2 povs
struct Accumulator {
    struct Pov_Accumulator perspective[2];
};

void Accumulator_AppendAddIndex(struct Accumulator* accumulator, int piece, int square, const int wkSq, const int bkSq, bool flip[2]);
void Accumulator_AppendSubIndex(struct Accumulator* accumulator, int piece, int square, const int wkSq, const int bkSq, bool flip[2]);
void Accumulator_ClearAddIndex(struct Accumulator* accumulator);
void Accumulator_ClearSubIndex(struct Accumulator* accumulator);

void NNUE_init();
void NNUE_accumulate(struct Accumulator* board_accumulator, struct Position* pos);
void NNUE_update(struct Accumulator* acc, struct Position* pos);
int32_t NNUE_ActivateFTAndAffineL1(const int16_t* us, const int16_t* them, const int16_t* weights, const int16_t bias);
int32_t NNUE_output(struct Accumulator* const board_accumulator, const int stm);