#include "history.h"

#include "move.h"
#include "position.h"
#include "search.h"
#include "options.h"
#include "shims.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define clamp(a,b,c) (((a) < (b)) ? (b) : ((a) > (c)) ? (c) : (a))
#define abs(x) ((x) < 0 ? -(x) : (x))


/* History updating works in the same way for all histories, we have 3 methods:
updateScore: this updates the score of a specific entry of <History-name> type
UpdateHistories : this performs a general update of all the heuristics, giving the bonus to the best move and a malus to all the other
GetScore: this is simply a getter for a specific entry of the history table
*/

int history_bonus(const int depth) {
    return min(16 * depth * depth + 32 * depth + 16, 1926);
}

void updateSingleCHScore(struct SearchStack* ss, const Move move, const int bonus, const int offset) {
    if ((ss - offset)->move) {
        // Scale bonus to fix it in a [-CH_MAX;CH_MAX] range
        const int scaledBonus = bonus - GetSingleCHScore(ss, move, offset) * abs(bonus) / CH_MAX;
        (*((ss - offset)->contHistEntry))[PieceTypeTo(move)] += scaledBonus;
    }
}

void updateCHScore(struct SearchStack* ss, const Move move, const int bonus) {
    // Update move score
    updateSingleCHScore(ss, move, bonus, 1);
    updateSingleCHScore(ss, move, bonus, 2);
    updateSingleCHScore(ss, move, bonus, 4);
}

void updateHHScore(const struct Position* pos, struct SearchData* sd, const Move move, const int bonus) {
    // Scale bonus to fix it in a [-HH_MAX;HH_MAX] range
    const int scaledBonus = bonus - GetHHScore(pos, sd, move) * abs(bonus) / HH_MAX;
    // Update move score
    sd->searchHistory[pos->side][FromTo(move)] += scaledBonus;
}

void updateCapthistScore(const struct Position* pos, struct SearchData* sd, const Move move, const int bonus) {
    // Scale bonus to fix it in a [-CAPTHIST_MAX;CAPTHIST_MAX] range
    const int scaledBonus = bonus - GetCapthistScore(pos, sd, move) * abs(bonus) / CAPTHIST_MAX;
    int capturedPiece = isEnpassant(move) ? PAWN : GetPieceType(Position_PieceOn(pos, To(move)));
    // If we captured an empty piece this means the move is a promotion, we can pretend we captured a pawn to use a slot of the table that would've otherwise went unused (you can't capture pawns on the 1st/8th rank)
    if (capturedPiece == EMPTY) capturedPiece = PAWN;
    // Update move score
    sd->captHist[PieceTo(move)][capturedPiece] += scaledBonus;
}

// Update all histories
void UpdateHistories(const struct Position* pos, struct SearchData* sd, struct SearchStack* ss, const int depth, const Move bestMove, const struct MoveList* quietMoves, const struct MoveList* noisyMoves, const bool rootNode) {
    const int bonus = history_bonus(depth);
    if (!isTactical(bestMove))
    {
        // increase bestMove HH and CH score
        updateHHScore(pos, sd, bestMove, bonus);
        updateCHScore(ss, bestMove, bonus);

        // Loop through all the quiet moves
        for (int i = 0; i < quietMoves->count; i++) {
            // For all the quiets moves that didn't cause a cut-off decrease the HH score
            const Move move = quietMoves->moves[i].move;
            if (move == bestMove) continue;
            updateHHScore(pos, sd, move, -bonus);
            updateCHScore(ss, move, -bonus);
        }
    }
    else {
        // increase the bestMove Capthist score
        updateCapthistScore(pos, sd, bestMove, bonus);
    }
    // For all the noisy moves that didn't cause a cut-off, even is the bestMove wasn't a noisy move, decrease the capthist score
    for (int i = 0; i < noisyMoves->count; i++) {
        const Move move = noisyMoves->moves[i].move;
        if (move == bestMove) continue;
        updateCapthistScore(pos, sd, move, -bonus);
    }
}

