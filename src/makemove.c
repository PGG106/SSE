#include "makemove.h"

#include "attack.h"
#include "init.h"
#include "position.h"
#include "ttable.h"

void HashKey(ZobristKey* originalKey, ZobristKey key) {
    *originalKey ^= key;
}

// Remove a piece from a square, the UPDATE params determines whether we want to update the NNUE weights or not
void ClearPiece(const bool UPDATE, const int piece, const int from, struct Position* pos) {
    assert(piece != EMPTY);
    // Do this first because if we happened to have moved the king we first need to get1lsb the king bitboard before removing it
    if (UPDATE) {
        bool flip[2] = { get_file(KingSQ(pos, WHITE)) > 3, get_file(KingSQ(pos, BLACK)) > 3 };
        int wkSq = KingSQ(pos, WHITE);
        int bkSq = KingSQ(pos, BLACK);
        Accumulator_AppendSubIndex(Position_AccumulatorTop(pos), piece, from, wkSq, bkSq, flip);
    }
    const int color = Color[piece];
    pop_bit(&pos->bitboards[piece], from);
    pop_bit(&pos->occupancies[color], from);
    pos->pieces[from] = EMPTY;
    HashKey(&pos->posKey, PieceKeys[piece][from]);
    if (GetPieceType(piece) == PAWN)
        HashKey(&pos->pawnKey, PieceKeys[piece][from]);
    else if (Color[piece] == WHITE)
        HashKey(&pos->whiteNonPawnKey, PieceKeys[piece][from]);
    else
        HashKey(&pos->blackNonPawnKey, PieceKeys[piece][from]);
}

// Add a piece to a square, the UPDATE params determines whether we want to update the NNUE weights or not
void AddPiece(const bool UPDATE, const int piece, const int to, struct Position* pos) {
    assert(piece != EMPTY);
    const int color = Color[piece];
    set_bit(&pos->bitboards[piece], to);
    set_bit(&pos->occupancies[color], to);
    pos->pieces[to] = piece;
    HashKey(&pos->posKey, PieceKeys[piece][to]);
    if (GetPieceType(piece) == PAWN)
        HashKey(&pos->pawnKey, PieceKeys[piece][to]);
    else if (Color[piece] == WHITE)
        HashKey(&pos->whiteNonPawnKey, PieceKeys[piece][to]);
    else
        HashKey(&pos->blackNonPawnKey, PieceKeys[piece][to]);
    // Do this last because if we happened to have moved the king we first need to re-add to the piece bitboards least we get1lsb an empty bitboard
    if (UPDATE) {
        bool flip[] = { get_file(KingSQ(pos, WHITE)) > 3, get_file(KingSQ(pos, BLACK)) > 3 };
        int wkSq = KingSQ(pos, WHITE);
        int bkSq = KingSQ(pos, BLACK);
        Accumulator_AppendAddIndex(Position_AccumulatorTop(pos), piece, to, wkSq, bkSq, flip);
    }
}

// Move a piece from the [to] square to the [from] square, the UPDATE params determines whether we want to update the NNUE weights or not
void MovePiece(const bool UPDATE, const int piece, const int from, const int to, struct Position* pos) {
    ClearPiece(UPDATE, piece, from, pos);
    AddPiece(UPDATE, piece, to, pos);
}

void UpdateCastlingPerms(struct Position* pos, int source_square, int target_square) {
    // Xor the old castling key from the zobrist key
    HashKey(&pos->posKey, CastleKeys[Position_getCastlingPerm(pos)]);
    // update castling rights
    pos->state.castlePerm &= castling_rights[source_square];
    pos->state.castlePerm &= castling_rights[target_square];
    // Xor the new one
    HashKey(&pos->posKey, CastleKeys[Position_getCastlingPerm(pos)]);
}

