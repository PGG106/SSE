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
