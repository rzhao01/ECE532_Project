#ifndef __COMMON_H___
#define __COMMON_H___

#define GRAPHICS

#define BOARD_ROWS 11
#define BOARD_COLS 11
#define BOARD_ELEMS 121

#define STONE_NONE 0
#define STONE_P1 1
#define STONE_P2 2

#ifdef SYNTHESIS
#include "ap_cint.h"
typedef uint2 ELEM;
typedef uint3 COUNTER3;
typedef uint4 COUNTER4;
typedef ELEM BOARD[BOARD_ELEMS];
#else
typedef unsigned char ELEM;
typedef unsigned char COUNTER3;
typedef unsigned char COUNTER4;
typedef ELEM BOARD[BOARD_ELEMS];
#endif

typedef struct {
	int p2[2], p3[2], p4[2], p5[2];
} COUNTS;

#endif
