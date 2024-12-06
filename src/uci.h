#pragma once

#include "move.h"

#include "shims.h"

struct Position;
struct SearchInfo;

#ifdef __cplusplus
extern "C" {
#endif
    // Parse a move from algebraic notation to the engine's internal encoding
    Move ParseMove(const char* move_string, struct Position* pos);
    // parse UCI "position" command
    void ParsePosition(const char* command, struct Position* pos);

    // parse UCI "go" command
    bool ParseGo(const char* const line, struct SearchInfo* info, struct Position* pos, const bool setStartTime);

    // main UCI loop
    void UciLoop();
#ifdef __cplusplus
}
#endif