#include "ai.h"
#include "board.h"
#include <stdlib.h>

int get_move_ai1 (BOARD b, char *row, char *col) {
	do {
		*row = rand() % BOARD_ROWS;
		*col = rand() % BOARD_COLS;
	} while (get_square(b, *row, *col) != EMPTY);
	return 1;
}