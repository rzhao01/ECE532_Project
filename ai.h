#ifndef __AI_H___
#define __AI_H___

#include "common.h"
#include <stdio.h>

typedef struct _TREE_NODE_ {
    long score;
    int layer;
    int x, y;
    int child_index[MOVE_BREADTH];
} TREE_NODE;

TREE_NODE node_array[NUM_NODES];

AI_PLAYER default_ai ();
long board_position_weight (int row, int col);
long board_count_score (AI_PLAYER ai, BOARD b, int p, int o);
int get_move_ai1 (AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O, char *row, char *col);

void print_ai (FILE* fp, AI_PLAYER ai);
#endif
