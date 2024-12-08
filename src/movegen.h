#pragma once

#include "types.h"
#include "shims.h"

struct Position;
struct MoveList;

enum MovegenType {
    MOVEGEN_NOISY = 1,
    MOVEGEN_QUIET = 2,
    MOVEGEN_ALL   = 3
};

#ifdef __cplusplus
extern "C" {
#endif
    // is the square given in input attacked by the current given side
    bool IsSquareAttacked(const struct Position* pos, const int square, const int side);

    // function that adds a (not yet scored) move to a move list
    void AddMoveNonScored(const Move move, struct MoveList* list);

    // function that adds an (already-scored) move to a move list
    void AddMoveScored(const Move move, const int score, struct MoveList* list);

    // Check for move pseudo-legality
    bool IsPseudoLegal(const struct Position* pos, const Move move);

    // Check for move legality
    bool IsLegal(struct Position* pos, const Move move);

    // generate moves
    void GenerateMoves(struct MoveList* move_list, struct Position* pos, enum MovegenType type);
#ifdef __cplusplus
}
#endif
