#ifndef __AI_H___
#define __AI_H___

#include "common.h"

AI_PLAYER default_ai ();
long board_position_weight (int row, int col);
long board_count_score (AI_PLAYER ai, BOARD_SET S, int p, int o);
int get_move_ai1 (AI_PLAYER ai, BOARD_SET S, int p, int o, char *row, char *col);

#endif
