#include <stdio.h>
#include "common.h"
#include "board.h"

int main() {
	BOARD b;
	int i = 0, j = 0;
	for (i = 0; i < BOARD_ELEMS; ++i)
		b[i] = STONE_NONE;
	COUNTS c;
		
	set_square (b, 2,2, 1);
	set_square (b, 2,3, 1);
	set_square (b, 2,4, 1);
	set_square (b, 2,5, 1);
	set_square (b, 2,6, 1);
	set_square (b, 3,1, 1);
	set_square (b, 3,2, 1);
	set_square (b, 3,3, 1);
	set_square (b, 4,1, 1);
	set_square (b, 4,2, 1);
	set_square (b, 5,1, 1);
	

	unsigned int temp_board[8];
		for (i = 0; i < 8 ; i++){
			unsigned int temp = 0;
			for (j=0; j<16; j++){
				temp = temp | ((b[i*16 + j] & 3) << (j*2));
			}
			temp_board[i] = temp;
		}

	generate_board_counts(NULL, &temp_board, &c);
	
	FILE* fp = fopen("out.dat","w");
	fprintf (fp, "p5: %d %d\np4: %d %d\np3: %d %d\np2: %d %d\n",
			c.p5[0], c.p5[1], c.p4[0], c.p4[1], 
			c.p3[0], c.p3[1], c.p2[0], c.p2[1]);
	fclose (fp);
	
	printf ("Comparing against output data \n");
	if (system("diff -w out.dat out.gold.dat")) {
		fprintf(stdout, "*******************************************\n");
		fprintf(stdout, "FAIL: Output DOES NOT match the golden output\n");
		fprintf(stdout, "*******************************************\n");
	 return 1;
	} else {
		fprintf(stdout, "*******************************************\n");
		fprintf(stdout, "PASS: The output matches the golden output!\n");
		fprintf(stdout, "*******************************************\n");
	 return 0;
	}
	return 1;
}
