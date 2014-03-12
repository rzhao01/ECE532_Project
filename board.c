#include "board.h"
#include "assert.h"

void assert_dims () {
    assert (BOARD_ROWS == BOARD_COLS);
    assert (BOARD_ROWS*BOARD_COLS == BOARD_ELEMS);
}

void init_board (BOARD b) {
	for (int i = 0; i < BOARD_ELEMS; ++i)
		b[i] = STONE_NONE;
}

void copy_board (BOARD to, BOARD from) {
	for (int i = 0; i < BOARD_ELEMS; ++i)
		to[i] = from[i];
}

int check_board_full (BOARD b) {
    for (int i = 0; i < BOARD_ELEMS; ++i)
		if (b[i] != STONE_NONE)
			return 0;
	return 1;
}
    
void zero_counts (COUNTS* C) {
	C->p2[0] = C->p2[1] = C->p3[0] = C->p3[1] = 0;
	C->p4[0] = C->p4[1] = C->p5[0] = C->p5[1] = 0;
}
void add_counts (COUNTS* C1, COUNTS* C2) {
	C1->p2[0] += C2->p2[0]; C1->p3[0] += C2->p3[0]; 
	C1->p4[0] += C2->p4[0]; C1->p5[0] += C2->p5[0];	
	C1->p2[1] += C2->p2[1]; C1->p3[1] += C2->p3[1]; 
	C1->p4[1] += C2->p4[1]; C1->p5[1] += C2->p5[1];	
}

// traverse the board using a horizontal window of 5 squares.
// count the number of occurances of exactly n stones in the window,
// with no opposing stones in the same window
COUNTS count_horiz (ELEM b[BOARD_ELEMS]) {
	COUNTS result;
	zero_counts (&result);
	
	int row, col;
	int m;
	
	for (row = 0; row < BOARD_ROWS; ++row) {
		for (col = 0; col < BOARD_COLS-4; ++col) {
			int p1_count = 0;
			int p2_count = 0;

			for (m = 0; m < 5; ++m) {
				ELEM elem = get_square(b, row, col+m);
				p1_count += (elem == STONE_P1);
				p2_count += (elem == STONE_P2);
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

COUNTS count_vert (ELEM b[BOARD_ELEMS]) {
	COUNTS result;
	zero_counts (&result);
	
	int row, col;
	int m;

	for (col = 0; col < BOARD_COLS; ++col) {
		for (row = 0; row < BOARD_ROWS-4; ++row) {
			int p1_count = 0;
			int p2_count = 0;

			for (m = 0; m < 5; ++m) {
				ELEM elem = get_square(b, row+m, col);
				p1_count += (elem == STONE_P1);
				p2_count += (elem == STONE_P2);
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
COUNTS count_ne (ELEM b[BOARD_ELEMS]) {
	COUNTS result;
	zero_counts (&result);
	
	int row, col;
	int m;
	
	for (row = 4; row < BOARD_ROWS; ++row) {
		for (col = 0; col < BOARD_COLS-4; ++col) {
			int p1_count = 0;
			int p2_count = 0;

			for (m = 0; m < 5; ++m) {
				ELEM elem = get_square(b, row-m, col+m);
				p1_count += (elem == STONE_P1);
				p2_count += (elem == STONE_P2);
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
COUNTS count_se (ELEM b[BOARD_ELEMS]) {
	COUNTS result;
	zero_counts (&result);

	int row, col;
	int m;
	

	for (row = 0; row < BOARD_ROWS-4; ++row) {
		for (col = 0; col < BOARD_COLS-4; ++col) {
			int p1_count = 0;
			int p2_count = 0;

			for (m = 0; m < 5; ++m) {
				ELEM elem = get_square(b, row+m, col+m);
				p1_count += (elem == STONE_P1);
				p2_count += (elem == STONE_P2);
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

COUNTS generate_board_counts (ELEM b[BOARD_ELEMS]){
	COUNTS result;
	zero_counts (&result);
	
	COUNTS h = count_horiz (b);
	add_counts (&result, &h);
	COUNTS v = count_vert (b);
	add_counts (&result, &v);
	COUNTS ne = count_ne (b);
	add_counts (&result, &ne);
	COUNTS se = count_se (b);
	add_counts (&result, &se);

	return result;
}

int check_board_win (BOARD b, PLAYER P) {
	COUNTS result = generate_board_counts(b);
	if (result.p5[(int)P.num] > 0)
	    return 1;
	return 0;
}
