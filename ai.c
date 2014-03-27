#include "board.h"
#include "ai.h"
#include <stdlib.h>
#include <stdio.h>


#define DEBUG
//#define PRUNING

#ifdef MICROBLAZE
	#include "xil_assert.h"
	#define printf xil_printf
		#ifdef DEBUG
		#define debug_printf(format, ...) xil_printf(format, ##__VA_ARGS__)
	#else
		#define debug_printf(format, ...)
	#endif

#else
	#include <limits.h>
	#include "graphics.h"
	#include <assert.h>


	#ifdef DEBUG
		#define debug_printf(format, ...) printf(format, ##__VA_ARGS__)
	#else
		#define debug_printf(format, ...)
	#endif

#endif


#define NABS(x) ((x) > 0 ? (-(x)) : (x))
#define PARITY(x) ((x) % 2 == 0 ? (-1) : (1))

AI_PLAYER default_ai () {
    AI_PLAYER ai;
    ai.CP4 = C_P4;
    ai.CO3 = C_O3;
    ai.CP3 = C_P3;
    ai.CO2 = C_O2;
    ai.CP2 = C_P2;
    return ai;
}

// more weight to center squares
inline int board_position_weight (BOARD b, PLAYER P, PLAYER O) {
    int score = 0;
    int r, c;
    for (r = 0; r < BOARD_ROWS; ++r) {
        for (c = 0; c < BOARD_COLS; ++c) {
            ELEM elem = get_square (b, r, c);
            if (elem == P.stone) // occupied
                score += 2*BOARD_ROWS + NABS(r - BOARD_ROWS/2) + NABS(c - BOARD_COLS/2);
            else if (elem == O.stone)
                score -= 2*BOARD_ROWS + NABS(r - BOARD_ROWS/2) + NABS(c - BOARD_COLS/2);
        }
    }
    //printf ("Board Position Weight = %d\n", score);
    return score; 
}

int board_count_score (BOARD b, PLAYER P, PLAYER O) {
    int p = P.num;
    int o = O.num;
    int score;

	#ifdef MICROBLAZE
    hardened_generate_board_count_score(b, p, o, &score);
	#else
    generate_board_count_score (b, p, o, &score);
	#endif
    
    return score;
}

// get the best move
int get_best_move (BOARD b, PLAYER P, PLAYER O, COORD* move) {
    BOARD bb;
    int best_row=0, best_col=0, r, c;
    int best_score = -MAX_SCORE;
    // try every move and maximize the board score
    for (r = 0; r < BOARD_ROWS; ++r) {
        for (c = 0; c < BOARD_COLS; ++c) {
            if (get_square (b, r, c)) // occupied
                continue;

            copy_board (bb, b);
            set_square (bb, r, c, (int)P.num+1);

            int score = board_position_weight(b, P, O) + board_count_score(bb, P, O);
            
            //printf ("Score (%2d,%2d) -> %d\n", r, c, score);

            if (score > best_score || ((score==best_score) && (rand()%2 == 1))) {
                best_score = score;
                best_row = r;
                best_col = c;
            }
        }
    }
    move->row = best_row;
    move->col = best_col;
    return best_score;
}

// get the MOVE_BREADTH best moves
int get_best_move_n (BOARD b, PLAYER P, PLAYER O, COORD moves[MOVE_BREADTH]) {
    BOARD bb;
    int best_score[MOVE_BREADTH];
    int i;
    for (i = 0; i < MOVE_BREADTH; ++i) {
        moves[i].row = -1;
        best_score[i] = -MAX_SCORE;
    }

    int n_moves = 0;
    int r, c;

    // try every move and maximize the board score
    for (r = 0; r < BOARD_ROWS; ++r) {
        for (c = 0; c < BOARD_COLS; ++c) {
            if (get_square (b, r, c)) // occupied
                continue;

            copy_board (bb, b);
            set_square (bb, r, c, (int)P.num+1);

            int score = board_position_weight(bb, P, O) + board_count_score(bb, P, O);

            int i, j;
            printf ("\tScored move (%2d,%2d) -> %5d\n", r, c, score);
            for (i = 0; i < MOVE_BREADTH; i++) {
                if (score > best_score[i] || ((score==best_score[i]) && (rand()%2 == 1)) || moves[i].row == -1) {
                    // shift every move and score down
                    for (j = MOVE_BREADTH-2; j >= i; --j) {
                        best_score[j+1] = best_score[j];
                        moves[j+1] = moves[j];
                    }

                    best_score[i] = score;
                    moves[i].row = r;
                    moves[i].col = c;
                    n_moves++;
                    break;
                }
            }
        }
    }

    /*printf ("get_best_move_n\n");
    for (int i = 0; i < MOVE_BREADTH; ++i) {
        printf ("\t(%2d,%2d): score %d\n", moves[i].row, moves[i].col, best_score[i]);
    }*/

    if (n_moves > MOVE_BREADTH) n_moves = MOVE_BREADTH;
    return n_moves;
}

