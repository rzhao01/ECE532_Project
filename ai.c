#include "ai.h"
#include "board.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define NABS(x) ((x) > 0 ? (-(x)) : (x))
#define CP5 1024*1024*1024
#define CO4 1024*1024*8
#define CP4 1024*1024
#define CO3 1024*8
#define CP3 1024
#define CO2 8
#define CP2 8

// more weight to center squares
inline
long board_position_weight (int row, int col) {
	return BOARD_ROWS/2 + NABS(row - BOARD_ROWS/2) + BOARD_COLS/2 + NABS(col - BOARD_COLS/2);
}

long board_count_score (BOARD_SET S, int p, int o) {
	long score;

	COUNTS horiz_p = count_horiz (S, p, o);
	COUNTS vert_p = count_vert (S, p, o);
	COUNTS ne_p = count_ne (S, p, o);
	COUNTS se_p = count_se (S, p, o);

	int p5 = horiz_p.p5 + vert_p.p5 + ne_p.p5 + se_p.p5;
	int p4 = horiz_p.p4 + vert_p.p4 + ne_p.p4 + se_p.p4;
	int p3 = horiz_p.p3 + vert_p.p3 + ne_p.p3 + se_p.p3;
	int p2 = horiz_p.p2 + vert_p.p2 + ne_p.p2 + se_p.p2;
	int o4 = horiz_p.o4 + vert_p.o4 + ne_p.o4 + se_p.o4;
	int o3 = horiz_p.o3 + vert_p.o3 + ne_p.o3 + se_p.o3;
	int o2 = horiz_p.o2 + vert_p.o2 + ne_p.o2 + se_p.o2;

	printf ("Board Evaluation:\n");
	printf ("\tp5=%3d, o5=%3d\n\tp4=%3d, o4=%3d\n\tp3=%3d, o3=%3d\n\tp2=%3d, o2=%3d\n", 
			p5,0, p4,o4, p3,o3, p2,o2);

	score = CP5*p5 - CO4*o4 + CP4*p4 - CO3*o3 + CP3*p3 - CO2*o2 + CP2*p2;
	return score;
}

int get_move_ai1 (BOARD_SET S, int p, int o, char *row, char *col) {
	BOARD_SET ss;

	int best_row=0, best_col=0;
	long best_score = -1000000000;
	// try every move and count number of 2,3,4's
	// maximize this number
	for (int r = 0; r < BOARD_ROWS; ++r) {
		for (int c = 0; c < BOARD_COLS; ++c) {
			if (get_square_set(S, r, c))				// occupied
				continue;

			copy_board_set (ss, S);
			set_square (ss[p], r, c);

			long score = board_position_weight(r, c) + board_count_score(ss, p, o);
			
			printf ("Score (%2d,%2d) -> %ld\n", r, c, score);

			if (score > best_score || ((score==best_score) && (rand()%2 == 1))) {
				best_score = score;
				best_row = r;
				best_col = c;
			}
		}
	}

	*row = best_row;
	*col = best_col;
	return 1;
}