inline void resetEpSquare(struct Position* pos) {
    if (Position_getEpSquare(pos) != no_sq) {
        HashKey(&pos->posKey, enpassant_keys[Position_getEpSquare(pos)]);
        pos->state.enPas = no_sq;
    }
}

void MakeCastle(const bool UPDATE, const Move move, struct Position* pos) {
    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);
    // Remove the piece fom the square it moved from
    ClearPiece(UPDATE, piece, sourceSquare, pos);
    // Set the piece to the destination square, if it was a promotion we directly set the promoted piece
    AddPiece(UPDATE, piece, targetSquare, pos);

    resetEpSquare(pos);

    // move the rook
    switch (targetSquare) {
        // white castles king side
    case (g1):
        // move H rook
        MovePiece(UPDATE, WR, h1, f1, pos);
        break;

        // white castles queen side
    case (c1):
        // move A rook
        MovePiece(UPDATE, WR, a1, d1, pos);
        break;

        // black castles king side
    case (g8):
        // move H rook
        MovePiece(UPDATE, BR, h8, f8, pos);
        break;

        // black castles queen side
    case (c8):
        // move A rook
        MovePiece(UPDATE, BR, a8, d8, pos);
        break;
    }
    UpdateCastlingPerms(pos, sourceSquare, targetSquare);
}

void MakeEp(const bool UPDATE, const Move move, struct Position* pos) {
    pos->state.fiftyMove = 0;

    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);
    const int SOUTH = pos->side == WHITE ? 8 : -8;

    const int pieceCap = GetPiece(PAWN, pos->side ^ 1);
    pos->history[pos->historyStackHead].capture = pieceCap;
    const int capturedPieceLocation = targetSquare + SOUTH;
    ClearPiece(UPDATE, pieceCap, capturedPieceLocation, pos);

    // Remove the piece fom the square it moved from
    ClearPiece(UPDATE, piece, sourceSquare, pos);
    // Set the piece to the destination square
    AddPiece(UPDATE, piece, targetSquare, pos);

    // Reset EP square
    assert(Position_getEpSquare(pos) != no_sq);
    HashKey(&pos->posKey, enpassant_keys[Position_getEpSquare(pos)]);
    pos->state.enPas = no_sq;
}

void MakePromo(const bool UPDATE, const Move move, struct Position* pos) {
    pos->state.fiftyMove = 0;

    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);
    const int promotedPiece = GetPiece(getPromotedPiecetype(move), pos->side);

    // Remove the piece fom the square it moved from
    ClearPiece(UPDATE, piece, sourceSquare, pos);
    // Set the piece to the destination square, if it was a promotion we directly set the promoted piece
    AddPiece(UPDATE, promotedPiece, targetSquare, pos);

    resetEpSquare(pos);

    UpdateCastlingPerms(pos, sourceSquare, targetSquare);
}

void MakePromocapture(const bool UPDATE, const Move move, struct Position* pos) {
    pos->state.fiftyMove = 0;

    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);
    const int promotedPiece = GetPiece(getPromotedPiecetype(move), pos->side);

    const int pieceCap = Position_PieceOn(pos, targetSquare);
    assert(pieceCap != EMPTY);
    assert(GetPieceType(pieceCap) != KING);
    ClearPiece(UPDATE, pieceCap, targetSquare, pos);

    pos->history[pos->historyStackHead].capture = pieceCap;

    // Remove the piece fom the square it moved from
    ClearPiece(UPDATE, piece, sourceSquare, pos);
    // Set the piece to the destination square, if it was a promotion we directly set the promoted piece
    AddPiece(UPDATE, promotedPiece, targetSquare, pos);

    resetEpSquare(pos);

    UpdateCastlingPerms(pos, sourceSquare, targetSquare);
}

void MakeQuiet(const bool UPDATE, const Move move, struct Position* pos) {
    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);

    // if a pawn was moved or a capture was played reset the 50 move rule counter
    if (GetPieceType(piece) == PAWN)
        pos->state.fiftyMove = 0;

    MovePiece(UPDATE, piece, sourceSquare, targetSquare, pos);

    resetEpSquare(pos);

    UpdateCastlingPerms(pos, sourceSquare, targetSquare);
}

