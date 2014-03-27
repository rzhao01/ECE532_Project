#include "board.h"

void zero_counts (COUNTS* C) {
	C->p2[0] = C->p2[1] = C->p3[0] = C->p3[1] = 0;
	C->p4[0] = C->p4[1] = C->p5[0] = C->p5[1] = 0;
}
void add_counts (COUNTS* C1, COUNTS* C2) {
	C1->p2[0] += C2->p2[0]; C1->p3[0] += C2->p3[0];
	C1->p4[0] += C2->p4[0]; C1->p5[0] += C2->p5[0];
	C1->p2[1] += C2->p2[1]; C1->p3[1] += C2->p3[1];
	C1->p4[1] += C2->p4[1]; C1->p5[1] += C2->p5[1];
}

ELEM get_square (ELEM b[BOARD_ELEMS], int row, int col) {
    return b[row*BOARD_COLS + col];
}
void set_square (ELEM b[BOARD_ELEMS], int row, int col, ELEM val) {
    b[row*BOARD_COLS + col] = val;
}

// traverse the board using a horizontal window of 5 squares.
// count the number of occurances of exactly n stones in the window,
// with no opposing stones in the same window
COUNTS count_horiz (ELEM b[BOARD_ELEMS]) {
	COUNTS result;
	zero_counts (&result);
	
	COUNTER4 row, col;
	COUNTER3 m;

count_horiz_loop1:
	for (row = 0; row < BOARD_ROWS; ++row) {

count_horiz_loop2:
		for (col = 0; col < BOARD_COLS-4; ++col) {
			COUNTER3 p1_count = 0;
			COUNTER3 p2_count = 0;

count_horiz_loop3:
			for (m = 0; m < 5; ++m) {
				ELEM elem = get_square(b, row, col+m);
				p1_count += (elem == STONE_P1);
				p2_count += (elem == STONE_P2);
			}

			if (p2_count == 0) {
				result.p5[0] += (p1_count == 5);
				result.p4[0] += (p1_count == 4);
				result.p3[0] += (p1_count == 3);
				result.p2[0] += (p1_count == 2);
			}
			if (p1_count == 0) {
				result.p5[1] += (p2_count == 5);
				result.p4[1] += (p2_count == 4);
				result.p3[1] += (p2_count == 3);
				result.p2[1] += (p2_count == 2);
			}
		}
	}
	return result;
}

COUNTS count_vert (ELEM b[BOARD_ELEMS]) {
	COUNTS result;
	zero_counts (&result);
	
	COUNTER4 row, col;
	COUNTER3 m;
	
count_vert_loop1:
	for (col = 0; col < BOARD_COLS; ++col) {

count_vert_loop2:
		for (row = 0; row < BOARD_ROWS-4; ++row) {
			COUNTER3 p1_count = 0;
			COUNTER3 p2_count = 0;

count_vert_loop3:
			for (m = 0; m < 5; ++m) {
				ELEM elem = get_square(b, row+m, col);
				p1_count += (elem == STONE_P1);
				p2_count += (elem == STONE_P2);
			}

			if (p2_count == 0) {
				result.p5[0] += (p1_count == 5);
				result.p4[0] += (p1_count == 4);
				result.p3[0] += (p1_count == 3);
				result.p2[0] += (p1_count == 2);
			}
			if (p1_count == 0) {
				result.p5[1] += (p2_count == 5);
				result.p4[1] += (p2_count == 4);
				result.p3[1] += (p2_count == 3);
				result.p2[1] += (p2_count == 2);
			}
		}
	}
	return result;
}
COUNTS count_ne (ELEM b[BOARD_ELEMS]) {
	COUNTS result;
	zero_counts (&result);
	
	COUNTER4 row, col;
	COUNTER3 m;

count_ne_loop1:
	for (row = 4; row < BOARD_ROWS; ++row) {

count_ne_loop2:
		for (col = 0; col < BOARD_COLS-4; ++col) {
			COUNTER3 p1_count = 0;
			COUNTER3 p2_count = 0;

count_ne_loop3:
			for (m = 0; m < 5; ++m) {
				ELEM elem = get_square(b, row-m, col+m);
				p1_count += (elem == STONE_P1);
				p2_count += (elem == STONE_P2);
			}

			if (p2_count == 0) {
				result.p5[0] += (p1_count == 5);
				result.p4[0] += (p1_count == 4);
				result.p3[0] += (p1_count == 3);
				result.p2[0] += (p1_count == 2);
			}
			if (p1_count == 0) {
				result.p5[1] += (p2_count == 5);
				result.p4[1] += (p2_count == 4);
				result.p3[1] += (p2_count == 3);
				result.p2[1] += (p2_count == 2);
			}
		}
	}
	return result;
}
COUNTS count_se (ELEM b[BOARD_ELEMS]) {
	COUNTS result;
	zero_counts (&result);

	COUNTER4 row, col;
	COUNTER3 m;

count_se_loop1:
	for (row = 0; row < BOARD_ROWS-4; ++row) {

count_se_loop2:
		for (col = 0; col < BOARD_COLS-4; ++col) {
			COUNTER3 p1_count = 0;
			COUNTER3 p2_count = 0;

count_se_loop3:
			for (m = 0; m < 5; ++m) {
				ELEM elem = get_square(b, row+m, col+m);
				p1_count += (elem == STONE_P1);
				p2_count += (elem == STONE_P2);
			}

			if (p2_count == 0) {
				result.p5[0] += (p1_count == 5);
				result.p4[0] += (p1_count == 4);
				result.p3[0] += (p1_count == 3);
				result.p2[0] += (p1_count == 2);
			}
			if (p1_count == 0) {
				result.p5[1] += (p2_count == 5);
				result.p4[1] += (p2_count == 4);
				result.p3[1] += (p2_count == 3);
				result.p2[1] += (p2_count == 2);
			}
		}
	}
	return result;
}

void generate_board_counts (volatile unsigned int* master, unsigned int board_addr, unsigned int result_addr) {

	COUNTS result;
	#pragma HLS RESOURCE variable=master core=AXI4M
	#pragma HLS INTERFACE ap_bus depth=16 port=master
	#pragma HLS INTERFACE ap_ctrl_hs port=return
	#pragma HLS INTERFACE ap_none port=board_addr
	#pragma HLS INTERFACE ap_none port=result_addr
	#pragma HLS RESOURCE variable=result_addr core=AXI4LiteS metadata="-bus_bundle ctrl"
	#pragma HLS RESOURCE variable=board_addr core=AXI4LiteS metadata="-bus_bundle ctrl"
	#pragma HLS RESOURCE variable=return core=AXI4LiteS metadata="-bus_bundle ctrl"

	int temp[8];
	ELEM b[128];

	memcpy (temp, master + (board_addr >> 2), 8*sizeof(int));

	int i, j;
generate_board_counts_load_data:
	for (i=0; i < 8; i++) {
generate_board_counts_load_data2:
		for (j=0; j < 16; j++)
			b[i*16+j] = (temp[i] >> 2*j) & 3;
	}


	zero_counts (&result);
	
	COUNTS h = count_horiz (b);
	add_counts (&result, &h);
	COUNTS v = count_vert (b);
	add_counts (&result, &v);
	COUNTS ne = count_ne (b);
	add_counts (&result, &ne);
	COUNTS se = count_se (b);
	add_counts (&result, &se);
	memcpy(master + (result_addr>>2), &result, sizeof(COUNTS));
}
