#include "board.h"
#include "assert.h"

void assert_dims () {
    assert (BOARD_ROWS == BOARD_COLS);
    assert (BOARD_ROWS*BOARD_COLS == BOARD_ELEMS);
}

void init_board (BOARD b) {
	int i;
	for (i = 0; i < BOARD_ELEMS; ++i)
		b[i] = STONE_NONE;
}

void copy_board (BOARD to, BOARD from) {
	int i;
	for (i = 0; i < BOARD_ELEMS; ++i)
		to[i] = from[i];
}

int check_board_full (BOARD b) {
	int i;
    for (i = 0; i < BOARD_ELEMS; ++i)
		if (b[i] != STONE_NONE)
			return 0;
	return 1;
}
    
void zero_counts (COUNTS* C) {
	C->p2[0] = C->p2[1] = C->p3[0] = C->p3[1] = 0;
	C->p4[0] = C->p4[1] = C->p5[0] = C->p5[1] = 0;
}

// traverse the board using a horizontal window of 5 squares.
// count the number of occurances of exactly n stones in the window,
// with no opposing stones in the same window
COUNTS count_horiz (BOARD b) {
	COUNTS result;
	zero_counts (&result);
	int row,col;
	for (row = 0; row < BOARD_ROWS; ++row) {
		for (col = 0; col < BOARD_COLS-4; ++col) {
			unsigned p1_count = 0;
			unsigned p2_count = 0;
			int m;
			for (m = 0; m < 5; ++m) {
				ELEM elem = get_square(b, row, col+m);
				if (elem == STONE_P1)
					p1_count++;
				else if (elem == STONE_P2)
					p2_count++;
			}

			if (p2_count == 0) {
				result.p5[0] += (p1_count == 5);
				result.p4[0] += (p1_count == 4);
				result.p3[0] += (p1_count == 3);
				result.p2[0] += (p1_count == 2);
			}
			if (p1_count == 0) {
				result.p5[1] += (p2_count == 5);
				result.p4[1] += (p2_count == 4);
				result.p3[1] += (p2_count == 3);
				result.p2[1] += (p2_count == 2);
			}
		}
	}
	return result;
}
COUNTS count_vert (BOARD b) {
	COUNTS result;
	zero_counts (&result);
	int col, row;
	for (col = 0; col < BOARD_COLS; ++col) {
		for (row = 0; row < BOARD_ROWS-4; ++row) {
			unsigned p1_count = 0;
			unsigned p2_count = 0;
			int m;
			for (m = 0; m < 5; ++m) {
				ELEM elem = get_square(b, row+m, col);
				if (elem == STONE_P1)
					p1_count++;
				else if (elem == STONE_P2)
					p2_count++;
			}

			if (p2_count == 0) {
				result.p5[0] += (p1_count == 5);
				result.p4[0] += (p1_count == 4);
				result.p3[0] += (p1_count == 3);
				result.p2[0] += (p1_count == 2);
			}
			if (p1_count == 0) {
				result.p5[1] += (p2_count == 5);
				result.p4[1] += (p2_count == 4);
				result.p3[1] += (p2_count == 3);
				result.p2[1] += (p2_count == 2);
			}
		}
	}
	return result;
}
COUNTS count_ne (BOARD b) {
	COUNTS result;
	zero_counts (&result);
	int row, col;
	for (row = 4; row < BOARD_ROWS; ++row) {
		for (col = 0; col < BOARD_COLS-4; ++col) {
			unsigned p1_count = 0;
			unsigned p2_count = 0;
			int m;
			for (m = 0; m < 5; ++m) {
				ELEM elem = get_square(b, row-m, col+m);
				if (elem == STONE_P1)
					p1_count++;
				else if (elem == STONE_P2)
					p2_count++;
			}

			if (p2_count == 0) {
				result.p5[0] += (p1_count == 5);
				result.p4[0] += (p1_count == 4);
				result.p3[0] += (p1_count == 3);
				result.p2[0] += (p1_count == 2);
			}
			if (p1_count == 0) {
				result.p5[1] += (p2_count == 5);
				result.p4[1] += (p2_count == 4);
				result.p3[1] += (p2_count == 3);
				result.p2[1] += (p2_count == 2);
			}
		}
	}
	return result;
}
COUNTS count_se (BOARD b) {
	COUNTS result;
	zero_counts (&result);
	int row, col;
	for (row = 0; row < BOARD_ROWS-4; ++row) {
		for (col = 0; col < BOARD_COLS-4; ++col) {
			unsigned p1_count = 0;
			unsigned p2_count = 0;
			int m;
			for (m = 0; m < 5; ++m) {
				ELEM elem = get_square(b, row+m, col+m);
				if (elem == STONE_P1)
					p1_count++;
				else if (elem == STONE_P2)
					p2_count++;
			}

			if (p2_count == 0) {
				result.p5[0] += (p1_count == 5);
				result.p4[0] += (p1_count == 4);
				result.p3[0] += (p1_count == 3);
				result.p2[0] += (p1_count == 2);
			}
			if (p1_count == 0) {
				result.p5[1] += (p2_count == 5);
				result.p4[1] += (p2_count == 4);
				result.p3[1] += (p2_count == 3);
				result.p2[1] += (p2_count == 2);
			}
		}
	}
	return result;
}

int check_board_win (BOARD b, PLAYER P) {
	COUNTS result;
	result = count_horiz(b);
	if (result.p5[(int)P.num] > 0)
		return 1;
	result = count_vert(b);
	if (result.p5[(int)P.num] > 0)
		return 1;
	result = count_ne(b);
	if (result.p5[(int)P.num] > 0)
		return 1;
	result = count_se(b);
	if (result.p5[(int)P.num] > 0)
		return 1;
	return 0;
}

ELEM get_square (BOARD b, int row, int col) {
    return b[row*BOARD_COLS + col];
}
void set_square (BOARD b, int row, int col, ELEM val) {
    b[row*BOARD_COLS + col] = val;
}