// Returns the history score of a move
int GetHHScore(const struct Position* pos, const struct SearchData* sd, const Move move) {
    return sd->searchHistory[pos->side][FromTo(move)];
}

// Returns the history score of a move
int GetCHScore(const struct SearchStack* ss, const Move move) {
    return   GetSingleCHScore(ss, move, 1)
             + GetSingleCHScore(ss, move, 2)
             + GetSingleCHScore(ss, move, 4);
}

int GetSingleCHScore(const struct SearchStack* ss, const Move move, const int offset) {
    return (ss - offset)->move ? (*((ss - offset)->contHistEntry))[PieceTypeTo(move)]
                               : 0;
}

// Returns the history score of a move
int GetCapthistScore(const struct Position* pos, const struct SearchData* sd, const Move move) {
    int capturedPiece = isEnpassant(move) ? PAWN : GetPieceType(Position_PieceOn(pos, To(move)));
    // If we captured an empty piece this means the move is a non capturing promotion, we can pretend we captured a pawn to use a slot of the table that would've otherwise went unused (you can't capture pawns on the 1st/8th rank)
    if (capturedPiece == EMPTY) capturedPiece = PAWN;
    return sd->captHist[PieceTo(move)][capturedPiece];
}

void updateSingleCorrHistScore(short *entry, const int scaledDiff, const int newWeight) {
    *entry = clamp((*entry * (CORRHIST_WEIGHT_SCALE - newWeight) + scaledDiff * newWeight) / CORRHIST_WEIGHT_SCALE, -CORRHIST_MAX, CORRHIST_MAX);
}

void updateCorrHistScore(const struct Position* pos, struct SearchData* sd, const struct SearchStack* ss, const int depth, const int diff) {
    const int scaledDiff = diff * CORRHIST_GRAIN;
    const int newWeight = min(depth * depth + 2 * depth + 1, 128);
    assert(newWeight <= CORRHIST_WEIGHT_SCALE);

    updateSingleCorrHistScore(&sd->pawnCorrHist[pos->side][pos->pawnKey % CORRHIST_SIZE], scaledDiff, newWeight);
    updateSingleCorrHistScore(&sd->whiteNonPawnCorrHist[pos->side][pos->whiteNonPawnKey % CORRHIST_SIZE], scaledDiff, newWeight);
    updateSingleCorrHistScore(&sd->blackNonPawnCorrHist[pos->side][pos->blackNonPawnKey % CORRHIST_SIZE], scaledDiff, newWeight);
}

int adjustEvalWithCorrHist(const struct Position* pos, const struct SearchData* sd, const struct SearchStack* ss, const int rawEval) {
    int adjustment = 0;

    adjustment += sd->pawnCorrHist[pos->side][pos->pawnKey % CORRHIST_SIZE];
    adjustment += sd->whiteNonPawnCorrHist[pos->side][pos->whiteNonPawnKey % CORRHIST_SIZE];
    adjustment += sd->blackNonPawnCorrHist[pos->side][pos->blackNonPawnKey % CORRHIST_SIZE];

    return clamp(rawEval + adjustment / CORRHIST_GRAIN, -MATE_FOUND + 1, MATE_FOUND - 1);
}

int GetHistoryScore(const struct Position* pos, const struct SearchData* sd, const Move move, const struct SearchStack* ss) {
    if (!isTactical(move))
        return GetHHScore(pos, sd, move) + GetCHScore(ss, move);
    else
        return GetCapthistScore(pos, sd, move);
}

// Resets the history tables
SMALL void CleanHistories(struct SearchData* sd) {
    memset(sd->searchHistory, 0, sizeof(sd->searchHistory));
    memset(sd->contHist, 0, sizeof(sd->contHist));
    memset(sd->captHist, 0, sizeof(sd->captHist));
    memset(sd->pawnCorrHist, 0, sizeof(sd->pawnCorrHist));
    memset(sd->whiteNonPawnCorrHist, 0, sizeof(sd->whiteNonPawnCorrHist));
    memset(sd->blackNonPawnCorrHist, 0, sizeof(sd->blackNonPawnCorrHist));
}
