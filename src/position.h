#pragma once

#include "bitboard.h"
#include "nnue.h"
#include "types.h"
#include "uci.h"

#include "shims.h"

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
Bitboard Position_GetPieceColorBB(struct Position const* const position, const int piecetype, const int color);
struct Accumulator* Position_AccumulatorTop(struct Position* position);
Bitboard Position_Occupancy(const struct Position* const pos, const int occupancySide);
int Position_PieceCount(const struct Position* const pos);
int Position_PieceOn(const struct Position* const pos, const int square);
ZobristKey Position_getPoskey(const struct Position* const pos);
int Position_get50MrCounter(const struct Position* const pos);
int Position_getCastlingPerm(const struct Position* const pos);
int Position_getEpSquare(const struct Position* const pos);
int Position_getPlyFromNull(const struct Position* const pos);
Bitboard Position_getCheckers(const struct Position* const pos);
Bitboard Position_getCheckmask(const struct Position* const pos);
Bitboard Position_getPinnedMask(const struct Position* const pos);
int Position_getCapturedPiece(const struct Position* const pos);
void Position_ChangeSide(struct Position* const pos);

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