void MakeCapture(const bool UPDATE, const Move move, struct Position* pos) {
    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);

    pos->state.fiftyMove = 0;

    const int pieceCap = Position_PieceOn(pos, targetSquare);
    assert(pieceCap != EMPTY);
    assert(GetPieceType(pieceCap) != KING);
    ClearPiece(UPDATE, pieceCap, targetSquare, pos);
    pos->history[pos->historyStackHead].capture = pieceCap;

    MovePiece(UPDATE, piece, sourceSquare, targetSquare, pos);

    resetEpSquare(pos);

    UpdateCastlingPerms(pos, sourceSquare, targetSquare);
}

void MakeDP(const bool UPDATE, const Move move, struct Position* pos)
{
    pos->state.fiftyMove = 0;

    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);

    MovePiece(UPDATE, piece, sourceSquare, targetSquare, pos);

    resetEpSquare(pos);

    // Add new ep square
    const int SOUTH = pos->side == WHITE ? 8 : -8;
    int epSquareCandidate = targetSquare + SOUTH;
    if (!(pawn_attacks[pos->side][epSquareCandidate] & Position_GetPieceColorBB(pos, PAWN, pos->side ^ 1)))
        epSquareCandidate = no_sq;
    pos->state.enPas = epSquareCandidate;
    if (Position_getEpSquare(pos) != no_sq)
        HashKey(&pos->posKey, enpassant_keys[Position_getEpSquare(pos)]);
}

bool shouldFlip(int from, int to) {
    const bool prevFlipped = get_file(from) > 3;
    const bool flipped = get_file(to) > 3;

    if (prevFlipped != flipped)
        return true;

    return false;
}

// make move on chess board
void MakeMove(const bool UPDATE, const Move move, struct Position* pos) {
    if (UPDATE) {
        saveBoardState(pos);
        pos->accumStackHead++;
    }
    // Store position key in the array of searched position
    pos->played_positions[pos->played_positions_size++] = pos->posKey;

    // parse move flag
    const bool capture = isCapture(move);
    const bool doublePush = isDP(move);
    const bool enpass = isEnpassant(move);
    const bool castling = isCastle(move);
    const bool promotion = isPromo(move);
    // increment fifty move rule counter
    pos->state.fiftyMove++;
    pos->state.plyFromNull++;
    pos->hisPly++;

    if (castling) {
        MakeCastle(UPDATE, move, pos);
    }
    else if (doublePush) {
        MakeDP(UPDATE, move, pos);
    }
    else if (enpass) {
        MakeEp(UPDATE, move, pos);
    }
    else if (promotion && capture) {
        MakePromocapture(UPDATE, move, pos);
    }
    else if (promotion) {
        MakePromo(UPDATE, move, pos);
    }
    else if (!capture) {
        MakeQuiet(UPDATE, move, pos);
    }
    else {
        MakeCapture(UPDATE, move, pos);
    }

    if (UPDATE)
        pos->historyStackHead++;

    // change side
    Position_ChangeSide(pos);
    // Xor the new side into the key
    HashKey(&pos->posKey, SideKey);
    // Update pinmasks and checkers
    UpdatePinsAndCheckers(pos, pos->side);
    // If we are in check get the squares between the checking piece and the king
    if (Position_getCheckers(pos)) {
        const int kingSquare = KingSQ(pos, pos->side);
        const int pieceLocation = GetLsbIndex(Position_getCheckers(pos));
        pos->state.checkMask = (1ULL << pieceLocation) | RayBetween(pieceLocation, kingSquare);
    }
    else
        pos->state.checkMask = fullCheckmask;

    // Figure out if we need to refresh the accumulator
    if (UPDATE) {
        if (PieceType[Piece(move)] == KING) {
            int kingColor = Color[Piece(move)];
            if (shouldFlip(From(move), To(move))) {
                // tell the right accumulator it'll need a refresh
                pos->accumStack[pos->accumStackHead - 1].perspective[kingColor].needsRefresh = true;
            }
        }
    }

    // Make sure a freshly generated zobrist key matches the one we are incrementally updating
    assert(pos->posKey == GeneratePosKey(pos));
    assert(pos->pawnKey == GeneratePawnKey(pos));
}

