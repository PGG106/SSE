#include "uci.h"

#include "bench.h"
#include "init.h"
#include "io.h"
#include "movegen.h"
#include "position.h"
#include "time_manager.h"
#include "search.h"

#include "shims.h"

// Parse a move from algebraic notation to the engine's internal encoding
SMALL Move ParseMove(const char* moveString, struct Position* pos) {
    // create move list instance
    struct MoveList moveList;
    moveList.count = 0;

    // generate moves
    GenerateMoves(&moveList, pos, MOVEGEN_ALL);

    // parse source square
    const int sourceSquare = (moveString[0] - 'a') + (8 - (moveString[1] - '0')) * 8;

    // parse target square
    const int targetSquare = (moveString[2] - 'a') + (8 - (moveString[3] - '0')) * 8;

    // loop over the moves within a move list
    for (int move_count = 0; move_count < moveList.count; move_count++) {
        // init move
        const int move = moveList.moves[move_count].move;
        // make sure source & target squares are available within the generated move
        if (sourceSquare == From(move) &&
            targetSquare == To(move)) {
            // promoted piece is available
            if (isPromo(move)) {
                switch (getPromotedPiecetype(move)) {
                case QUEEN:
                    if (moveString[4] == 'q')
                        return move;
                    break;
                case ROOK:
                    if (moveString[4] == 'r')
                        return move;
                    break;
                case BISHOP:
                    if (moveString[4] == 'b')
                        return move;
                    break;
                case KNIGHT:
                    if (moveString[4] == 'n')
                        return move;
                    break;
                }
                // continue the loop on possible wrong promotions (e.g. "e7e8f")
                continue;
            }
            // return legal move
            return move;
        }
    }
    printf("Illegal move parsed: %s", (size_t)moveString);

    // return illegal move
    return NOMOVE;
}

// parse UCI "go" command, returns true if we have to search afterwards and false otherwise
SMALL bool ParseGo(const char * const line, struct SearchInfo* info, struct Position* pos) {
    ResetInfo(info);
    int depth = -1;
    int time = -1, inc = 0;

    char token[128];
    int token_index = 0;

    // loop over all the tokens and parse the commands
    while (next_token(line, &token_index, token)) {
        if (!strcmp(token, "binc") && pos->side == BLACK) {
            next_token(line, &token_index, token);
            inc = atoi(token);
        }

        if (!strcmp(token, "winc") && pos->side == WHITE) {
            next_token(line, &token_index, token);
            inc = atoi(token);
        }

        if (!strcmp(token, "wtime") && pos->side == WHITE) {
            next_token(line, &token_index, token);
            time = atoi(token);
            info->timeset = true;
        }
        if (!strcmp(token, "btime") && pos->side == BLACK) {
            next_token(line, &token_index, token);
            time = atoi(token);
            info->timeset = true;
        }
    }

    info->starttime = GetTimeMs();
    info->depth = depth;
    // calculate time allocation for the move
    Optimum(info, time, inc);

    return true;
}

// parse UCI "position" command
SMALL void ParsePosition(const char* command, struct Position* pos) {
    // parse UCI "startpos" command
    if (strstr(command, "startpos") != NULL) {
        // init chess board with start position
        ParseFen(start_position, pos);
    }

    // parse UCI "fen" command
    else {
        // if a "fen" command is available within command string
        if (strstr(command, "fen") != NULL) {
            // init chess board with position from FEN string
            const char* fen_start_ptr = strstr(command, "fen") + 4;
            ParseFen(fen_start_ptr, pos);
        }
    }

#if UCI
    // if there are moves to be played in the fen play them
    if (strstr(command, "moves") != NULL) {
        pos->played_positions_size = 0;
        //auto string_start = command.find("moves") + 6;
        const char* string_start_ptr = strstr(command, "moves") + 6;
        // Avoid looking for a moves that doesn't exist in the case of "position startpos moves <blank>" (Needed for Scid support)
        if (*string_start_ptr != '\0') {
            const char* moves_substr = string_start_ptr;
            parse_moves(moves_substr, pos);
        }
    }
#endif

    // Update accumulator state to reflect the new position
    NNUE_accumulate(&pos->accumStack[0], pos);
    pos->accumStackHead = 1;
}

// main UCI loop
SMALL void UciLoop() {
#if BENCH
    int benchDepth = 14;
    StartBench(benchDepth);
    return;
#endif

    struct ThreadData td;
    init_thread_data(&td);

    // main loop
    while (true) {
#if UCI
        char input[8192];
#else
        char input[256];
#endif

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        // make sure input is available
        if (input[0] == '\0') {
            continue;
        }

        size_t len = strlen(input);
        if (input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        char token[128];
        int input_index = 0;
        next_token(input, &input_index, token);

        // parse UCI "position" command
        if (!strcmp(token, "position")) {
            // call parse position function
            ParsePosition(input, &td.pos);
        }

        // parse UCI "go" command
        else if (!strcmp(token, "go")) {
            // call parse go function
            bool search = ParseGo(input, &td.info, &td.pos);
            // Start search in a separate thread
            RootSearch(MAXDEPTH, &td);
        }

#if UCI
        // parse UCI "isready" command
        else if (!strcmp(token, "isready")) {
            puts("readyok");
            fflush(stdout);
            continue;
        }

        // parse UCI "uci" command
        else if (!strcmp(token, "uci")) {
            // print engine info
            puts("id name SSE 0.1");
            puts("id author Zuppa, CJ and Gedas\n");
            puts("uciok");
            fflush(stdout);
        }

        // parse UCI "ucinewgame" command
        else if (!strcmp(token, "ucinewgame")) {
            InitNewGame(&td);
        }

        else if (!strcmp(token, "eval")) {
            // print position eval
            printf("Raw eval: %i\n", EvalPositionRaw(&td.pos));
            printf("Scaled eval: %i\n", EvalPosition(&td.pos));
        }
        else printf("Unknown command: %s\n", (size_t)input);
#endif
    }
}
