#include "piece_data.h"

// convert ASCII character pieces to encoded constants
int char_pieces(const char ch) {
	switch (ch)
	{
	case 'P':
		return WP;
	case 'N':
		return WN;
	case 'B':
		return WB;
	case 'R':
		return WR;
	case 'Q':
		return WQ;
	case 'K':
		return WK;
	case 'p':
		return BP;
	case 'n':
		return BN;
	case 'b':
		return BB;
	case 'r':
		return BR;
	case 'q':
		return BQ;
	case 'k':
		return BK;
	default:
		__builtin_unreachable();
	}
}

// Map promoted piece to the corresponding ASCII character
char promoted_pieces(const int piece) {
	switch (piece)
	{
	case WQ:
		return 'q';
	case WR:
		return 'r';
	case WB:
		return 'b';
	case WN:
		return 'n';
	case BQ:
		return 'q';
	case BR:
		return 'r';
	case BB:
		return 'b';
	case BN:
		return 'n';
	default:
		__builtin_unreachable();
	}
}