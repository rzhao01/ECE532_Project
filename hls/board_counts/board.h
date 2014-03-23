#ifndef __BOARD_H___
#define __BOARD_H___

#include "common.h"

ELEM get_square (ELEM b[BOARD_ELEMS], int row, int col);
void set_square (ELEM b[BOARD_ELEMS], int row, int col, ELEM val);

COUNTS count_horiz (ELEM b[BOARD_ELEMS]);
COUNTS count_vert (ELEM b[BOARD_ELEMS]);
COUNTS count_ne (ELEM b[BOARD_ELEMS]);
COUNTS count_se (ELEM b[BOARD_ELEMS]);

void generate_board_counts(volatile unsigned int* master, unsigned int board_addr, unsigned int result_addr);

#endif
