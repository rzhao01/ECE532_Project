#include "ai.h"
#include "board.h"
#include <stdlib.h>

int get_move_ai1 (BOARD_SET S, char *row, char *col) {
	do {
		*row = rand() % BOARD_ROWS;
		*col = rand() % BOARD_COLS;
	} while (get_square_set(S, *row, *col));
	return 1;
}