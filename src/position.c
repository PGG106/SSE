#include "position.h"

#include "attack.h"
#include "hyperbola.h"
#include "init.h"
#include "io.h"
#include "piece_data.h"
#include "makemove.h"

#include "shims.h"

const int castling_rights[64] = {
     7, 15, 15, 15,  3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14,
};

SMALL void square_to_coordinates(char *buf, const int sq) {
    buf[0] = 'a' + sq % 8;
    buf[1] = '8' - sq / 8;
}

// Reset the position to a clean state
SMALL void ResetBoard(struct Position* const pos) {
    // reset board position (pos->pos->bitboards)
    memset(pos->bitboards, 0, sizeof(pos->bitboards));

    // reset pos->occupancies (pos->pos->bitboards)
    memset(pos->occupancies, 0, sizeof(pos->occupancies));

    memset(&pos->pieces, 0, sizeof(Bitboard) * 12);
    for (int index = 0; index < 64; ++index) {
        pos->pieces[index] = EMPTY;
    }
    pos->state.castlePerm = 0;
    pos->state.plyFromNull = 0;
}

SMALL void ResetInfo(struct SearchInfo* const info) {
    info->depth = 0;
    info->nodes = 0;
    info->starttime = 0;
    info->stoptimeOpt = 0;
    info->stoptimeMax = 0;
    info->stopped = false;
    info->timeset = false;
}

// Generates zobrist key from scratch
SMALL ZobristKey GeneratePosKey(const struct Position* const pos) {
    Bitboard finalkey = 0;
    // for every square
    for (int sq = 0; sq < 64; ++sq) {
        // get piece on that square
        const int piece = Position_PieceOn(pos, sq);
        // if it's not empty add that piece to the zobrist key
        if (piece != EMPTY) {
            assert(piece >= WP && piece <= BK);
            finalkey ^= PieceKeys[piece][sq];
        }
    }
    // include side in the key
    if (pos->side == BLACK) {
        finalkey ^= SideKey;
    }
    // include the ep square in the key
    if (Position_getEpSquare(pos) != no_sq) {
        assert(Position_getEpSquare(pos) >= 0 && Position_getEpSquare(pos) < 64);
        finalkey ^= enpassant_keys[Position_getEpSquare(pos)];
    }
    assert(Position_getCastlingPerm(pos) >= 0 && Position_getCastlingPerm(pos) <= 15);
    // add to the key the status of the castling permissions
    finalkey ^= CastleKeys[Position_getCastlingPerm(pos)];
    return finalkey;
}

// Generates zobrist key (for only the pawns) from scratch
SMALL ZobristKey GeneratePawnKey(const struct Position* pos) {
    Bitboard pawnKey = 0;
    for (int sq = 0; sq < 64; ++sq) {
        // get piece on that square
        const int piece = Position_PieceOn(pos, sq);
        if (piece == WP || piece == BP) {
            pawnKey ^= PieceKeys[piece][sq];
        }
    }
    return pawnKey;
}

// Generates zobrist key (for non-pawns) from scratch
SMALL ZobristKey GenerateNonPawnKey(const struct Position* pos, int side) {
    Bitboard nonPawnKey = 0;
    for (int sq = 0; sq < 64; ++sq) {
        // get piece on that square
        const int piece = Position_PieceOn(pos, sq);
        if (piece != EMPTY && piece != WP && piece != BP && Color[piece] == side) {
            nonPawnKey ^= PieceKeys[piece][sq];
        }
    }
    return nonPawnKey;
}

Bitboard RayBetween(int square1, int square2) {
    return SQUARES_BETWEEN_BB[square1][square2];
}

// Get on what square of the board the king of color c resides
int KingSQ(const struct Position* pos, const int c) {
    return GetLsbIndex(Position_GetPieceColorBB(pos, KING, c));
}

void UpdatePinsAndCheckers(struct Position* pos, const int side) {
    const Bitboard them = Position_Occupancy(pos, side ^ 1);
    const int kingSquare = KingSQ(pos, side);
    const Bitboard pawnCheckers = Position_GetPieceColorBB(pos, PAWN, side ^ 1) & pawn_attacks[side][kingSquare];
    const Bitboard knightCheckers = Position_GetPieceColorBB(pos, KNIGHT, side ^ 1) & knight_attacks[kingSquare];
    const Bitboard bishopsQueens = Position_GetPieceColorBB(pos, BISHOP, side ^ 1) | Position_GetPieceColorBB(pos, QUEEN, side ^ 1);
    const Bitboard rooksQueens = Position_GetPieceColorBB(pos, ROOK, side ^ 1) | Position_GetPieceColorBB(pos, QUEEN, side ^ 1);
    Bitboard sliderAttacks = (bishopsQueens & GetBishopAttacks(kingSquare, them)) | (rooksQueens & GetRookAttacks(kingSquare, them));
    pos->state.pinned = 0ULL;
    pos->state.checkers = pawnCheckers | knightCheckers;

    while (sliderAttacks) {
        const int sq = popLsb(&sliderAttacks);
        const Bitboard blockers = RayBetween(kingSquare, sq) & Position_Occupancy(pos, side);
        const int numBlockers = CountBits(blockers);
        if (!numBlockers)
            pos->state.checkers |= 1ULL << sq;
        else if (numBlockers == 1)
            pos->state.pinned |= blockers;
    }
}

// Returns the bitboard of a piecetype
Bitboard GetPieceBB(const struct Position* pos, const int piecetype) {
    return Position_GetPieceColorBB(pos, piecetype, WHITE) | Position_GetPieceColorBB(pos, piecetype, BLACK);
}

// Return a piece based on the piecetype and the color
int GetPiece(const int piecetype, const int color) {
    return piecetype + 6 * color;
}

