#ifndef __COMMON_H___
#define __COMMON_H___

#define GRAPHICS

#define BOARD_ROWS 15
#define BOARD_COLS 15

typedef int ELEM;
typedef ELEM BOARD[BOARD_ROWS];
typedef BOARD BOARD_SET[2];

#define P1 0
#define P2 1
#define HUMAN 1
#define AI 2

typedef struct {
	int p2, p3, p4, p5;
	int o2, o3, o4, o5;
} COUNTS;

typedef struct {
	int num, life;
} PLAYER;

typedef struct {
    int CP5, CO4, CP4, CO3, CP3, CO2, CP2;
} AI_PLAYER;

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
