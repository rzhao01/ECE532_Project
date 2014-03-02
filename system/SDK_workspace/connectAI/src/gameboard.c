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

	Xil_AssertVoid(Gameboard!=NULL);
	int i;
	for (i = 0; i < BOARD_SIZE; i++){
		Gameboard->BlackPositions[i] = 0;
		Gameboard->WhitePositions[i] = 0;
	}
	Gameboard->MoveBufferSize = 0;

	Gameboard->BlackMode=human;
	Gameboard->WhiteMode=human;

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
		case white:
			Gameboard->WhitePositions[Y] = Gameboard->WhitePositions[Y]|(1<<X);
			Gameboard->BlackPositions[Y] = Gameboard->BlackPositions[Y]& ~(1<<X);
			break;
		case black:
			Gameboard->WhitePositions[Y] = Gameboard->WhitePositions[Y] & ~(1<<X);
			Gameboard->BlackPositions[Y] = Gameboard->BlackPositions[Y] | (1<<X);
			break;
		case unplayed:
			Gameboard->WhitePositions[Y] = Gameboard->WhitePositions[Y] & ~(1<<X);
			Gameboard->BlackPositions[Y] = Gameboard->BlackPositions[Y] & ~(1<<X);
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

	int blackPos = Gameboard->BlackPositions[Y]&(1<<X);
	int whitePos = Gameboard->WhitePositions[Y]&(1<<X);

	Xil_AssertNonvoid (!blackPos || !whitePos);
	if (whitePos)
		return white;
	else if(blackPos)
		return black;
	else
		return unplayed;
}

/*****************************************************************************/
/*
*	Renders a square based on its current state.
*
*
*****************************************************************************/
void Gameboard_RenderSquare(gameboard_t * Gameboard, u32 X, u32 Y)
{
	Xil_AssertVoid(Gameboard != NULL);
	Xil_AssertVoid(Gameboard->TftPtr != NULL);
	Xil_AssertVoid(X < BOARD_SIZE);
	Xil_AssertVoid(Y < BOARD_SIZE);

	square_state_t square_state = Gameboard_GetSquare(Gameboard,X,Y);

	u16 CentreX = (X * SQUARE_DIM) + SQUARE_DIM/2 + BOARD_OFFSET_X;
	u16 CentreY = (Y * SQUARE_DIM) + SQUARE_DIM/2 + BOARD_OFFSET_Y;

	if (square_state == white)
		Graphics_RenderCircle(CentreX,CentreY,PIECE_RADIUS,WHITE,Gameboard->TftPtr);
	else if(square_state == black)
		Graphics_RenderCircle(CentreX,CentreY,PIECE_RADIUS,RED,Gameboard->TftPtr);
	else
		Graphics_RenderCircle(CentreX,CentreY,PIECE_RADIUS,BLACK,Gameboard->TftPtr);


}

void Gameboard_PlayMove(gameboard_t * Gameboard, u32 X, u32 Y) {

	Xil_AssertVoid(Gameboard != NULL);

	if (Gameboard_GetSquare(Gameboard,X,Y) != unplayed)
		return;

	int MoveNumber = Gameboard->MoveBufferSize;
	square_state_t NewPlay = (MoveNumber % 2 == 0) ? black : white;
	xil_printf("Legal move to %d,%d for %s\n\r", X, Y, (NewPlay == black) ? "black" : "white");
	Gameboard_SetSquare(Gameboard, X, Y, NewPlay);
	Gameboard->MoveBuffer[MoveNumber].X = X;
	Gameboard->MoveBuffer[MoveNumber].Y = Y;
	Gameboard->MoveBufferSize = (MoveNumber + 1);

	if (Gameboard->TftPtr != NULL)
		Gameboard_RenderSquare(Gameboard, X, Y);

}






