#include "board.h"

void init_board (BOARD b) {
	for (int i = 0; i < BOARD_MEM_SIZE; ++i)
		b[i] = 0;
}

char get_square (BOARD b, char row, char col) {
	return b[row*BOARD_COLS + col];
}

void set_square (BOARD b, char row, char col, char value) {
	b[row*BOARD_COLS + col] = value;
}

int check_board_full (BOARD b) {
	char * ptr = b;
	for (int n = 0; n < BOARD_ROWS*BOARD_COLS; ++n) {
		if (*ptr != EMPTY)
			return 0;
		ptr++;
	}
	return 1;
}

inline int count_horiz (char* ptr, char value) {
	return ((ptr[0]==value) + (ptr[1]==value) + (ptr[2]==value) + (ptr[3]==value) + (ptr[4]==value));
}
inline int count_vert (char* ptr, char value) {
	return ((ptr[0]==value) + (ptr[1*BOARD_COLS]==value) + (ptr[2*BOARD_COLS]==value) + (ptr[3*BOARD_COLS]==value) + (ptr[4*BOARD_COLS]==value));
}
inline int count_ne (char* ptr, char value) {
	return ((ptr[0]==value) + (ptr[1-1*BOARD_COLS]==value) + (ptr[2-2*BOARD_COLS]==value) + (ptr[3-3*BOARD_COLS]==value) + (ptr[4-4*BOARD_COLS]==value));
}
inline int count_se (char* ptr, char value) {
	return ((ptr[0]==value) + (ptr[1+1*BOARD_COLS]==value) + (ptr[2+2*BOARD_COLS]==value) + (ptr[3+3*BOARD_COLS]==value) + (ptr[4+4*BOARD_COLS]==value));
}

int check_board_win (BOARD b, char value) {
	char* ptr;
	for (int row = 0; row < BOARD_ROWS; ++row) {
		ptr = b + row*BOARD_COLS;
		for (int col = 0; col < BOARD_COLS-4; ++col) {
			if (count_horiz(ptr, value) == 5)
				return 1;
			ptr++;
		}
	}
	for (int row = 0; row < BOARD_ROWS-4; ++row) {
		ptr = b + row*BOARD_COLS;
		for (int col = 0; col < BOARD_COLS; ++col) {
			if (count_vert(ptr, value) == 5)
				return 1;
			ptr++;
		}
	}
	for (int row = 4; row < BOARD_ROWS; ++row) {
		ptr = b + row*BOARD_COLS;
		for (int col = 0; col < BOARD_COLS-4; ++col) {
			if (count_ne(ptr, value) == 5)
				return 1;
			ptr++;
		}
	}
	for (int row = 0; row < BOARD_ROWS-4; ++row) {
		ptr = b + row*BOARD_COLS;
		for (int col = 0; col < BOARD_COLS-4; ++col) {
			if (count_se(ptr, value) == 5)
				return 1;
			ptr++;
		}
	}


	return 0;
}