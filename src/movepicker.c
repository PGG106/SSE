#include "movepicker.h"

#include "movegen.h"

#include "shims.h"

// ScoreMoves takes a list of move as an argument and assigns a score to each move
void ScoreMoves(struct Movepicker* mp) {
    struct MoveList* moveList = &mp->moveList;
    struct Position* pos = mp->pos;
    struct SearchData* sd = mp->sd;
    struct SearchStack* ss = mp->ss;
    // Loop through all the move in the movelist
    for (int i = mp->idx; i < moveList->count; i++) {
        const Move move = moveList->moves[i].move;
        if (isTactical(move)) {
            // Score by most valuable victim and capthist
            int capturedPiece = isEnpassant(move) ? PAWN : GetPieceType(Position_PieceOn(pos, To(move)));
            // If we captured an empty piece this means the move is a non capturing promotion, we can pretend we captured a pawn to use a slot of the table that would've otherwise went unused (you can't capture pawns on the 1st/8th rank)
            if (capturedPiece == EMPTY) capturedPiece = PAWN;
            moveList->moves[i].score = SEEValue[capturedPiece] * 16 + GetCapthistScore(pos, sd, move);
        }
        else {
            moveList->moves[i].score = GetHistoryScore(pos, sd, move, ss);
        }
    }
}

void partialInsertionSort(struct MoveList* moveList, const int moveNum) {
    int bestScore = moveList->moves[moveNum].score;
    int bestNum = moveNum;
    // starting at the number of the current move and stopping at the end of the list
    for (int index = moveNum + 1; index < moveList->count; ++index) {
        // if we find a move with a better score than our bestmove we use that as the new best move
        if (moveList->moves[index].score > bestScore) {
            bestScore = moveList->moves[index].score;
            bestNum = index;
        }
    }
    // swap the move with the best score with the move in place moveNum
    const struct ScoredMove temp = moveList->moves[moveNum];
    moveList->moves[moveNum] = moveList->moves[bestNum];
    moveList->moves[bestNum] = temp;
}

void InitMP(struct Movepicker* mp, struct Position* pos, struct SearchData* sd, struct SearchStack* ss, const Move ttMove, const enum MovepickerType movepickerType, const bool rootNode) {
    mp->moveList.count = 0;
    mp->badCaptureList.count = 0;

    const Move killer = ss->searchKiller;
    mp->movepickerType = movepickerType;
    mp->pos = pos;
    mp->sd = sd;
    mp->ss = ss;
    mp->ttMove = ttMove;
    mp->idx = 0;
    mp->stage = mp->ttMove ? PICK_TT : GEN_NOISY;
    mp->killer = killer != ttMove ? killer : NOMOVE;
}

Move NextMove(struct Movepicker* mp, const bool skip) {
top:
    if (skip) {
        // In search, the skip variable is used to dictate whether we skip quiet moves
        if (mp->movepickerType == SEARCH
            && mp->stage > PICK_GOOD_NOISY
            && mp->stage < GEN_BAD_NOISY) {
            mp->stage = GEN_BAD_NOISY;
        }

        // In qsearch, the skip variable is used to dictate whether we skip quiet moves and bad captures
        if (mp->movepickerType == QSEARCH
            && mp->stage > PICK_GOOD_NOISY) {
            return NOMOVE;
        }
    }
    switch (mp->stage) {
    case PICK_TT:
        ++mp->stage;
        // If we are in qsearch and not in check, skip quiet TT moves
        if (mp->movepickerType == QSEARCH && skip && !isTactical(mp->ttMove))
            goto top;

        // If the TT move if not pseudo legal we skip it too
        if (!IsPseudoLegal(mp->pos, mp->ttMove))
            goto top;

        return mp->ttMove;

    case GEN_NOISY:
        GenerateMoves(&mp->moveList, mp->pos, MOVEGEN_NOISY);
        ScoreMoves(mp);
        ++mp->stage;
        goto top;

    case PICK_GOOD_NOISY:
        while (mp->idx < mp->moveList.count) {
            partialInsertionSort(&mp->moveList, mp->idx);
            const Move move = mp->moveList.moves[mp->idx].move;
            const int score = mp->moveList.moves[mp->idx].score;
            const int SEEThreshold = -score / 32 + 236;
            ++mp->idx;
            if (move == mp->ttMove)
                continue;

            if (!SEE(mp->pos, move, SEEThreshold)) {
                AddMoveScored(move, score, &mp->badCaptureList);
                continue;
            }

            assert(isTactical(move));

            return move;
        }
        ++mp->stage;
        goto top;

    case PICK_KILLER:
        ++mp->stage;
        if (IsPseudoLegal(mp->pos, mp->killer))
            return mp->killer;
        goto top;

    case GEN_QUIETS:
        GenerateMoves(&mp->moveList, mp->pos, MOVEGEN_QUIET);
        ScoreMoves(mp);
        ++mp->stage;
        goto top;

    case PICK_QUIETS:
        while (mp->idx < mp->moveList.count) {
            partialInsertionSort(&mp->moveList, mp->idx);
            const Move move = mp->moveList.moves[mp->idx].move;
            ++mp->idx;
            if (move == mp->ttMove
                || move == mp->killer)
                continue;

            assert(!isTactical(move));
            return move;
        }
        ++mp->stage;
        goto top;

    case GEN_BAD_NOISY:
        // Nothing to generate lol, just reset mp->idx
        mp->idx = 0;
        ++mp->stage;
        goto top;

    case PICK_BAD_NOISY:
        while (mp->idx < mp->badCaptureList.count) {
            const Move move = mp->badCaptureList.moves[mp->idx].move;
            ++mp->idx;
            if (move == mp->ttMove)
                continue;

            assert(isTactical(move));
            return move;
        }
        return NOMOVE;

    default:
        // we should never end up here because a movepicker stage should be always be valid and accounted for
        assert(false);
        __builtin_unreachable();
    }
}
