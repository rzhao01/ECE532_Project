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
typedef int2 ELEM;
typedef ELEM BOARD[BOARD_ELEMS];
#else
typedef char ELEM;
typedef ELEM BOARD[BOARD_ELEMS];
#endif

//DELETEtypedef enum {HUMAN,AI} PLAYER_TYPE;
typedef enum {P1,P2} PLAYER_NUMBER;

typedef struct {
	int p2[2], p3[2], p4[2], p5[2];
} COUNTS;

typedef struct {
	PLAYER_NUMBER num;
	//DELETEPLAYER_TYPE type;
} PLAYER;

typedef struct {
    int CP5, CO4, CP4, CO3, CP3, CO2, CP2;
} AI_PLAYER;

#define SQUARE_SIZE  32
#define STATUSBAR_SIZE 15
#define EXTRA_WIDTH 200
/* DELETE
#define GRAY 0xffffff50
#define WHITE 0xffffffff
#define RED 0xff0000ff
#define BLACK 0x00000000
#define YELLOW 0xffff00ff
#define PURPLE 0xff00ffff
#define BLUE 0x00ffffff
#define GREEN 0x00ff00ff
*/
#endif
