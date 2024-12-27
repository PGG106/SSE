#include "movegen.h"

#include "attack.h"
#include "magic.h"
#include "makemove.h"
#include "move.h"
#include "position.h"

#include "shims.h"

#define abs(x) ((x) < 0 ? -(x) : (x))

// function that adds a (not yet scored) move to a move list
void AddMoveNonScored(const Move move, struct MoveList* list) {
    list->moves[list->count].move = move;
    list->count++;
}

// function that adds an (already-scored) move to a move list
void AddMoveScored(const Move move, const int score, struct MoveList* list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = score;
    list->count++;
}

static Bitboard NORTH(const Bitboard in, const int color) {
    return color == WHITE ? in >> 8 : in << 8;
}

void PseudoLegalPawnMoves(struct Position* pos, int color, struct MoveList* list, const enum MovegenType type) {
    const Bitboard enemy = Position_Occupancy(pos, color ^ 1);
    const Bitboard ourPawns = Position_GetPieceColorBB(pos, PAWN, color);
    const Bitboard rank4BB = color == WHITE ? 0x000000FF00000000ULL : 0x00000000FF000000ULL;
    const Bitboard freeSquares = ~Position_Occupancy(pos, BOTH);
    const int pawnType = GetPiece(PAWN, pos->side);
    const int north = color == WHITE ? -8 : 8;
    const bool genNoisy = type & MOVEGEN_NOISY;
    const bool genQuiet = type & MOVEGEN_QUIET;

    // Quiet moves (ie push/double-push)
    if (genQuiet) {
        Bitboard push = NORTH(ourPawns, color) & freeSquares & ~0xFF000000000000FFULL;
        Bitboard doublePushBb = NORTH(push, color) & freeSquares & rank4BB;
        push &= Position_getCheckmask(pos);
        doublePushBb &= Position_getCheckmask(pos);
        while (push) {
            const int to = popLsb(&push);
            AddMoveNonScored(encode_move(to - north, to, pawnType, Quiet), list);
        }
        while (doublePushBb) {
            const int to = popLsb(&doublePushBb);
            AddMoveNonScored(encode_move(to - north * 2, to, pawnType, doublePush), list);
        }
    }

    if (genNoisy) {
        // Push promotions
        Bitboard pushPromo = NORTH(ourPawns, color) & freeSquares & 0xFF000000000000FFULL & Position_getCheckmask(pos);
        while (pushPromo) {
            const int to = popLsb(&pushPromo);
            AddMoveNonScored(encode_move(to - north, to, pawnType, queenPromo | Quiet), list);
            AddMoveNonScored(encode_move(to - north, to, pawnType, rookPromo | Quiet), list);
            AddMoveNonScored(encode_move(to - north, to, pawnType, bishopPromo | Quiet), list);
            AddMoveNonScored(encode_move(to - north, to, pawnType, knightPromo | Quiet), list);
        }

        // Captures and capture-promotions
        Bitboard captBB1 = (NORTH(ourPawns, color) >> 1) & ~0x8080808080808080ULL & enemy & Position_getCheckmask(pos);
        Bitboard captBB2 = (NORTH(ourPawns, color) << 1) & ~0x101010101010101ULL & enemy & Position_getCheckmask(pos);
        while (captBB1) {
            const int to = popLsb(&captBB1);
            const int from = to - north + 1;
            if (get_rank[to] != (color == WHITE ? 7 : 0))
                AddMoveNonScored(encode_move(from, to, pawnType, Capture), list);
            else {
                AddMoveNonScored(encode_move(from, to, pawnType, queenPromo | Capture), list);
                AddMoveNonScored(encode_move(from, to, pawnType, rookPromo | Capture), list);
                AddMoveNonScored(encode_move(from, to, pawnType, bishopPromo | Capture), list);
                AddMoveNonScored(encode_move(from, to, pawnType, knightPromo | Capture), list);
            }
        }

        while (captBB2) {
            const int to = popLsb(&captBB2);
            const int from = to - north - 1;
            if (get_rank[to] != (color == WHITE ? 7 : 0))
                AddMoveNonScored(encode_move(from, to, pawnType, Capture), list);
            else {
                AddMoveNonScored(encode_move(from, to, pawnType, queenPromo | Capture), list);
                AddMoveNonScored(encode_move(from, to, pawnType, rookPromo | Capture), list);
                AddMoveNonScored(encode_move(from, to, pawnType, bishopPromo | Capture), list);
                AddMoveNonScored(encode_move(from, to, pawnType, knightPromo | Capture), list);
            }
        }

        const int epSq = Position_getEpSquare(pos);
        if (epSq == no_sq)
            return;

        // En passant
        Bitboard epPieces = pawn_attacks[color ^ 1][epSq] & ourPawns;
        while (epPieces) {
            int from = popLsb(&epPieces);
            AddMoveNonScored(encode_move(from, epSq, pawnType, enPassant), list);
        }
    }
}

