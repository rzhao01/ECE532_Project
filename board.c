#include "board.h"
#include "assert.h"

void assert_dims () {
    assert (BOARD_ROWS == BOARD_COLS);
    assert (BITS_PER_ELEM == 8*sizeof(ELEM));
    assert (BOARD_ELEMS*BITS_PER_ELEM >= BOARD_ROWS*BOARD_COLS);
    assert (BOARD_ELEMS*BITS_PER_ELEM - BOARD_ROWS*BOARD_COLS < BITS_PER_ELEM);
}

void init_board (BOARD b) {
	for (int i = 0; i < BOARD_ELEMS; ++i)
		b[i] = 0;
}
void init_board_set (BOARD_SET S) {
	init_board(S[0]);
	init_board(S[1]);
}

void copy_board (BOARD to, BOARD from) {
	for (int i = 0; i < BOARD_ELEMS; ++i)
		to[i] = from[i];
}
void copy_board_set (BOARD_SET to, BOARD_SET from) {
	copy_board (to[0], from[0]);
	copy_board (to[1], from[1]);
}

int check_board_full (BOARD_SET S) {
        int count = 0;

	for (int i = 0; i < BOARD_ELEMS-1; ++i) {
	    if ((S[0][i] | S[1][i]) != 0xFFFF)
		return 0;
            count += BITS_PER_ELEM;
        }

        int last = S[0][BOARD_ELEMS-1] | S[1][BOARD_ELEMS-1]; 
        for (int i = 0; i < BITS_PER_ELEM; i++) {
            if ((last >> i) & 1)
                count++;
        }

	return count == BOARD_ROWS*BOARD_COLS;
}

int get_square (BOARD b, int row, int col) {
    int index = row*BOARD_COLS + col;
    int elem = index / BITS_PER_ELEM;
    int bit = index - elem*BITS_PER_ELEM;

    return (b[elem] >> bit) & 1;
}

void set_square (BOARD b, int row, int col) {
    int index = row*BOARD_COLS + col;
    int elem = index / BITS_PER_ELEM;
    int bit = index - elem*BITS_PER_ELEM;

    b[elem] |= (1 << bit);
}

void unset_square (BOARD b, int row, int col) {
    int index = row*BOARD_COLS + col;
    int elem = index / BITS_PER_ELEM;
    int bit = index - elem*BITS_PER_ELEM;

    b[elem] &= ~(1 << bit);
}
    
void zero_counts (COUNTS* C) {
	C->p2 = C->p3 = C->p4 = C->p5 = 0;
	C->o2 = C->o3 = C->o4 = C->o5 = 0;
}

// traverse the board using a horizontal window of 5 squares.
// count the number of occurances of exactly n stones in the window,
// with no opposing stones in the same window
COUNTS count_horiz (BOARD_SET S, int p, int o) {
	COUNTS result;
	zero_counts (&result);
	for (int row = 0; row < BOARD_ROWS; ++row) {
		for (int col = 0; col < BOARD_COLS-4; ++col) {
			unsigned p_count = 0;
			unsigned o_count = 0;

			for (int m = 0; m < 5; ++m) {
				if (get_square(S[o], row, col+m))
					o_count++;
				if (get_square(S[p], row, col+m))
					p_count++;
			}

			if (o_count == 0) {
				result.p5 += (p_count == 5);
				result.p4 += (p_count == 4);
				result.p3 += (p_count == 3);
				result.p2 += (p_count == 2);
			}
			if (p_count == 0) {
				result.o5 += (o_count == 5);
				result.o4 += (o_count == 4);
				result.o3 += (o_count == 3);
				result.o2 += (o_count == 2);
			}
		}
	}
	return result;
}
COUNTS count_vert (BOARD_SET S, int p, int o) {
	COUNTS result;
	zero_counts (&result);
	for (int col = 0; col < BOARD_COLS; ++col) {
		for (int row = 0; row < BOARD_ROWS-4; ++row) {
			unsigned p_count = 0;
			unsigned o_count = 0;

			for (int m = 0; m < 5; ++m) {
				if (get_square(S[o], row+m, col))
					o_count++;
				if (get_square(S[p], row+m, col))
					p_count++;
			}

			if (o_count == 0) {
				result.p5 += (p_count == 5);
				result.p4 += (p_count == 4);
				result.p3 += (p_count == 3);
				result.p2 += (p_count == 2);
			}
			if (p_count == 0) {
				result.o5 += (o_count == 5);
				result.o4 += (o_count == 4);
				result.o3 += (o_count == 3);
				result.o2 += (o_count == 2);
			}
		}
	}
	return result;
}
COUNTS count_ne (BOARD_SET S, int p, int o) {
	COUNTS result;
	zero_counts (&result);
	for (int row = 4; row < BOARD_ROWS; ++row) {
		for (int col = 0; col < BOARD_COLS-4; ++col) {	
			unsigned p_count = 0;
			unsigned o_count = 0;

			for (int m = 0; m < 5; ++m) {
				if (get_square(S[o], row-m, col+m))
					o_count++;
				if (get_square(S[p], row-m, col+m))
					p_count++;
			}

			if (o_count == 0) {
				result.p5 += (p_count == 5);
				result.p4 += (p_count == 4);
				result.p3 += (p_count == 3);
				result.p2 += (p_count == 2);
			}
			if (p_count == 0) {
				result.o5 += (o_count == 5);
				result.o4 += (o_count == 4);
				result.o3 += (o_count == 3);
				result.o2 += (o_count == 2);
			}
		}
	}
	return result;
}
COUNTS count_se (BOARD_SET S, int p, int o) {
	COUNTS result;
	zero_counts (&result);
	for (int row = 0; row < BOARD_ROWS-4; ++row) {
		for (int col = 0; col < BOARD_COLS-4; ++col) {	
			unsigned p_count = 0;
			unsigned o_count = 0;

			for (int m = 0; m < 5; ++m) {
				if (get_square(S[o], row+m, col+m))
					o_count++;
				if (get_square(S[p], row+m, col+m))
					p_count++;
			}

			if (o_count == 0) {
				result.p5 += (p_count == 5);
				result.p4 += (p_count == 4);
				result.p3 += (p_count == 3);
				result.p2 += (p_count == 2);
			}
			if (p_count == 0) {
				result.o5 += (o_count == 5);
				result.o4 += (o_count == 4);
				result.o3 += (o_count == 3);
				result.o2 += (o_count == 2);
			}
		}
	}
	return result;
}

int check_board_win (BOARD_SET S, PLAYER player, PLAYER opp) {
	COUNTS result;
	result = count_horiz(S, player.num, opp.num);
	if (result.p5 > 0)
		return 1;
	result = count_vert(S, player.num, opp.num);
	if (result.p5 > 0)
		return 1;
	result = count_ne(S, player.num, opp.num);
	if (result.p5 > 0)
		return 1;
	result = count_se(S, player.num, opp.num);
	if (result.p5 > 0)
		return 1;
	return 0;
}
