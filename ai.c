#include "ai.h"
#include "board.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

#include "graphics.h"

#define DEBUG
#ifdef DEBUG
    #define debug_printf(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define debug_printf(format, ...)
#endif

#define NABS(x) ((x) > 0 ? (-(x)) : (x))
#define PARITY(x) ((x) % 2 == 0 ? (-1) : (1))

#define MAX_SCORE 1000000000
#define C_P4 1024*8
#define C_O3 1024*8
#define C_P3 1024*3
#define C_O2 8
#define C_P2 8

TREE_NODE tree_node_array[NUM_NODES];

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
    for (int r = 0; r < BOARD_ROWS; ++r) {
        for (int c = 0; c < BOARD_COLS; ++c) {
            ELEM elem = get_square (b, r, c);
            if (elem == P.stone) // occupied
                score += BOARD_ROWS + NABS(r - BOARD_ROWS/2) + NABS(c - BOARD_COLS/2);
            else if (elem == O.stone)
                score -= BOARD_ROWS + NABS(r - BOARD_ROWS/2) + NABS(c - BOARD_COLS/2);
        }
    }
    return score; 
}

int board_count_score (AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O) {
    int p = P.num;
    int o = O.num;
    int score;

    COUNTS C = generate_board_counts (b);
/*
    printf ("Board Evaluation:\n");
    printf ("\tp5=%3d, o5=%3d\n\tp4=%3d, o4=%3d\n\tp3=%3d, o3=%3d\n\tp2=%3d, o2=%3d\n", 
                    C.p5[p],0, C.p4[p],C.p4[o], C.p3[p],C.p3[o], C.p2[p],C.p2[o]);
*/
    
    score = ai.CP4*C.p4[p] - ai.CO3*C.p3[o] + ai.CP3*C.p3[p] - ai.CO2*C.p2[o] + ai.CP2*C.p2[p];
    if (C.p5[p] > 0)
        score = MAX_SCORE;          // win
    else if (C.p4[o] > 0)
        score = -(MAX_SCORE-2);     // lose
    else if (C.p4[p] > 1)
        score = MAX_SCORE-3;        // win in 1
    
    return score;
}

// get the best move
int get_best_move (AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O, COORD* move) {
    BOARD bb;
    int best_row=0, best_col=0;
    int best_score = -MAX_SCORE;
    // try every move and maximize the board score
    for (int r = 0; r < BOARD_ROWS; ++r) {
        for (int c = 0; c < BOARD_COLS; ++c) {
            if (get_square (b, r, c)) // occupied
                continue;

            copy_board (bb, b);
            set_square (bb, r, c, (int)P.num+1);

            int score = board_position_weight(b, P, O) + board_count_score(ai, bb, P, O);
            
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
int get_best_move_n (AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O, COORD moves[MOVE_BREADTH]) {
    BOARD bb;
    int best_score[MOVE_BREADTH];
    for (int i = 0; i < MOVE_BREADTH; ++i) {
        moves[i].row = -1;
        best_score[i] = -MAX_SCORE;
    }

    int n_moves = 0;
    
    // try every move and maximize the board score
    for (int r = 0; r < BOARD_ROWS; ++r) {
        for (int c = 0; c < BOARD_COLS; ++c) {
            if (get_square (b, r, c)) // occupied
                continue;

            copy_board (bb, b);
            set_square (bb, r, c, (int)P.num+1);

            int score = board_position_weight(bb, P, O) + board_count_score(ai, bb, P, O);

            //printf ("Score (%2d,%2d) -> %d\n", r, c, score);
            for (int i = 0; i < MOVE_BREADTH; ++i) {
                if (score > best_score[i] || ((score==best_score[i]) && (rand()%2 == 1))) {
                    // shift every score down
                    for (int j = MOVE_BREADTH-2; j >= i; --j) {
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

int get_move_ai1 (AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O, COORD* move) {
	get_best_move (ai, b, P, O, move);
	return 1;
}

void traverse_tree(int index, int layer, AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O, COORD* move) {
    for (int i = 0; i < layer; i++) debug_printf ("  ");
    debug_printf ("layer %d, ind %d, P%d. move(%2d,%2d) ", layer, index, P.stone, move->row, move->col); 

    // play the move passed to us except for the root node
    if (index != 0) {
        assert (move->row >= 0 && move->col >= 0);
        set_square (b, move->row, move->col, P.stone);
    }
    TREE_NODE* curr_node = tree_node_array + index;
/*
    SDL_Surface* screen;
    initDisplay (&screen, "debug");
    
    drawBoard (screen, b);
    waitEvent (screen);
*/
    // if this is a leaf node
    if (layer == MOVE_DEPTH) {
        // calculate score after best opponent move
        //COORD temp;
        curr_node->score = board_position_weight(b, P, O) + board_count_score(ai, b, P, O);
        debug_printf ("  score %d.\n", curr_node->score);
    }
    // if not a leaf node
    else {
        // generate the MOVE_BREADTH best moves from this position
        COORD moves[MOVE_BREADTH];
        int n_moves = get_best_move_n (ai, b, O, P, moves);
        
        if (layer < 2) {
            debug_printf ("  generated %d moves.\n", n_moves);
        }

        // visit each child using 1 of the MOVE_BREADTH moves, invert O and P
        int i;
        for (i = 0; i < n_moves; ++i) {
            traverse_tree (index*MOVE_BREADTH+i+1, layer+1, ai, b, O, P, moves+i);
        }
        
        // maximize score
        int best_i = 0;
        curr_node->score = -MAX_SCORE;
        for (i = 0; i < n_moves; ++i) {
            TREE_NODE* t = tree_node_array + index*MOVE_BREADTH+i+1;
            if (t->score > curr_node->score) {
                best_i = i;
                curr_node->score = t->score;
            }
        }

        if (index == 0)
            *move = moves[best_i];
        else if (layer % 2 == 1) // opp takes next action
            curr_node->score = -curr_node->score;

        if (layer < 2) {
            for (int i = 0; i < layer; i++) debug_printf ("  ");
            debug_printf ("internal (%d), index %d, P%d. move (%d,%d) final score %d.\n", 
                layer, index, P.stone, move->row, move->col, curr_node->score);
        }
    }
    
    // unplay the move
    if (index != 0) set_square (b, move->row, move->col, STONE_NONE);
}

int get_move_ai2 (AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O, COORD* move) {
    // starting at the top level (layer 0) node, process the entire tree
    traverse_tree (0,0, ai, b, O, P, move);
    return 1;
}

void print_ai (FILE* fp, AI_PLAYER ai) {
	fprintf (fp, "C4: %15d, %15s\nC3: %15d, %15d\nC2: %15d, %15d\n", 
		ai.CP4, "X", ai.CP3, ai.CO3, ai.CP2, ai.CO2);
}