void PseudoLegalKnightMoves(struct Position* pos, int color, struct MoveList* list, const enum MovegenType type) {
    Bitboard knights = Position_GetPieceColorBB(pos, KNIGHT, color);
    const int knightType = GetPiece(KNIGHT, color);
    const bool genNoisy = type & MOVEGEN_NOISY;
    const bool genQuiet = type & MOVEGEN_QUIET;
    Bitboard moveMask = 0ULL; // We restrict the number of squares the knight can travel to

    // The type requested includes noisy moves
    if (genNoisy)
        moveMask |= Position_Occupancy(pos, color ^ 1);

    // The type requested includes quiet moves
    if (genQuiet)
        moveMask |= ~Position_Occupancy(pos, BOTH);

    while (knights) {
        const int from = popLsb(&knights);
        Bitboard possible_moves = knight_attacks[from] & moveMask & Position_getCheckmask(pos);
        while (possible_moves) {
            const int to = popLsb(&possible_moves);
            const enum Movetype movetype = Position_PieceOn(pos, to) != EMPTY ? Capture : Quiet;
            AddMoveNonScored(encode_move(from, to, knightType, movetype), list);
        }
    }
}

void PseudoLegalSlidersMoves(struct Position* pos, int color, struct MoveList* list, const enum MovegenType type) {
    const bool genNoisy = type & MOVEGEN_NOISY;
    const bool genQuiet = type & MOVEGEN_QUIET;
    Bitboard boardOccupancy = Position_Occupancy(pos, BOTH);
    Bitboard moveMask = 0ULL; // We restrict the number of squares the bishop can travel to

    // The type requested includes noisy moves
    if (genNoisy)
        moveMask |= Position_Occupancy(pos, color ^ 1);

    // The type requested includes quiet moves
    if (genQuiet)
        moveMask |= ~Position_Occupancy(pos, BOTH);

    for (int piecetype = BISHOP; piecetype <= QUEEN; piecetype++) {
        Bitboard pieces = Position_GetPieceColorBB(pos, piecetype, color);
        const int coloredPieceValue = GetPiece(piecetype, color);
        while (pieces) {
            const int from = popLsb(&pieces);
            Bitboard possible_moves =
                pieceAttacks(piecetype, from, boardOccupancy) & moveMask & Position_getCheckmask(pos);
            while (possible_moves) {
                const int to = popLsb(&possible_moves);
                const enum Movetype movetype = Position_PieceOn(pos, to) != EMPTY ? Capture : Quiet;
                AddMoveNonScored(encode_move(from, to, coloredPieceValue, movetype), list);
            }
        }
    }
}

void PseudoLegalKingMoves(struct Position* pos, int color, struct MoveList* list, const enum MovegenType type) {
    const int kingType = GetPiece(KING, color);
    const int from = KingSQ(pos, color);
    const bool genNoisy = type & MOVEGEN_NOISY;
    const bool genQuiet = type & MOVEGEN_QUIET;
    Bitboard moveMask = 0ULL; // We restrict the number of squares the king can travel to

    // The type requested includes noisy moves
    if (genNoisy)
        moveMask |= Position_Occupancy(pos, color ^ 1);

    // The type requested includes quiet moves
    if (genQuiet)
        moveMask |= ~Position_Occupancy(pos, BOTH);

    Bitboard possible_moves = king_attacks[from] & moveMask;
    while (possible_moves) {
        const int to = popLsb(&possible_moves);
        enum Movetype movetype = Position_PieceOn(pos, to) != EMPTY ? Capture : Quiet;
        AddMoveNonScored(encode_move(from, to, kingType, movetype), list);
    }

    // Only generate castling moves if we are generating quiets
    // Castling is illegal in check
    if (genQuiet && !Position_getCheckers(pos)) {
        const Bitboard occ = Position_Occupancy(pos, BOTH);
        const int castlePerms = Position_getCastlingPerm(pos);
        if (color == WHITE) {
            // king side castling is available
            if ((castlePerms & WKCA) && !(occ & 0x6000000000000000ULL))
                AddMoveNonScored(encode_move(e1, g1, WK, KSCastle), list);

            // queen side castling is available
            if ((castlePerms & WQCA) && !(occ & 0x0E00000000000000ULL))
                AddMoveNonScored(encode_move(e1, c1, WK, QSCastle), list);
        }
        else {
            // king side castling is available
            if ((castlePerms & BKCA) && !(occ & 0x0000000000000060ULL))
                AddMoveNonScored(encode_move(e8, g8, BK, KSCastle), list);

            // queen side castling is available
            if ((castlePerms & BQCA) && !(occ & 0x000000000000000EULL))
                AddMoveNonScored(encode_move(e8, c8, BK, QSCastle), list);
        }
    }
}

