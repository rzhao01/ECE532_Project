#ifndef __BOARD_H___
#define __BOARD_H___

#include "common.h"

extern ELEM FULL_MASK[6];
extern ELEM BIT_MASK[6];

// check some assumptions we made about the dimensions
void assert_dims ();
void init_board (BOARD b);
void copy_board (BOARD to, BOARD from);


ELEM get_square (BOARD b, int row, int col);
void set_square (BOARD b, int row, int col, ELEM val);

COUNTS count_horiz (BOARD b);
COUNTS count_vert (BOARD b);
COUNTS count_ne (BOARD b);
COUNTS count_se (BOARD b);

int check_board_full (BOARD b);
int check_board_win (BOARD b, PLAYER P);

#endif
