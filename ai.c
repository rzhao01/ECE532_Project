#include "ai.h"
#include "board.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define NABS(x) ((x) > 0 ? (-(x)) : (x))
#define PARITY(x) ((x) % 2 == 0 ? (-1) : (1))

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
inline long board_position_weight (char row, char col) {
    return BOARD_ROWS/2 + NABS(row - BOARD_ROWS/2) + BOARD_COLS/2 + NABS(col - BOARD_COLS/2);
}

long board_count_score (AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O) {
    int p = P.num;
    int o = O.num;
    long score;

    COUNTS C = generate_board_counts (b);
/*
    printf ("Board Evaluation:\n");
    printf ("\tp5=%3d, o5=%3d\n\tp4=%3d, o4=%3d\n\tp3=%3d, o3=%3d\n\tp2=%3d, o2=%3d\n", 
                    C.p5[p],0, C.p4[p],C.p4[o], C.p3[p],C.p3[o], C.p2[p],C.p2[o]);
*/
    
    score = ai.CP4*C.p4[p] - ai.CO3*C.p3[o] + ai.CP3*C.p3[p] - ai.CO2*C.p2[o] + ai.CP2*C.p2[p];
    if (C.p5[p] > 0)
        score += 1e10;
    else if (C.p4[o] > 0)
        score += -1e9;
    else if (C.p4[p] > 1)
        score += 1e9;
    
    return score;
}

// get the best move
long get_best_move (AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O, COORD* move) {
    BOARD bb;
    int best_row=0, best_col=0;
    long best_score = -1e15;
    // try every move and maximize the board score
    for (int r = 0; r < BOARD_ROWS; ++r) {
        for (int c = 0; c < BOARD_COLS; ++c) {
            if (get_square (b, r, c)) // occupied
                continue;

            copy_board (bb, b);
            set_square (bb, r, c, (int)P.num+1);

            long score = board_position_weight(r, c) + board_count_score(ai, bb, P, O);
            
            //printf ("Score (%2d,%2d) -> %ld\n", r, c, score);

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
void get_best_move_n (AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O, COORD moves[MOVE_BREADTH]) {
    BOARD bb;
    long best_score[MOVE_BREADTH];
    for (int i = 0; i < MOVE_BREADTH; ++i)
        best_score[i] = -1e15;

    // try every move and maximize the board score
    for (int r = 0; r < BOARD_ROWS; ++r) {
        for (int c = 0; c < BOARD_COLS; ++c) {
            if (get_square (b, r, c)) // occupied
                continue;

            copy_board (bb, b);
            set_square (bb, r, c, (int)P.num+1);

            long score = board_position_weight(r, c) + board_count_score(ai, bb, P, O);
            
            //printf ("Score (%2d,%2d) -> %ld\n", r, c, score);
            for (int i = 0; i < MOVE_BREADTH; ++i) {
                if (score > best_score[i] || ((score==best_score[i]) && (rand()%2 == 1))) {
                    // shift every score down
                    for (int j = i; j < MOVE_BREADTH-1; ++j) {
                        best_score[j+1] = best_score[j];
                        moves[j+1] = moves[j];
                    }

                    best_score[i] = score;
                    moves[i].row = r;
                    moves[i].col = c;
                }
            }
        }
    }
}

int get_move_ai1 (AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O, COORD* move) {
	get_best_move (ai, b, P, O, move);
	return 1;
}

void traverse_tree(int index, int layer, AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O, COORD move) {
    // play the move passed to us except for the root node
    if (index != 0) set_square (b, move.row, move.col, P.stone);
    TREE_NODE* curr_node = tree_node_array + index;
    
    // if this is a leaf node
    if (layer == MOVE_DEPTH) {
        // calculate score after best opponent move
        COORD temp;
        curr_node->score = get_best_move (ai, b, O, P, &temp);
        curr_node->move = temp;
        printf ("leaf (%d), index %d. move (%d,%d) score %ld.\n", 
            layer, index, curr_node->move.row, curr_node->move.col, curr_node->score);
    }
    // if not a leaf node
    else {
        // generate the MOVE_BREADTH best moves from this position
        COORD moves[MOVE_BREADTH];
        get_best_move_n (ai, b, O, P, moves);

        // visit each child using 1 of the MOVE_BREADTH moves, invert O and P
        int i;
        for (i = 0; i < MOVE_BREADTH; ++i) {
            traverse_tree (index*MOVE_BREADTH+i+1, layer+1, ai, b, O, P, moves[i]);
        }

        if (layer % 2 == 0) {// player takes next action
            // maximize score
            curr_node->score = -1e15;
            for (i = 0; i < MOVE_BREADTH; ++i) {
                TREE_NODE* t = tree_node_array + index*MOVE_BREADTH+i+1;
                if (t->score > curr_node->score) {
                    curr_node->score = t->score;
                    curr_node->move = t->move;
                }
            }
        }
        else {
            // minimize score
            curr_node->score = 1e15;
            for (i = 0; i < MOVE_BREADTH; ++i) {
                TREE_NODE* t = tree_node_array + index*MOVE_BREADTH+i+1;
                if (t->score < curr_node->score) {
                    curr_node->score = t->score;
                    curr_node->move = t->move;
                }
            }  
        }

        printf ("internal (%d), index %d. move (%d,%d) score %ld.\n", 
            layer, index, curr_node->move.row, curr_node->move.col, curr_node->score);
    }
    
    // unplay the move
    if (index != 0) set_square (b, move.row, move.col, STONE_NONE);
}

int get_move_ai2 (AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O, COORD* move) {
    // starting at the top level (layer 0) node, process the entire tree
    COORD temp;
    traverse_tree (0,0, ai, b, O, P, temp);
    *move = tree_node_array[0].move;
    return 1;
}

void print_ai (FILE* fp, AI_PLAYER ai) {
	fprintf (fp, "C4: %15d, %15s\nC3: %15d, %15d\nC2: %15d, %15d\n", 
		ai.CP4, "X", ai.CP3, ai.CO3, ai.CP2, ai.CO2);
}
