#include "init.h"
#include "bench.h"
#include "uci.h"
#include "io.h"

#ifdef NOSTDLIB
SMALL __attribute__((naked)) void _start() {
#ifdef UCI
    register long* stack asm("rsp");
    int argc = (int)*stack;
    char** argv = (char**)(stack + 1);
#endif
#else
SMALL int main(int argc, char **argv) {
#endif

#ifdef UCI
    bool bench = argc > 1 && !strcmp(argv[1], "bench");
#endif
    InitAll();
#ifdef UCI
    if (bench) {
        StartBench(14);
    } else {
#endif
#ifdef OB
        if (argc > 1) {
            // assume correct input for private engines
            char input[1024] = { 0 };
            for (int i = 0;; i++) {
                input[i] = argv[1][i];
                if (argv[1][i] == 0) {
                    break;
                }
            }
            char token[1024];
            int input_index = 0;
            next_token(input, &input_index, token); // setoption
            next_token(input, &input_index, token); // name
            next_token(input, &input_index, token); // EvalFile
            next_token(input, &input_index, token); // value
            next_token(input, &input_index, token);
            NNUE_init(token);
            StartBench(14);
            exit(0);
        }
#endif
        UciLoop();
#ifdef UCI
    }
#endif

#ifdef NOSTDLIB
    exit(0);
#else
    return 0;
#endif
}