int get_move_ai1 (BOARD b, PLAYER P, PLAYER O, COORD* move) {
	get_best_move (b, P, O, move);
	return 1;
}


int tree_search (int layer, BOARD b, PLAYER P, PLAYER O, COORD* move, int prev_layer_score) {
    int i, score_to_return;
    for (i = 0; i < layer; i++) debug_printf ("  ");
    debug_printf ("layer %d, P%d. move(%2d,%2d) ", layer, P.stone, move->row, move->col); 

    // play the move passed to us except for the root node
    if (layer != 0) {
	#ifdef MICROBLAZE
    	Xil_AssertNonvoid(move->row >= 0 && move->col >= 0);
    #else
    	assert (move->row >= 0 && move->col >= 0);
	#endif
        set_square (b, move->row, move->col, P.stone);
    }

    // if this is a leaf node
    if (layer == MOVE_DEPTH) {
        // calculate score after best opponent move
        score_to_return = board_position_weight(b, P, O) + board_count_score(b, P, O);
        debug_printf ("  score %d.\n", score_to_return);
    }
    // if not a leaf node
    else {
        // generate the MOVE_BREADTH best moves from this position
        COORD moves[MOVE_BREADTH];
        int n_moves = get_best_move_n (b, O, P, moves);
        debug_printf ("  generated %d moves.\n", n_moves);

        int best_i = -1;
        
        // visit each child using 1 of the MOVE_BREADTH moves, invert O and P
        if (layer % 2 == 0) {   
            // curr layer is a max layer
            score_to_return = -MAX_SCORE;
            for (i = 0; i < n_moves; ++i) {
                int score = tree_search (layer+1, b, O, P, moves+i, score_to_return);
                if (score > score_to_return || ((score==score_to_return) && (rand()%2 == 1)) || best_i == -1) {
                    score_to_return = score;
                    best_i = i;
                }
                #ifdef PRUNING
                // previous layer is min layer. Stop computation when curr layer score
                // is greated than prev layer score
                if (score_to_return > prev_layer_score)
                    break;
                #endif
            }
        }
        else {                  
            // curr layer is min layer
            score_to_return = MAX_SCORE;
            for (i = 0; i < n_moves; ++i) {
                int score = tree_search (layer+1, b, O, P, moves+i, score_to_return);
                if (score < score_to_return || ((score==score_to_return) && (rand()%2 == 1)) || best_i == -1) {
                    score_to_return = score;
                    best_i = i;
                }
                #ifdef PRUNING
                if (layer != 0 && score_to_return < prev_layer_score)
                    break;
                #endif
            }
        }

        if (layer == 0)
            *move = moves[best_i];
        
        for (i = 0; i < layer; i++) debug_printf ("  ");
        debug_printf ("final score P%d (%2d,%2d) -> %d.\n", P.stone, move->row, move->col, score_to_return);
    }
    
    // unplay the move
    if (layer != 0) set_square (b, move->row, move->col, STONE_NONE);
    return score_to_return;
}

int get_move_ai2 (BOARD b, PLAYER P, PLAYER O, int turn, COORD* move) {
    if (turn == 1) {
        move->row = BOARD_ROWS/2;
        move->col = BOARD_COLS/2;
    }
    else {
        // starting at the top level (layer 0) node, process the entire tree
        tree_search (0, b, O, P, move, 0);
    }
    return 1;
}

#ifndef MICROBLAZE
void print_ai (FILE* fp, AI_PLAYER ai) {
	fprintf (fp, "C4: %15d, %15s\nC3: %15d, %15d\nC2: %15d, %15d\n", 
		ai.CP4, "X", ai.CP3, ai.CO3, ai.CP2, ai.CO2);
}
#endif

