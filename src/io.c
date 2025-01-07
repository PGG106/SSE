#include "io.h"

#include "piece_data.h"

SMALL void PrintMove(const Move move) {
    char buffer[6] = { 0 };
    square_to_coordinates(buffer, From(move));
    square_to_coordinates(buffer+2,To(move));
    if (isPromo(move)) {
        buffer[4] = promoted_pieces(getPromotedPiecetype(move));
    }
    puts_nonewline(buffer);
}

SMALL bool next_token(const char* str, int* index, char* token)
{
    if (str[*index] == '\0')
    {
        return false;
    }

    int token_len = 0;
    while (true) {
        char current = str[*index];

        if (current != '\0') {
            (*index)++;
        }

        if (current == '\0' || current == ' ')
        {
            token[token_len] = '\0';
            return true;
        }

        token[token_len] = current;
        token_len++;
    }
}