// generate moves
void GenerateMoves(struct MoveList* move_list, struct Position* pos, enum MovegenType type) {

    assert(type == MOVEGEN_ALL || type == MOVEGEN_NOISY || type == MOVEGEN_QUIET);

    const int checks = CountBits(Position_getCheckers(pos));
    if (checks < 2) {
        PseudoLegalPawnMoves(pos, pos->side, move_list, type);
        PseudoLegalKnightMoves(pos, pos->side, move_list, type);
        PseudoLegalSlidersMoves(pos, pos->side, move_list, type);
    }
    PseudoLegalKingMoves(pos, pos->side, move_list, type);
}

// Pseudo-legality test inspired by Koivisto
bool IsPseudoLegal(const struct Position* pos, const Move move) {

    if (move == NOMOVE)
        return false;

    const int from = From(move);
    const int to = To(move);
    const int movedPiece = Piece(move);
    const int pieceType = GetPieceType(movedPiece);

    if (from == to)
        return false;

    if (movedPiece == EMPTY)
        return false;

    if (Position_PieceOn(pos, from) != movedPiece)
        return false;

    if (Color[movedPiece] != pos->side)
        return false;

    if ((1ULL << to) & Position_Occupancy(pos, pos->side))
        return false;

    if ((!isCapture(move) || isEnpassant(move)) && Position_PieceOn(pos, to) != EMPTY)
        return false;

    if (isCapture(move) && !isEnpassant(move) && Position_PieceOn(pos, to) == EMPTY)
        return false;

    if ((isDP(move)
        || isPromo(move)
        || isEnpassant(move)) && pieceType != PAWN)
        return false;

    if (isCastle(move) && pieceType != KING)
        return false;

    if ((CountBits(Position_getCheckers(pos)) >= 2) && pieceType != KING)
        return false;

    const int NORTH = pos->side == WHITE ? -8 : 8;

    switch (pieceType) {
    case PAWN:
        if (isDP(move)) {
            if (from + NORTH + NORTH != to)
                return false;

            if (Position_PieceOn(pos, from + NORTH) != EMPTY)
                return false;

            if ((pos->side == WHITE && get_rank[from] != 1)
                || (pos->side == BLACK && get_rank[from] != 6))
                return false;
        }
        else if (!isCapture(move)) {
            if (from + NORTH != to)
                return false;
        }
        if (isEnpassant(move)) {
            if (to != Position_getEpSquare(pos))
                return false;

            if (!((1ULL << (to - NORTH)) & Position_GetPieceColorBB(pos, PAWN, pos->side ^ 1)))
                return false;
        }
        if (isCapture(move) && !(pawn_attacks[pos->side][from] & (1ULL << to)))
            return false;

        if (isPromo(move)) {
            if ((pos->side == WHITE && get_rank[from] != 6)
                || (pos->side == BLACK && get_rank[from] != 1))
                return false;

            if ((pos->side == WHITE && get_rank[to] != 7)
                || (pos->side == BLACK && get_rank[to] != 0))
                return false;
        }
        else {
            if ((pos->side == WHITE && get_rank[from] >= 6)
                || (pos->side == BLACK && get_rank[from] <= 1))
                return false;
        }
        break;

    case KNIGHT:
        if (!(knight_attacks[from] & (1ULL << to)))
            return false;

        break;

    case BISHOP:
        if (get_diagonal[from] != get_diagonal[to]
            && get_antidiagonal(from) != get_antidiagonal(to))
            return false;

        if (RayBetween(from, to) & Position_Occupancy(pos, BOTH))
            return false;

        break;

    case ROOK:
        if (get_file(from) != get_file(to)
            && get_rank[from] != get_rank[to])
            return false;

        if (RayBetween(from, to) & Position_Occupancy(pos, BOTH))
            return false;

        break;

    case QUEEN:
        if (get_file(from) != get_file(to)
            && get_rank[from] != get_rank[to]
            && get_diagonal[from] != get_diagonal[to]
            && get_antidiagonal(from) != get_antidiagonal(to))
            return false;

        if (RayBetween(from, to) & Position_Occupancy(pos, BOTH))
            return false;

        break;

    case KING:
        if (isCastle(move)) {
            if (Position_getCheckers(pos))
                return false;

            if (abs(to - from) != 2)
                return false;

            bool isKSCastle = GetMovetype(move) == KSCastle;

            Bitboard castleBlocked = Position_Occupancy(pos, BOTH) & (pos->side == WHITE ? isKSCastle ? 0x6000000000000000ULL
                : 0x0E00000000000000ULL
                : isKSCastle ? 0x0000000000000060ULL
                : 0x000000000000000EULL);
            int castleType = pos->side == WHITE ? isKSCastle ? WKCA
                : WQCA
                : isKSCastle ? BKCA
                : BQCA;

            if (!castleBlocked && (Position_getCastlingPerm(pos) & castleType))
                return true;

            return false;
        }
        if (!(king_attacks[from] & (1ULL << to)))
            return false;

        break;
    }
    return true;
}

