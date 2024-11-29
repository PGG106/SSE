#pragma once
#include "position.h"
#include "piece_data.h"

// print move (for UCI purposes)
inline void PrintMove(const Move move) {
    const char* from = square_to_coordinates[From(move)];
    const char* to = square_to_coordinates[To(move)];

    if (isPromo(move))
        printf("%s%s%c", from, to, promoted_pieces(getPromotedPiecetype(move)));
    else
        printf("%s%s", from, to);
}

