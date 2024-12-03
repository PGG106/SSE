#pragma once

#include "shims.h"

#define NAME "Alexandria-7.1.1"

extern int see_margin[64][2];

#ifdef __cplusplus
static const std::string start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
#else
static const char* start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
#endif

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
