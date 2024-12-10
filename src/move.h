#pragma once

#include "types.h"
#include "shims.h"

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
    knightPromo=8, bishopPromo, rookPromo, queenPromo,
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
