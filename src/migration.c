#include "migration.h"

bool next_token(const char* str, int* index, char* token)
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