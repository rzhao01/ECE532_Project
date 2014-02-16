#ifndef __COMMON_H___
#define __COMMON_H___

#define GRAPHICS

#define BOARD_ROWS 15
#define BOARD_COLS 15
#define BITS_PER_ELEM 8
#define BOARD_MEM_SIZE 225 // number of elements needed to store the bitboard

#define EMPTY 0
#define P1 1
#define P2 2
#define HUMAN 1
#define AI 2

typedef char BOARD[BOARD_MEM_SIZE];
typedef struct {
	int num, life;
} PLAYER;

#define SQUARE_SIZE  32
#define STATUSBAR_SIZE 15
#define EXTRA_WIDTH 200

#define GRAY 0xffffff50
#define WHITE 0xffffffff
#define RED 0xff0000ff
#define BLACK 0x00000000
#define YELLOW 0xffff00ff
#define PURPLE 0xff00ffff
#define BLUE 0x00ffffff
#define GREEN 0x00ff00ff

#endif
