#include "ai.h"
#include "board.h"
#include <stdlib.h>
#include <assert.h>

#define NABS(x) ((x) > 0 ? (-(x)) : (x))

// more weight to center squares
inline
int board_position_weight (int row, int col) {
	return BOARD_ROWS/2 + NABS(row - BOARD_ROWS/2) + BOARD_COLS/2 + NABS(col - BOARD_COLS/2);
}

int get_move_ai1 (BOARD_SET S, int player, char *row, char *col) {
	BOARD b;

	int best_row=0, best_col=0;
	long best_score = -1;
	// try every move and count number of 2,3,4's
	// maximize this number
	for (int r = 0; r < BOARD_ROWS; ++r) {
		for (int c = 0; c < BOARD_COLS; ++c) {
			if (get_square_set(S, r, c))				// occupied
				continue;

			copy_board (b, S[player]);
			set_square (b, r, c);

			long score = board_position_weight(r, c);
			score += count_horiz(b,4) + count_vert(b,4) + count_ne(b,4) + count_se(b,4);
			score = score*256 + count_horiz(b,3) + count_vert(b,3) + count_ne(b,3) + count_se(b,3);
			score = score*256 + count_horiz(b,2) + count_vert(b,2) + count_ne(b,2) + count_se(b,2);
			printf ("%d %d -> %d\n", r, c, score);

			if (score > best_score) {
				best_score = score;
				best_row = r;
				best_col = c;
			}
		}
	}

	assert (best_score >= 0);
	*row = best_row;
	*col = best_col;
	return 1;
}