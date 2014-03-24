/*
 * main.c
 *
 *  Created on: 2013-02-25
 *
 *      Placeholder main routine taken from our example.
 *      This should be reworked when Ritchie
 *      retargets the C implementation to microblaze
 *
 */

#include <stdio.h>
#include "xparameters.h"    /* generated system parameters                  */
#include "xbasic_types.h"   /* Xilinx basic types for device drivers        */
#include "touchsensor.h"
#include "touchsensor_buttons.h"
#include "xintc.h" /* layer 1 interrupt controller device driver */
#include "tft.h"
#include "gameboard.h"
#include "xuartlite.h"
#include "../../../../board.h"

#define printf xil_printf

void PrintTouchCoordinates(TouchSensor* InstPtr);
void HandleGameboardTouch(TouchSensor* touchSensor, button_t * Button);
void HandleResetTouch(TouchSensor* touchSensor, button_t * Button);
void HandleWhiteModeTouch(TouchSensor* touchSensor, button_t * Button);
void HandleBlackModeTouch(TouchSensor* touchSensor, button_t * Button);

void RecvUartCommand(XUartLite * InstPtr);
void SendUartCommand(XUartLite * InstPtr, u32 X, u32 Y);
void HandleAiMove(AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O);
void blackScreen();

XIntc InterruptController;
XUartLite Uart;
TouchSensor touchSensor;
ButtonManager_t Manager;
gameboard_t Gameboard;
XStatus Status;
TFT tft;


