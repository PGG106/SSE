#include "all.h"

Move encode_move(const int source, const int target, const int piece, const int movetype) {
    return (source) | (target << 6) | (movetype << 12) | (piece << 16);
}

int From(const Move move) { return move & 0x3F; }
int To(const Move move) { return ((move & 0xFC0) >> 6); }
int FromTo(const Move move) { return move & 0xFFF; }
int Piece(const Move move) { return ((move & 0xF0000) >> 16); }
int PieceTo(const Move move) { return (Piece(move) << 6) | To(move); }
int PieceTypeTo(const Move move) { return (PieceType[Piece(move)] << 6) | To(move); }
int GetMovetype(const Move move) { return ((move & 0xF000) >> 12); }
int getPromotedPiecetype(const Move move) { return (GetMovetype(move) & 3) + 1; }
bool isEnpassant(const Move move) { return GetMovetype(move) == enPassant; }
bool isDP(const Move move) { return GetMovetype(move) == doublePush; }
bool isCastle(const Move move) {
    return (GetMovetype(move) == KSCastle) || (GetMovetype(move) == QSCastle);
}
bool isCapture(const Move move) { return GetMovetype(move) & Capture; }
bool isQuiet(const Move move) { return !isCapture(move); }
bool isPromo(const Move move) { return GetMovetype(move) & 8; }
// Shorthand for captures + any promotion no matter if quiet or not 
bool isTactical(const Move move) { return isCapture(move) || isPromo(move); }
