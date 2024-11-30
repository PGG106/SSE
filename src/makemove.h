#pragma once

#include "types.h"
#include "shims.h"

struct Position;

#ifdef __cplusplus
extern "C" {
#endif

	void ClearPiece(const bool UPDATE, const int piece, const int from, struct Position* pos);

	void AddPiece(const bool UPDATE, const int piece, const int to, struct Position* pos);

	void UpdateCastlingPerms(struct Position* pos, int source_square, int target_square);

	void MakeMove(const bool UPDATE, const Move move, struct Position* pos);
	// Reverts the previously played move
	void UnmakeMove(const Move move, struct Position* pos);
	// makes a null move (a move that doesn't move any piece)
	void MakeNullMove(struct Position* pos);
	// Reverts the previously played null move
	void TakeNullMove(struct Position* pos);

#ifdef __cplusplus
}
#endif