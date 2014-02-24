/*
 * touchsensor_button.h
 *
 *  Created on: Feb 8, 2014
 *      Author: DavidDev997516512
 */

#ifndef TOUCHSENSOR_BUTTON_H
#define TOUCHSENSOR_BUTTON_H
#ifdef __cplusplus
extern "C" {
#endif


/***************************** Include Files *********************************/

#include "touchsensor.h"
#include "xbasic_types.h"
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "tft.h"

//sin array will have 90/SIN_DIV elements to capture first quarater of sin
#define SIN_DIV 15

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/

/**
 * Signature for the callback function.
 *
 * @param	CallBackRef is a callback reference passed in by the upper layer
 *		when setting the callback functions, and passed back to the
 *		upper layer when the callback is invoked. Its type is
 *		 unimportant to the driver, so it is a void pointer.
 */
typedef void (*TouchSensorButtons_Handler) (void *CallBackRef_Button, void *CallBackRef_TouchSensor);

typedef enum {RECT, CIRCLE} button_shape_t;

typedef struct button_t {
	button_shape_t shape;

	u16 PosX, PosY; //Used to define location.
	u16 DimA, DimB; //Used to define size Dim_A = radius in case of circle;

	int Enabled; //Used to determine if button is enabled
	TouchSensorButtons_Handler Handler; // Function pointer to the button specific handler.

	//It probably would have been more savory to do this in C++...
	struct button_t * Next;
	struct button_t * Prev;

}button_t;

typedef struct ButtonManager_t{
	button_t * ButtonList;
	TouchSensor * Sensor;
	TouchSensor_Handler Handler;
} ButtonManager_t;




/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/



int TouchSensorButtons_InitializeManager(ButtonManager_t * Manager,TouchSensor * touchSensor, void * FuncPtr);

void TouchSensorButtons_RegisterButton(ButtonManager_t * Manager, button_t * Button);
void TouchSensorButtons_RemoveButton(ButtonManager_t * Manager, button_t * Button);

// Enable a wireframe outline of a registered button.
void TouchSensorButtons_RenderButton(button_t * Button, u16 colour, TFT *TftPtr);
void Button_RenderCircle(button_t * Button, u16 colour, TFT *TftPtr);
void Button_RenderRect(button_t * Button, u16 colour, TFT *TftPtr);

// Ignore or enable presses on the registered button
void TouchSensorButtons_EnableButton(button_t * Button);
void TouchSensorButtons_DisableButton(button_t * Button);

// Handler first called on an interrupt. Used to detect which
// button was pressed
void TouchSensorButtons_InterruptHandler(void * InstancePtr);

//Button list managing functions;
void ButtonList_Prepend(ButtonManager_t * Manager, button_t * Button);
void ButtonList_Remove(ButtonManager_t * Manager, button_t * Button);
button_t * ButtonList_Find(ButtonManager_t * Manager, u16 X, u16 Y);

//Button functions
int Button_CheckCircle(button_t * Button, u16 X, u16 Y);
int Button_CheckRect(button_t * Button, u16 X, u16 Y);


void Button_SetCircleDim(button_t * Button, u16 radius, u16 X, u16 Y);
void Button_SetRectDim(button_t * Button,  u16 LenX, u16 LenY, u16 X, u16 Y);
void Button_AssignHandler(button_t * Button, void * FuncPtr);

//sin and cos lookup for rendering circle
double sin_lookup(double i);
double cos_lookup(double i);
void render_line(double x1, double y1, double x2, double y2, u16 colour, TFT *TftPtr);
void draw_line(double x1, double y1, double x2, double y2, double m, u16 colour, TFT *TftPtr);

/************************** Variable Definitions *****************************/


/*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
