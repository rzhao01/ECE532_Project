#ifndef __BOARD_H___
#define __BOARD_H___

#include "common.h"

#ifdef MICROBLAZE
#include "xparameters.h"
#include "system/SDK_workspace/connectAI/src/xgenerate_board_counts.h"
#endif

inline ELEM get_square (BOARD b, int row, int col) {
    return b[row*BOARD_COLS + col];
}
inline void set_square (BOARD b, int row, int col, ELEM val) {
    b[row*BOARD_COLS + col] = val;
}

// check some assumptions we made about the dimensions
void assert_dims ();
void init_board (BOARD b);
void copy_board (BOARD to, BOARD from);

void generate_board_count_score (ELEM b[BOARD_ELEMS], int p, int o, int* score);

int check_board_full (BOARD b);
int check_board_win (BOARD b, PLAYER P, PLAYER O);

void print_board (BOARD b);

#ifdef MICROBLAZE
	//Initialization function
	int initialize_accelerator(XGenerate_board_counts * accel, unsigned int accelerator_base_address);
	void hardened_generate_board_count_score (ELEM b[BOARD_ELEMS], int p, int o, int* score);
	XGenerate_board_counts board_count_accelerator;
	#define BOARD_OFFSET  0x1000000
	#define RESULT_OFFSET 0x1000020
#endif

#endif