int main()
{

	/********************** TouchSensor Setup *********************/
	Status = TouchSensor_Initialize(&touchSensor, XPAR_TOUCHSENSOR_0_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
	//	printf("TouchSensor initialisation error\n\r\r");
	}
    TouchSensorButtons_InitializeManager(&Manager, &touchSensor, &PrintTouchCoordinates);

    button_t GameboardGridButton;
    Button_SetGridDim(&GameboardGridButton, SQUARE_DIM, SQUARE_DIM, BOARD_OFFSET_X, BOARD_OFFSET_Y, BOARD_SIZE, BOARD_SIZE);
    Button_AssignHandler(&GameboardGridButton, &HandleGameboardTouch);
    TouchSensorButtons_RegisterButton(&Manager, &GameboardGridButton);
    TouchSensorButtons_EnableButton(&GameboardGridButton);

    button_t WhiteModeButton;
    Button_SetGridDim(&WhiteModeButton, 50, 50, 300, 10 , 3, 1);
    Button_AssignHandler(&WhiteModeButton, &HandleWhiteModeTouch);
    TouchSensorButtons_RegisterButton(&Manager, &WhiteModeButton);
    TouchSensorButtons_EnableButton(&WhiteModeButton);

    button_t BlackModeButton;
    Button_SetGridDim(&BlackModeButton, 50, 50, 300, 75, 3, 1);
    Button_AssignHandler(&BlackModeButton, &HandleBlackModeTouch);
    TouchSensorButtons_RegisterButton(&Manager, &BlackModeButton);
    TouchSensorButtons_EnableButton(&BlackModeButton);

    button_t ResetButton;
    Button_SetRectDim(&ResetButton, 100, 50, 325, 200);
    Button_AssignHandler(&ResetButton, &HandleResetTouch);
    TouchSensorButtons_RegisterButton(&Manager, &ResetButton);
    TouchSensorButtons_EnableButton(&ResetButton);

    /********************** BoardCount Accelerator Setup********************/
    initialize_accelerator(&board_count_accelerator, XPAR_GENERATE_BOARD_COUNTS_TOP_0_S_AXI_CTRL_BASEADDR);

    /********************** TFT Setup *********************/
    blackScreen();


	TFT_Initialize(&tft, XPAR_TFT_PERHIPHERAL_0_DEVICE_ID);
	TFT_SetImageAddress(&tft, XPAR_MCB_DDR2_S0_AXI_BASEADDR);
	TFT_SetBrightness(&tft, 7);
	TFT_TurnOn(&tft);


	//Render Buttons
	//TouchSensorButtons_RenderButton(&MyCircle, GREEN, &tft);
	//TouchSensorButtons_RenderButton(&MyRect, RED, &tft);
	TouchSensorButtons_RenderButton(&GameboardGridButton, BLUE, &tft);
	TouchSensorButtons_RenderButton(&ResetButton, ORANGE, &tft);
	TouchSensorButtons_RenderButton(&WhiteModeButton, WHITE, &tft);
	TouchSensorButtons_RenderButton(&BlackModeButton, RED, &tft);


    Gameboard.TftPtr = &tft;

    /********************** UART Setup *********************/

    Status = XUartLite_Initialize(&Uart, XPAR_UARTLITE_1_DEVICE_ID);
    if (Status != XST_SUCCESS)
    {
        printf("XUartLite initialization error\n\r");
    }

    XUartLite_SetRecvHandler(&Uart, (XUartLite_Handler) &RecvUartCommand, &Uart);



/********************** Interrupt Controller Setup *********************/
   /*
	* Initialize the interrupt controller driver so that it's ready to use,
	* using the device ID that is generated in xparameters.h
	*/
   Status = XIntc_Initialize(&InterruptController, XPAR_AXI_INTC_0_DEVICE_ID);
   if (Status != XST_SUCCESS)
   {
	   //printf("Interrupt controller initialization error\n\r");
   }

   /*
	* Connect the device driver handler that will be called when an interrupt
	* for the device occurs, the device driver handler performs the specific
	* interrupt processing for the device
	*/
   Status = XIntc_Connect(&InterruptController,
		   XPAR_AXI_INTC_0_TOUCHSENSOR_0_INTERRUPT_INTR,
						  (XInterruptHandler)TouchSensorButtons_InterruptHandler,
						  &Manager);
   if (Status != XST_SUCCESS)
   {
	   //printf("Interrupt controller connect error\n\r");
   }

   Status = XIntc_Connect(&InterruptController,
		   XPAR_AXI_INTC_0_RS232_UART_1_INTERRUPT_INTR,
						  (XInterruptHandler)XUartLite_InterruptHandler,
						  &Uart);
   if (Status != XST_SUCCESS)
   {
	   printf("Interrupt controller connect to Uart error\n\r");
   }


   Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
   if (Status != XST_SUCCESS)
   {
	   //printf("Interrupt controller start error\n\r");
   }

   XIntc_Enable(&InterruptController,
		   XPAR_AXI_INTC_0_TOUCHSENSOR_0_INTERRUPT_INTR);
   XIntc_Enable(&InterruptController,
		   XPAR_AXI_INTC_0_RS232_UART_1_INTERRUPT_INTR);

   XUartLite_ResetFifos(&Uart);
   XUartLite_EnableInterrupt(&Uart);

   AI_PLAYER ai = default_ai();
   PLAYER Player1, Player2;
   Player1.num = P1;
   Player2.num = P2;
   Gameboard_Initialize(&Gameboard,human,fpga);
   microblaze_enable_interrupts();

   while (1) {
		PLAYER Curr_P, Opp_P;

		//Workaround to enable multiple resets on turn 0.
		if (Gameboard.MoveBufferSize == -1)
			Gameboard.MoveBufferSize++;

		int CurrentMove = Gameboard.MoveBufferSize;
		player_mode_t CurrentPlayerMode = (CurrentMove % 2) ? Gameboard.WhiteMode : Gameboard.BlackMode;
		Curr_P = (CurrentMove % 2) ? Player1 : Player2;
		Opp_P = (CurrentMove % 2) ? Player2 : Player1;
		switch (CurrentPlayerMode){
			case human:
				TouchSensorButtons_EnableButton(&GameboardGridButton);
				break;
			case fpga:
				TouchSensorButtons_DisableButton(&GameboardGridButton);
				HandleAiMove(ai, Gameboard.master_board, Curr_P, Opp_P);
				break;
			case uart:
				TouchSensorButtons_DisableButton(&GameboardGridButton);
				if(Gameboard.MoveBufferSize > 0){
					SendUartCommand(&Uart, Gameboard.MoveBuffer[Gameboard.MoveBufferSize-1].X,
										Gameboard.MoveBuffer[Gameboard.MoveBufferSize-1].Y);
				}
				break;
			default:
				TouchSensorButtons_EnableButton(&GameboardGridButton);
				break;
		}
		/*
		if (check_board_full(Gameboard.master_board)) {
			//printf("Gameboard full\n\r");
			break;
		}
		*/
		if (check_board_win(Gameboard.master_board, Curr_P)) {
			xil_printf("Player %s won\n\r", (Curr_P.num == P1) ? "white" : "black");
			//Spin waiting on reset
			while(Gameboard.MoveBufferSize != -1);
		}

		while(CurrentMove == Gameboard.MoveBufferSize);

   }

	return 0;
}
//Default handler for touches
void PrintTouchCoordinates(TouchSensor* InstPtr){
	//printf("Default Handler\n\r");
	//printf("X, Y: %d, %d\n\r", InstPtr->LastTouch.x, InstPtr->LastTouch.y);

}

void HandleAiMove(AI_PLAYER ai, BOARD b, PLAYER P, PLAYER O){
	//TODO: Fix row/col order
	COORD move;
	get_move_ai1 (ai, b, P, O, &move);
	Gameboard_PlayMove(&Gameboard,(u32)move.col,(u32)move.row,P.num);
}