void UnmakeMove(const Move move, struct Position* pos) {
    // quiet moves
    pos->hisPly--;
    pos->historyStackHead--;

    restorePreviousBoardState(pos);

    Accumulator_ClearAddIndex(Position_AccumulatorTop(pos));
    Accumulator_ClearSubIndex(Position_AccumulatorTop(pos));
    Position_AccumulatorTop(pos)->perspective[WHITE].needsRefresh = false;
    Position_AccumulatorTop(pos)->perspective[BLACK].needsRefresh = false;

    pos->accumStackHead--;

    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    // parse move flag
    const bool capture = isCapture(move);
    const bool enpass = isEnpassant(move);
    const bool castling = isCastle(move);
    const bool promotion = isPromo(move);

    const int piece = promotion ? GetPiece(getPromotedPiecetype(move), pos->side ^ 1) : Piece(move);

    // clear the piece on the target square
    ClearPiece(false, piece, targetSquare, pos);
    // no matter what add back the piece that was originally moved, ignoring any promotion
    AddPiece(false, Piece(move), sourceSquare, pos);

    // handle captures
    if (capture) {
        const int SOUTH = pos->side == WHITE ? -8 : 8;
        // Retrieve the captured piece we have to restore
        const int piececap = Position_getCapturedPiece(pos);
        const int capturedPieceLocation = enpass ? targetSquare + SOUTH : targetSquare;
        AddPiece(false, piececap, capturedPieceLocation, pos);
    }

    // handle castling moves
    if (castling) {
        // switch target square
        switch (targetSquare) {
            // white castles king side
        case (g1):
            // move H rook
            MovePiece(false, WR, f1, h1, pos);
            break;

            // white castles queen side
        case (c1):
            // move A rook
            MovePiece(false, WR, d1, a1, pos);
            break;

            // black castles king side
        case (g8):
            // move H rook
            MovePiece(false, BR, f8, h8, pos);
            break;

            // black castles queen side
        case (c8):
            // move A rook
            MovePiece(false, BR, d8, a8, pos);
            break;
        }
    }

    // change side
    Position_ChangeSide(pos);

    // restore zobrist key (done at the end to avoid overwriting the value while moving pieces back to their place)
    // we don't need to do the same for the pawn key because the unmake function correctly restores it already
    pos->posKey = pos->played_positions[--pos->played_positions_size];
}

// MakeNullMove handles the playing of a null move (a move that doesn't move any piece)
void MakeNullMove(struct Position* pos) {
    saveBoardState(pos);
    // Store position key in the array of searched position
    pos->played_positions[pos->played_positions_size++] = pos->posKey;
    // Update the zobrist key asap so we can prefetch
    resetEpSquare(pos);
    Position_ChangeSide(pos);
    HashKey(&pos->posKey, SideKey);
    TTPrefetch(Position_getPoskey(pos));

    pos->hisPly++;
    pos->historyStackHead++;
    pos->state.fiftyMove++;
    pos->state.plyFromNull = 0;

    // Update pinmasks and checkers
    UpdatePinsAndCheckers(pos, pos->side);
}

// Take back a null move
void TakeNullMove(struct Position* pos) {
    pos->hisPly--;
    pos->historyStackHead--;

    restorePreviousBoardState(pos);

    Position_ChangeSide(pos);
    pos->posKey = pos->played_positions[--pos->played_positions_size];
}

