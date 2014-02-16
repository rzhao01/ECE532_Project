#include "board.h"
#include "assert.h"

// X in a row bit masks, X from 0 to 5
ELEM BIT_MASK[6] = {0x0, 0x1, 0x3, 0x7, 0xF, 0x1F};

void assert_dims () {
	assert (BOARD_ROWS == BOARD_COLS);
	assert (sizeof(ELEM)*8 >= BOARD_COLS);
}

void init_board (BOARD b) {
	for (int i = 0; i < BOARD_ROWS; ++i)
		b[i] = 0;
}
void init_board_set (BOARD_SET S) {
	init_board(S[0]);
	init_board(S[1]);
}

void copy_board (BOARD to, BOARD from) {
	for (int i = 0; i < BOARD_ROWS; ++i)
		to[i] = from[i];
}

int check_board_full (BOARD_SET S) {
	for (int i = 0; i < BOARD_ROWS; ++i)
		if ((S[0][i] | S[1][i]) != 0xFFFF)
			return 0;
	return 1;
}

// each count function counts the number of n in a rows
// so count_horiz with n = 3 counts the number of occurances of 3 in a row
int count_horiz (BOARD b, int n) {
	ELEM mask;
	int result = 0;
	for (int col = 0; col < BOARD_COLS-n+1; ++col) {		// in this loop calculate the row mask
		mask = BIT_MASK[n] << col;
		for (int row = 0; row < BOARD_ROWS; ++row) {
			if ((b[row] & mask) == mask)
				result++;
		}
	}
	return result;
}
int count_vert (BOARD b, int n) {
	ELEM mask;
	int result = 0;
	for (int col = 0; col < BOARD_COLS; ++col) {			// row mask is always 1
		mask = 1 << col;
		for (int row = 0; row < BOARD_ROWS-n+1; ++row) {	// each row
			int count = 0;
			for (int m = row; m < row+n; ++m)				// each sequence of n rows
				if ((b[m] & mask) == mask)
					count++;
			result += (count == n);
		}
	}
	return result;
}
int count_ne (BOARD b, int n) {
	return 0;
}
int count_se (BOARD b, int n) {
	return 0;
}

int check_board_win (BOARD b) {
	if (count_horiz(b,5) > 0)
		return 1;
	if (count_vert(b,5) > 0)
		return 1;
	if (count_ne(b,5) > 0)
		return 1;
	if (count_se(b,5) > 0)
		return 1;
	return 0;
}