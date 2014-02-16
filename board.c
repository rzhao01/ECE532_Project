#include "board.h"
#include "assert.h"

// X in a row bit masks, X from 0 to 5
ELEM FULL_MASK[6] = {0x0, 0x1, 0x3, 0x7, 0xF, 0x1F};
ELEM BIT_MASK[6] = {0x0, 0x1, 0x2, 0x4, 0x8, 0x10};

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
void copy_board_set (BOARD_SET to, BOARD_SET from) {
	copy_board (to[0], from[0]);
	copy_board (to[1], from[1]);
}

int check_board_full (BOARD_SET S) {
	for (int i = 0; i < BOARD_ROWS; ++i)
		if ((S[0][i] | S[1][i]) != 0xFFFF)
			return 0;
	return 1;
}

// each count function counts the number of n in a rows
// so count_horiz with n = 3 counts the number of occurances of 3 in a row
int count_horiz (BOARD_SET S, int p, int n) {
	ELEM mask;
	int result = 0;
	for (int col = 0; col < BOARD_COLS-n+1; ++col) {		// in this loop calculate the row mask
		mask = FULL_MASK[n] << col;
		for (int row = 0; row < BOARD_ROWS; ++row) {
			if ((S[p][row] & mask) == mask)
				result++;
		}
	}
	return result;
}
int count_vert (BOARD_SET S, int p, int n) {
	ELEM mask;
	int result = 0;
	for (int col = 0; col < BOARD_COLS; ++col) {			// row mask is always 1
		mask = 1 << col;
		for (int row = 0; row < BOARD_ROWS-n+1; ++row) {	// each row
			int count = 0;
			for (int m = row; m < row+n; ++m)				// each sequence of n rows
				if ((S[p][m] & mask) == mask)
					count++;
			result += (count == n);
		}
	}
	return result;
}
int count_ne (BOARD_SET S, int p, int n) {
// similar to count_vert but each row uses a different mask offset by 1 bit
	ELEM mask[n];
	for (int i = 0; i < n; ++i)
		mask[i] = BIT_MASK[n-i];

	int result = 0;
	for (int col = 0; col < BOARD_COLS-n+1; ++col) {		// row mask is always 1
		for (int row = 0; row < BOARD_ROWS-n+1; ++row) {	// each row
			int count = 0;
			for (int m = 0; m < n; ++m)						// each sequence of n rows
				if ((S[p][row+m] & mask[m]) == mask[m])
					count++;
			result += (count == n);
		}

		for (int i = 0; i < n; ++i)							// shift each mask by 1
			mask[i] = mask[i] << 1;
	}
	return result;
}
int count_se (BOARD_SET S, int p, int n) {
	// similar to count_vert but each row uses a different mask offset by 1 bit
	ELEM mask[n];
	for (int i = 0; i < n; ++i)
		mask[i] = BIT_MASK[i+1];

	int result = 0;
	for (int col = 0; col < BOARD_COLS-n+1; ++col) {		// row mask is always 1
		for (int row = 0; row < BOARD_ROWS-n+1; ++row) {	// each row
			int count = 0;
			for (int m = 0; m < n; ++m)						// each sequence of n rows
				if ((S[p][row+m] & mask[m]) == mask[m])
					count++;
			result += (count == n);
		}

		for (int i = 0; i < n; ++i)							// shift each mask by 1
			mask[i] = mask[i] << 1;
	}
	return result;
}

int check_board_win (BOARD_SET S, PLAYER player) {
	if (count_horiz(S, player.num, 5) > 0)
		return 1;
	if (count_vert(S, player.num, 5) > 0)
		return 1;
	if (count_ne(S, player.num, 5) > 0)
		return 1;
	if (count_se(S, player.num, 5) > 0)
		return 1;
	return 0;
}