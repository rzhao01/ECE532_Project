/*
 * gameboard.h
 *
 *  Created on: Feb 24, 2014
 *      Author: DavidDev997516512
 */

#ifndef GAMEBOARD_H_
#define GAMEBOARD_H_

/***************************** Include Files *********************************/

#include "xbasic_types.h"
#include "xil_types.h"
#include "xil_assert.h"
#include "graphics.h"

/************************** Constant Definitions *****************************/

#define BOARD_SIZE 11 // Dimension of the board in squares
//For board graphics
#define SQUARE_DIM 24 // Dimension of the square in pixels
#define BOARD_OFFSET_X 4 //Offset of the button grid
#define BOARD_OFFSET_Y 4
#define PIECE_RADIUS 8 //Radius of pieces in Pixels


/**************************** Type Definitions *******************************/

typedef struct gameboard_t {
	u32 BlackPositions [BOARD_SIZE];
	u32 WhitePositions [BOARD_SIZE];
} gameboard_t;

typedef enum square_state_t {
	SQUARESTATE_UNPLAYED,
	SQUARESTATE_WHITE,
	SQUARESTATE_BLACK
} square_state_t;




/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

void Gameboard_Intialize(gameboard_t * Gameboard);

void Gameboard_SetSquare(gameboard_t * Gameboard, u32 X, u32 Y, square_state_t State);
square_state_t Gameboard_GetSquare(gameboard_t * Gameboard, u32 X, u32 Y);

void Gameboard_RenderSquare(gameboard_t * Gameboard, u32 X, u32 Y, TFT * TftPtr);
void Gameboard_RenderBlankBoard(TFT * TftPtr);

/************************** Variable Definitions *****************************/


/*****************************************************************************/

#endif /* GAMEBOARD_H_ */
