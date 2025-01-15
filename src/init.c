#include "init.h"

#include "position.h"
#include "attack.h"
#include "magic.h"
#include "hyperbola.h"
#include "search.h"
#include "ttable.h"
#include "history.h"
#include "uci.h"

Bitboard PieceKeys[12][64];
Bitboard enpassant_keys[64];
Bitboard SideKey;
Bitboard CastleKeys[16];

// pawn attacks table [side][square]
Bitboard pawn_attacks[2][64];

// knight attacks table [square]
Bitboard knight_attacks[64];

// king attacks table [square]
Bitboard king_attacks[64];

Bitboard SQUARES_BETWEEN_BB[64][64];

// pseudo random number state
static uint64_t random_state = 6379633040001738036;

// generate 64-bit pseudo legal numbers
SMALL static uint64_t GetRandomU64Number() {
    // get current state
    uint64_t number = random_state;

    // XOR shift algorithm
    number ^= number << 13;
    number ^= number >> 7;
    number ^= number << 17;

    // update random number state
    random_state = number;

    // return random number
    return number;
}

// Initialize the Zobrist keys
SMALL void initHashKeys() {
    for (int Typeindex = WP; Typeindex <= BK; ++Typeindex) {
        for (int squareIndex = 0; squareIndex < 64; ++squareIndex) {
            PieceKeys[Typeindex][squareIndex] = GetRandomU64Number();
        }
    }
    // loop over board squares
    for (int square = 0; square < 64; square++)
        // init random enpassant keys
        enpassant_keys[square] = GetRandomU64Number();

    // loop over castling keys
    for (int index = 0; index < 16; index++)
        // init castling keys
        CastleKeys[index] = GetRandomU64Number();

    // init random side key
    SideKey = GetRandomU64Number();
}

// init attack tables for all the piece types, indexable by square
SMALL void InitAttackTables() {
    for (int square = 0; square < 64; square++) {
        // init pawn attacks
        pawn_attacks[WHITE][square] = MaskPawnAttacks(WHITE, square);
        pawn_attacks[BLACK][square] = MaskPawnAttacks(BLACK, square);

        // init knight attacks
        knight_attacks[square] = MaskKnightAttacks(square);

        // init king attacks
        king_attacks[square] = MaskKingAttacks(square);

        bishop_masks[square] = MaskBishopAttacks(square);

        // init bishop mask
        Bitboard bishop_mask = bishop_masks[square];

        // get the relevant occupancy bit count
        int relevant_bits_count = CountBits(bishop_mask);

        // init occupancy indices
        int occupancy_indices = 1 << relevant_bits_count;

        // loop over occupancy indices
        for (int index = 0; index < occupancy_indices; index++) {
            // init current occupancy variation
            Bitboard occupancy = SetOccupancy(index, relevant_bits_count, bishop_mask);

            // init magic index
            uint64_t magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - bishop_relevant_bits);

            // init bishop attacks
            bishop_attacks[square][magic_index] = BishopAttacksOnTheFly(square, occupancy);
        }
    }
}

SMALL void initializeLookupTables() {
    // initialize squares between table
    Bitboard sqs;
    for (int sq1 = 0; sq1 < 64; ++sq1) {
        for (int sq2 = 0; sq2 < 64; ++sq2) {
            sqs = (1ULL << sq1) | (1ULL << sq2);
            if (get_file(sq1) == get_file(sq2) || get_rank[sq1] == get_rank[sq2])
                SQUARES_BETWEEN_BB[sq1][sq2] = GetRookAttacks(sq1, sqs) & GetRookAttacks(sq2, sqs);
            else if (get_diagonal[sq1] == get_diagonal[sq2] || get_antidiagonal(sq1) == get_antidiagonal(sq2))
                SQUARES_BETWEEN_BB[sq1][sq2] = GetBishopAttacks(sq1, sqs) & GetBishopAttacks(sq2, sqs);
        }
    }
}

// PreCalculate the logarithms used in the reduction calculation
SMALL void InitReductions() {
    for (int depth = 0; depth < 64; depth++) {

        see_margin[depth][1] = -80.0 * depth; // Quiet moves
        see_margin[depth][0] = -30.0 * depth * depth; // Non quiets
    }

    for (int i = 1; i < 64; i++) {
        for (int j = 1; j < 64; j++) {
            reductions[0][i][j] = -0.25 + log(i) * log(j) / 2.25;
            reductions[1][i][j] = +1.00 + log(i) * log(j) / 2.00;
        }
    }
}

SMALL void InitAll() {
    TT.pTable = NULL;
    // init attacks tables
    InitAttackTables();
    initializeLookupTables();
    initHashKeys();
    InitReductions();
    // Init TT
    InitTT(768);
#ifndef OB
    NNUE_init(NULL);
#endif
}

SMALL void InitNewGame(struct ThreadData* td) {
    // Extract data structures from ThreadData
    struct Position* pos = &td->pos;
    struct SearchData* sd = &td->sd;
    struct SearchInfo* info = &td->info;

    CleanHistories(sd);

    // Reset plies and search info
    info->starttime = GetTimeMs();
    info->stopped = 0;
    info->nodes = 0;
    info->seldepth = 0;
    // Clear TT
    ClearTT();

    // delete played moves hashes
    pos->played_positions_size = 0;

    // call parse position function
    ParsePosition("position startpos", pos);
}