// Returns the piecetype of a piece
int GetPieceType(const int piece) {
    if (piece == EMPTY)
        return EMPTY;
    return PieceType[piece];
}

// Returns true if side has at least one piece on the board that isn't a pawn or the king, false otherwise
bool BoardHasNonPawns(const struct Position* pos, const int side) {
    return Position_Occupancy(pos, side) ^ Position_GetPieceColorBB(pos, PAWN, side) ^ Position_GetPieceColorBB(pos, KING, side);
}

// Calculates what the key for position pos will be after move <move>, it's a rough estimate and will fail for "special" moves such as promotions and castling
ZobristKey keyAfter(const struct Position* pos, const Move move) {

    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);
    const int  captured = Position_PieceOn(pos, targetSquare);

    ZobristKey newKey = Position_getPoskey(pos) ^ SideKey ^ PieceKeys[piece][sourceSquare] ^ PieceKeys[piece][targetSquare];

    if (captured != EMPTY)
        newKey ^= PieceKeys[captured][targetSquare];

    return newKey;
}

// parse FEN string
SMALL void ParseFen(const char* command, struct Position* pos) {

    ResetBoard(pos);

    int token_index = 0;

    char pos_string[128];
    next_token(command, &token_index, pos_string);

    char turn[2];
    next_token(command, &token_index, turn);

    char castle_perm[5];
    next_token(command, &token_index, castle_perm);

    char ep_square[3];
    next_token(command, &token_index, ep_square);

    char fifty_move[8] = { 0 };
    char HisPly[8] = { 0 };

    // Keep fifty move and Hisply arguments optional
    if (next_token(command, &token_index, fifty_move)) {
        next_token(command, &token_index, HisPly);
    }

    int fen_counter = 0;
    for (int rank = 0; rank < 8; rank++) {
        // loop over board files
        for (int file = 0; file < 8; file++) {
            // init current square
            const int square = rank * 8 + file;
            const char current_char = pos_string[fen_counter];

            // match ascii pieces within FEN string
            if ((current_char >= 'a' && current_char <= 'z') || (current_char >= 'A' && current_char <= 'Z')) {
                // init piece type
                const int piece = char_pieces(current_char);
                if (piece != EMPTY) {
                    // set piece on corresponding bitboard
                    set_bit(&pos->bitboards[piece], square);
                    pos->pieces[square] = piece;
                }
                fen_counter++;
            }

            // match empty square numbers within FEN string
            if (current_char >= '0' && current_char <= '9') {
                // init offset (convert char 0 to int 0)
                const int offset = current_char - '0';

                const int piece = Position_PieceOn(pos, square);
                // on empty current square
                if (piece == EMPTY)
                    // decrement file
                    file--;

                // adjust file counter
                file += offset;

                // increment pointer to FEN string
                fen_counter++;
            }

            // match rank separator
            if (pos_string[fen_counter] == '/')
                // increment pointer to FEN string
                fen_counter++;
        }
    }

    // parse player turn
    pos->side = turn[0] == 'w' ? WHITE : BLACK;

    // Parse castling rights
    for (int i = 0;; i++) {
        const char c = castle_perm[i];
        if (c == '\0') {
            break;
        }

        switch (c) {
        case 'K':
            (pos->state.castlePerm) |= WKCA;
            break;
        case 'Q':
            (pos->state.castlePerm) |= WQCA;
            break;
        case 'k':
            (pos->state.castlePerm) |= BKCA;
            break;
        case 'q':
            (pos->state.castlePerm) |= BQCA;
            break;
        case '-':
            break;
        }
    }

    // parse enpassant square
    if (ep_square[0] != '\0' && ep_square[1] != '\0' && ep_square[0] != '-') {
        // parse enpassant file & rank
        const int file = ep_square[0] - 'a';
        const int rank = 8 - (ep_square[1] - '0');

        // init enpassant square
        pos->state.enPas = rank * 8 + file;
    }
    // no enpassant square
    else
        pos->state.enPas = no_sq;

    // Read fifty moves counter
    if (fifty_move[0] != '\0') {
        pos->state.fiftyMove = atoi(fifty_move);
    }
    else {
        pos->state.fiftyMove = 0;
    }
    // Read Hisply moves counter
    if (HisPly[0] != '\0') {
        pos->hisPly = atoi(HisPly);
    }
    else {
        pos->hisPly = 0;
    }

    for (int piece = WP; piece <= WK; piece++)
        // populate white occupancy bitboard
        pos->occupancies[WHITE] |= pos->bitboards[piece];

    for (int piece = BP; piece <= BK; piece++)
        // populate white occupancy bitboard
        pos->occupancies[BLACK] |= pos->bitboards[piece];

    pos->posKey = GeneratePosKey(pos);
    pos->pawnKey = GeneratePawnKey(pos);
    pos->whiteNonPawnKey = GenerateNonPawnKey(pos, WHITE);
    pos->blackNonPawnKey = GenerateNonPawnKey(pos, BLACK);

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

    // Update nnue accumulator to reflect board state
    NNUE_accumulate(&pos->accumStack[0], pos);
    pos->accumStackHead = 1;
}

void saveBoardState(struct Position* pos) {
    pos->history[pos->historyStackHead] = pos->state;
}

void restorePreviousBoardState(struct Position* pos)
{
    pos->state = pos->history[pos->historyStackHead];
}

SMALL void parse_moves(const char* moves, struct Position* pos) {
#if UCI
    char move_str[6];
    int token_index = 0;

    // loop over moves within a move string
    while (next_token(moves, &token_index, move_str)) {
        // parse next move
        const Move move = ParseMove(move_str, pos);
        // make move on the chess board
        MakeMove(false, move, pos);
    }
#endif
}
