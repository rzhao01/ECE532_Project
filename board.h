#ifndef __BOARD_H___
#define __BOARD_H___

#include "common.h"

void init_board (BOARD b);
char get_square (BOARD b, char row, char col);
void set_square (BOARD b, char row, char col, char value);

int check_board_full (BOARD b);
int check_board_win (BOARD b, char value);


#endif