bool IsLegal(struct Position* pos, const Move move) {

    const int color = pos->side;
    const int ksq = KingSQ(pos, color);
    const int from = From(move);
    const int to = To(move);
    const int movedPiece = Piece(move);
    const int pieceType = GetPieceType(movedPiece);

    if (isEnpassant(move)) {
        int offset = color == WHITE ? 8 : -8;
        int ourPawn = GetPiece(PAWN, color);
        int theirPawn = GetPiece(PAWN, color ^ 1);
        ClearPiece(false, ourPawn, from, pos);
        ClearPiece(false, theirPawn, to + offset, pos);
        AddPiece(false, ourPawn, to, pos);
        bool isLegal = !IsSquareAttacked(pos, ksq, color ^ 1);
        AddPiece(false, ourPawn, from, pos);
        AddPiece(false, theirPawn, to + offset, pos);
        ClearPiece(false, ourPawn, to, pos);
        return isLegal;
    }
    else if (isCastle(move)) {
        bool isKSCastle = GetMovetype(move) == KSCastle;
        if (isKSCastle) {
            return    !IsSquareAttacked(pos, color == WHITE ? f1 : f8, color ^ 1)
                && !IsSquareAttacked(pos, color == WHITE ? g1 : g8, color ^ 1);
        }
        else {
            return    !IsSquareAttacked(pos, color == WHITE ? d1 : d8, color ^ 1)
                && !IsSquareAttacked(pos, color == WHITE ? c1 : c8, color ^ 1);
        }
    }

    if (pieceType == KING) {
        int king = GetPiece(KING, color);
        ClearPiece(false, king, ksq, pos);
        bool isLegal = !IsSquareAttacked(pos, to, color ^ 1);
        AddPiece(false, king, ksq, pos);
        return isLegal;
    }
    else if (Position_getPinnedMask(pos) & (1ULL << from)) {
        return !Position_getCheckers(pos) && (((1ULL << to) & RayBetween(ksq, from)) || ((1ULL << from) & RayBetween(ksq, to)));
    }
    else if (Position_getCheckers(pos)) {
        return (1ULL << to) & Position_getCheckmask(pos);
    }
    else
        return true;
}

// is the square given in input attacked by the current given side
bool IsSquareAttacked(const struct Position* pos, const int square, const int side) {
    // Take the occupancies of both positions, encoding where all the pieces on the board reside
    const Bitboard occ = Position_Occupancy(pos, BOTH);
    // is the square attacked by pawns
    if (pawn_attacks[side ^ 1][square] & Position_GetPieceColorBB(pos, PAWN, side))
        return true;
    // is the square attacked by knights
    if (knight_attacks[square] & Position_GetPieceColorBB(pos, KNIGHT, side))
        return true;
    // is the square attacked by kings
    if (king_attacks[square] & Position_GetPieceColorBB(pos, KING, side))
        return true;
    // is the square attacked by bishops
    if (GetBishopAttacks(square, occ) & (Position_GetPieceColorBB(pos, BISHOP, side) | Position_GetPieceColorBB(pos, QUEEN, side)))
        return true;
    // is the square attacked by rooks
    if (GetRookAttacks(square, occ) & (Position_GetPieceColorBB(pos, ROOK, side) | Position_GetPieceColorBB(pos, QUEEN, side)))
        return true;
    // by default return false
    return false;
}
