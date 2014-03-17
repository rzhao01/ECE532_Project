#ifndef __AI_H___
#define __AI_H___

#include "common.h"
#include <stdio.h>

typedef struct _TREE_NODE_ {
    int score;
} TREE_NODE;

extern TREE_NODE tree_node_array[NUM_NODES];

AI_PLAYER default_ai ();

int board_position_weight (BOARD b, PLAYER P, PLAYER O);
int board_count_score (AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O);

int get_move_ai1 (AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O, COORD* move);
int get_move_ai2 (AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O, COORD* move);

void print_ai (FILE* fp, AI_PLAYER ai);
#endif
