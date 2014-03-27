#ifndef __BOARD_H___
#define __BOARD_H___

#include "common.h"

extern ELEM FULL_MASK[6];
extern ELEM BIT_MASK[6];

// check some assumptions we made about the dimensions
void assert_dims ();
void init_board (BOARD b);
void copy_board (BOARD to, BOARD from);

inline ELEM get_square (BOARD b, int row, int col) {
    return b[row*BOARD_COLS + col];
}
inline void set_square (BOARD b, int row, int col, ELEM val) {
    b[row*BOARD_COLS + col] = val;
}

COUNTS generate_board_counts (ELEM b[BOARD_ELEMS]);

int check_board_full (BOARD b);
int check_board_win (BOARD b, PLAYER P);

#endif
