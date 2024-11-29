#pragma once

#include <stdint.h>
#include "move.h"

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
    bool ParseGo(const char* const line, struct SearchInfo* info, struct Position* pos);

    // main UCI loop
    void UciLoop(int argc, char** argv);
#ifdef __cplusplus
}
#endif