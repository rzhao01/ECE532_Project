/*
 * gameboard.c
 *
 *  Created on: Feb 25, 2014
 *      Author: DavidDev997516512
 */

/***************************** Include Files *********************************/

#include "gameboard.h"

/*****************************************************************************/
/*
*
*	Initializes the gameboard state.
*	Black and white positions are set to 0;
*
* @param 	Gameboard, the gameboard struct to be intialized ;
*****************************************************************************/


void Gameboard_Initialize(gameboard_t * Gameboard) {

}

/*****************************************************************************/
/*
*	Sets a square in the gameboard to a particular state, over writing
*	what was there previously.
*
* @param 	Gameboard, the gameboard struct to be intialized ;
* @param 	X, the X coordinate of the square
* @param 	Y, the Y coordinate of the square
* @param    State, the state teh square should be set to.
*
*****************************************************************************/
void Gameboard_SetSquare(gameboard_t * Gameboard, u32 X, u32 Y, square_state_t State)
{
	Xil_AssertVoid(Gameboard != NULL);
	Xil_AssertVoid(X < BOARD_SIZE);
	Xil_AssertVoid(Y < BOARD_SIZE);

	switch (State) {
		case SQUARESTATE_WHITE:
			Gameboard->WhitePositions[Y] = Gameboard->WhitePositions[Y]|(1<<X);
			Gameboard->BlackPositions[Y] = Gameboard->WhitePositions[Y]& ~(1<<X);
			break;
		case SQUARESTATE_BLACK:
			Gameboard->WhitePositions[Y] = Gameboard->WhitePositions[Y] & ~(1<<X);
			Gameboard->BlackPositions[Y] = Gameboard->WhitePositions[Y] | (1<<X);
			break;
		case SQUARESTATE_UNPLAYED:
			Gameboard->WhitePositions[Y] = Gameboard->WhitePositions[Y] & ~(1<<X);
			Gameboard->BlackPositions[Y] = Gameboard->WhitePositions[Y] & ~(1<<X);
			break;
		default:
			break;
		}
}

/*****************************************************************************/
/*
*	Returns the state enum of the queried square
*
* @param 	Gameboard, the gameboard struct to be intialized ;
* @param 	X, the X coordinate of the square
* @param 	Y, the Y coordinate of the square
*
* @returns   the state of the square (WHITE, BLACK, UNPLAYED)
*****************************************************************************/
square_state_t Gameboard_GetSquare(gameboard_t * Gameboard, u32 X, u32 Y)
{
	Xil_AssertNonvoid(Gameboard != NULL);
	Xil_AssertNonvoid(X < BOARD_SIZE);
	Xil_AssertNonvoid(Y < BOARD_SIZE);

	int black = Gameboard->BlackPositions[Y]&(1<<X);
	int white = Gameboard->WhitePositions[Y]&(1<<X);

	Xil_AssertNonvoid (!black || !white);
	if (white)
		return SQUARESTATE_WHITE;
	else if(black)
		return SQUARESTATE_BLACK;
	else
		return SQUARESTATE_UNPLAYED;
}

/*****************************************************************************/
/*
*	Renders a square based on its current state.
*
*
*****************************************************************************/
void Gameboard_RenderSquare(gameboard_t * Gameboard, u32 X, u32 Y, TFT * TftPtr)
{
	Xil_AssertVoid(Gameboard != NULL);
	Xil_AssertVoid(TftPtr != NULL);
	Xil_AssertVoid(X < BOARD_SIZE);
	Xil_AssertVoid(Y < BOARD_SIZE);

	square_state_t square_state = Gameboard_GetSquare(Gameboard,X,Y);

	u16 CentreX = (X * SQUARE_DIM) + SQUARE_DIM/2 + BOARD_OFFSET_X;
	u16 CentreY = (Y * SQUARE_DIM) + SQUARE_DIM/2 + BOARD_OFFSET_Y;

	if (square_state == SQUARESTATE_WHITE)
		Graphics_RenderCircle(CentreX,CentreY,PIECE_RADIUS,WHITE,TftPtr);
	else if(square_state == SQUARESTATE_BLACK)
		Graphics_RenderCircle(CentreX,CentreY,PIECE_RADIUS,RED,TftPtr);
	else
		Graphics_RenderCircle(CentreX,CentreY,PIECE_RADIUS,BLACK,TftPtr);


}




