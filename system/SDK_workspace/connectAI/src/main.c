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

#define printf xil_printf

#define BLACK 0x0000
#define RED 0xF800
#define GREEN 0x07D0
#define BLUE 0x001F
#define YELLOW 0xFF80
#define WHITE 0xFFFF
#define ORANGE 0xFCC0
#define DARK_PURPLE 0xA81F
#define PINK 0xFADF
#define LIGHT_BLUE 0x077F
#define MAGENTA 0xF81F
#define CYAN 0x07FF
#define BROWN 0x82E5

void PrintTouchCoordinates(TouchSensor* InstPtr);
void PrintCircleId(TouchSensor* touchSensor, button_t * Button);
void PrintRectId(TouchSensor* touchSensor, button_t * Button);
void blackScreen();

XIntc InterruptController;
TouchSensor touchSensor;
ButtonManager_t Manager;
XStatus Status;

int main()
{

	/********************** TouchSensor Setup *********************/
	Status = TouchSensor_Initialize(&touchSensor, XPAR_TOUCHSENSOR_0_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		printf("TouchSensor initialisation error\n\r\r");
	}
    TouchSensorButtons_InitializeManager(&Manager, &touchSensor, &PrintTouchCoordinates);

    button_t MyCircle;
    Button_SetCircleDim(&MyCircle, 10, 100, 100);
    Button_AssignHandler(&MyCircle, &PrintCircleId);
    TouchSensorButtons_RegisterButton(&Manager, &MyCircle);
    TouchSensorButtons_EnableButton(&MyCircle);

    button_t MyRect;
    Button_SetRectDim(&MyRect, 20, 20, 300, 20);
    Button_AssignHandler(&MyRect, &PrintRectId);
    TouchSensorButtons_RegisterButton(&Manager, &MyRect);
    TouchSensorButtons_EnableButton(&MyRect);

    //Example of how to disable button below. See header
    //file for details
    // TouchSensorButtons_DisableButton(&MyRect);

    //Example of how to remove button from the button manager. See
    //header file for details
    //TouchSensorButtons_RemoveButton(&Manager, $MyRect);

    /********************** TFT Setup *********************/
    blackScreen();

    TFT tft;
	TFT_Initialize(&tft, XPAR_TFT_PERHIPHERAL_0_DEVICE_ID);
	TFT_SetImageAddress(&tft, XPAR_MCB_DDR2_S0_AXI_BASEADDR);
	TFT_SetBrightness(&tft, 7);
	TFT_TurnOn(&tft);


	//Render Buttons
	TouchSensorButtons_RenderButton(&MyCircle, GREEN, &tft);
	TouchSensorButtons_RenderButton(&MyRect, RED, &tft);

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

