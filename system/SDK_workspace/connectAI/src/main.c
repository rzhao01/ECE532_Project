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
void PrintCircleId(TouchSensor* touchSensor, button_t * Button);
void PrintRectId(TouchSensor* touchSensor, button_t * Button);
void PrintGridId(TouchSensor* touchSensor, button_t * Button);
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

   /* button_t MyCircle;
    Button_SetCircleDim(&MyCircle, 10, 100, 100);
    Button_AssignHandler(&MyCircle, &PrintCircleId);
    TouchSensorButtons_RegisterButton(&Manager, &MyCircle);
    TouchSensorButtons_EnableButton(&MyCircle);

    button_t MyRect;
    Button_SetRectDim(&MyRect, 20, 20, 300, 20);
    Button_AssignHandler(&MyRect, &PrintRectId);
    TouchSensorButtons_RegisterButton(&Manager, &MyRect);
    TouchSensorButtons_EnableButton(&MyRect);*/

    button_t MyGrid;
    Button_SetGridDim(&MyGrid, SQUARE_DIM, SQUARE_DIM, BOARD_OFFSET_X, BOARD_OFFSET_Y, BOARD_SIZE);
    Button_AssignHandler(&MyGrid, &PrintGridId);
    TouchSensorButtons_RegisterButton(&Manager, &MyGrid);
    TouchSensorButtons_EnableButton(&MyGrid);

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
	TouchSensorButtons_RenderButton(&MyGrid, BLUE, &tft);

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

       printf("Beginning infinite loop\n\r");

	while (1) {
		//Loop infinitely waiting on touch sensor interrupts
	}

	return 0;
}
//Default handler for touches
void PrintTouchCoordinates(TouchSensor* InstPtr){
	printf("Default Handler\n\r");
	printf("X, Y: %d, %d\n\r", InstPtr->LastTouch.x, InstPtr->LastTouch.y);
}

//Example button handler for the circle button added
void PrintCircleId(TouchSensor* touchSensor, button_t * Button){
	printf("Circle Handler\n\r");
	printf("X, Y: %d, %d\n\r", touchSensor->LastTouch.x, touchSensor->LastTouch.y);

}

//Example button handler for the rectangle button added
void PrintRectId(TouchSensor* touchSensor, button_t * Button){
	printf("Rect Handler\n\r");
	printf("X, Y: %d, %d\n\r", touchSensor->LastTouch.x, touchSensor->LastTouch.y);

}

void PrintGridId(TouchSensor* touchSensor, button_t * Button){
	printf("Grid Handler\n\r");
	u16 GridX, GridY;
	if (Button_GetGridLoc(Button, touchSensor->LastTouch.x, touchSensor->LastTouch.y,
			&GridX, &GridY)) {
		printf("Grid Square (%d,%d) was depressed\n\r", GridX, GridY);
		Gameboard_SetSquare(&Gameboard,GridX,GridY,SQUARESTATE_WHITE);
		Gameboard_RenderSquare(&Gameboard,GridX,GridY,&tft);
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