void HandleGameboardTouch(TouchSensor* touchSensor, button_t * Button){
	int CurrentMove = Gameboard.MoveBufferSize;
	PLAYER_NUMBER P = (CurrentMove % 2) ? P1 : P2;
	u16 GridX, GridY;
	if (Button_GetGridLoc(Button, touchSensor->LastTouch.x, touchSensor->LastTouch.y,
			&GridX, &GridY)) {
			Gameboard_PlayMove(&Gameboard,GridX,GridY,P);
	} else {
		//printf("Error: Could not look up valid grid square from touch coordinates");
	}

}

void HandleResetTouch(TouchSensor* touchSensor, button_t * Button){
	xil_printf("Touchsensor Reset \n\r");
	Gameboard_Initialize(&Gameboard, Gameboard.BlackMode, Gameboard.WhiteMode);
	Gameboard_RenderBoard(&Gameboard);
}

void HandleWhiteModeTouch(TouchSensor* touchSensor, button_t * Button){

	u16 GridX, GridY;
	if (Button_GetGridLoc(Button, touchSensor->LastTouch.x, touchSensor->LastTouch.y,
				&GridX, &GridY)){
		player_mode_t newMode;
		switch (GridX){
		case 0:
			newMode = human;
			xil_printf("Seting white's input mode to human\n\r");
			break;
		case 1:
			newMode = fpga;
			xil_printf("Seting white's input mode to FPGA AI\n\r");
			break;
		case 2:
			newMode = uart;
			xil_printf("Seting white's input mode to UART\n\r");
			break;
		default:
			newMode = Gameboard.WhiteMode;
			break;
		}
		Gameboard.WhiteMode = newMode;
	}
}

void HandleBlackModeTouch(TouchSensor* touchSensor, button_t * Button){

	u16 GridX, GridY;
	if (Button_GetGridLoc(Button, touchSensor->LastTouch.x, touchSensor->LastTouch.y,
				&GridX, &GridY)){
		player_mode_t newMode;
		switch (GridX){
		case 0:
			newMode = human;
			xil_printf("Setting black's input mode to human\n\r");
			break;
		case 1:
			newMode = fpga;
			xil_printf("Setting black's input mode to FPGA AI\n\r");
			break;
		case 2:
			newMode = uart;
			xil_printf("Setting black's input mode to UART\n\r");
			break;
		default:
			newMode = Gameboard.BlackMode;
			break;
		}
		Gameboard.BlackMode = newMode;
	}
}

void RecvUartCommand(XUartLite * InstPtr){

	u8 dataBuffer[3];
	XUartLite_Recv(InstPtr,&dataBuffer,3);
	printf("UART command: %c%c%c\n\r", dataBuffer[0],dataBuffer[1],dataBuffer[2]);

	if ((char)dataBuffer[0] == '0'){
		player_mode_t whiteMode = Gameboard.WhiteMode;
		player_mode_t blackMode = Gameboard.BlackMode;

		if (dataBuffer[1] == 'F'){
			blackMode = fpga;
		} else if(dataBuffer[1] == 'U'){
			blackMode = uart;
		} else if (dataBuffer[2] == 'H'){
			blackMode = human;
		}

		if (dataBuffer[2] == 'F'){
			whiteMode = fpga;
		} else if(dataBuffer[2] == 'U'){
			whiteMode = uart;
		} else if (dataBuffer[2] == 'H'){
			whiteMode = human;
		}

		Gameboard_Initialize(&Gameboard, blackMode, whiteMode);
		Gameboard_RenderBoard(&Gameboard);

	} else if ((dataBuffer[0] >= 'A' && dataBuffer[0] <= ('A' + BOARD_SIZE)) &&
				dataBuffer[2] >= 'A' && dataBuffer[2] <= ('A' + BOARD_SIZE)) {
		//If we expect a move from the uart, play it now, otherwise ignore this command.
		if (((Gameboard.MoveBufferSize % 2) ? Gameboard.WhiteMode : Gameboard.BlackMode) == uart) {
			Gameboard_PlayMove(&Gameboard,dataBuffer[0] - 'A' , dataBuffer[2] - 'A', (Gameboard.MoveBufferSize % 2) ? P1 : P2);
		}
	}



}

void SendUartCommand(XUartLite * InstPtr, u32 X, u32 Y){
	u8 dataBuffer[4];
	dataBuffer[0] = X + 'A';
	dataBuffer[1] = ',';
	dataBuffer[2] = Y + 'A';
	dataBuffer[3] = '\n';
	printf("%c%c%c\n\r", dataBuffer[0],dataBuffer[1],dataBuffer[2]);
}
//Inialize the screen to red
void blackScreen() {
	int i = 0;
	    int j = 0;
	    for (i = 0; i < 272; i ++) {
			for (j = 0; j < 480; j ++) {
				int x, y;
				x = j;
				y = i;
				TFT_WriteToPixel(x, y, BLACK, XPAR_MCB_DDR2_S0_AXI_BASEADDR);
			}
		}
}

