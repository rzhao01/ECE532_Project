#ifndef __BOARD_H___
#define __BOARD_H___

#include "common.h"

#ifdef MICROBLAZE
#include "xparameters.h"
#include "system/SDK_workspace/connectAI/src/xgenerate_board_counts.h"
#endif

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

void generate_board_counts (ELEM b[BOARD_ELEMS], COUNTS * results);

int check_board_full (BOARD b);
int check_board_win (BOARD b, PLAYER P);

#ifdef MICROBLAZE
	//Initialization function
	int initialize_accelerator(XGenerate_board_counts * accel, unsigned int accelerator_base_address);
	void hardened_generate_board_counts (ELEM b[BOARD_ELEMS], COUNTS * results);
	XGenerate_board_counts board_count_accelerator;
	#define BOARD_OFFSET  0x1000000
	#define RESULT_OFFSET 0x1000020
#endif

#endif
