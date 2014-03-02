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

#define printf xil_printf

void PrintTouchCoordinates(TouchSensor* InstPtr);
void HandleGameboardTouch(TouchSensor* touchSensor, button_t * Button);
void blackScreen();

XIntc InterruptController;
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
		printf("TouchSensor initialisation error\n\r\r");
	}
    TouchSensorButtons_InitializeManager(&Manager, &touchSensor, &PrintTouchCoordinates);

    button_t GameboardGridButton;
    Button_SetGridDim(&GameboardGridButton, SQUARE_DIM, SQUARE_DIM, BOARD_OFFSET_X, BOARD_OFFSET_Y, BOARD_SIZE);
    Button_AssignHandler(&GameboardGridButton, &HandleGameboardTouch);
    TouchSensorButtons_RegisterButton(&Manager, &GameboardGridButton);
    TouchSensorButtons_EnableButton(&GameboardGridButton);

    //Example of how to disable button below. See header
    //file for details
    // TouchSensorButtons_DisableButton(&MyRect);

    //Example of how to remove button from the button manager. See
    //header file for details
    //TouchSensorButtons_RemoveButton(&Manager, $MyRect);

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


    Gameboard_Initialize(&Gameboard);
    Gameboard.TftPtr = &tft;


    /********************** Interrupt Controller Setup *********************/
       /*
        * Initialize the interrupt controller driver so that it's ready to use,
        * using the device ID that is generated in xparameters.h
        */
       Status = XIntc_Initialize(&InterruptController, XPAR_AXI_INTC_0_DEVICE_ID);
       if (Status != XST_SUCCESS)
       {
           printf("Interrupt controller initialization error\n\r");
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
           printf("Interrupt controller connect error\n\r");
       }

       Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
       if (Status != XST_SUCCESS)
       {
           printf("Interrupt controller start error\n\r");
       }

       XIntc_Enable(&InterruptController,
    		   XPAR_AXI_INTC_0_TOUCHSENSOR_0_INTERRUPT_INTR);
       microblaze_enable_interrupts();

	while (1) {

		int CurrentMove = Gameboard.MoveBufferSize;
		player_mode_t CurrentPlayerMode = (CurrentMove % 2) ? Gameboard.WhiteMode : Gameboard.BlackMode;

		switch (CurrentPlayerMode){
			case human:
			    TouchSensorButtons_EnableButton(&GameboardGridButton);

				break;
			case fpga:
				TouchSensorButtons_DisableButton(&GameboardGridButton);

				break;
			case uart:
				TouchSensorButtons_DisableButton(&GameboardGridButton);

				break;
			default:
				TouchSensorButtons_EnableButton(&GameboardGridButton);
				break;
			}

		while(CurrentMove == Gameboard.MoveBufferSize);
	}

	return 0;
}
//Default handler for touches
void PrintTouchCoordinates(TouchSensor* InstPtr){
	printf("Default Handler\n\r");
	printf("X, Y: %d, %d\n\r", InstPtr->LastTouch.x, InstPtr->LastTouch.y);
}


void HandleGameboardTouch(TouchSensor* touchSensor, button_t * Button){
	u16 GridX, GridY;
	if (Button_GetGridLoc(Button, touchSensor->LastTouch.x, touchSensor->LastTouch.y,
			&GridX, &GridY)) {
			Gameboard_PlayMove(&Gameboard,GridX,GridY);
	} else {
		printf("Error: Could not look up valid grid square from touch coordinates");
	}

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

