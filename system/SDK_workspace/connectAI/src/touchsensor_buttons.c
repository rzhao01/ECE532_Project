/*
 * touchsensor_button.c
 *
 *  Created on: Feb 8, 2014
 *      Author: DavidDev997516512
 */

/***************************** Include Files *********************************/

#include "touchsensor.h"
#include "touchsensor_buttons.h"
#include "xil_assert.h"

double sin_a[360/SIN_DIV] = {0, 0.258819045, 0.5, 0.707106781, 0.866025404, 0.965925826, 1};

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/


/************************** Variable Definitions *****************************/


/*****************************************************************************/
/**
*
* Initializes the button manager. Sets the default handler, and hooks in the
* appropriate TouchSensor Instance
*
* @param 	Manager is the manager instance we are going to initialize;
* @param	Inst is the TouchSensor we are linking to the manager
* @param	FuncPtr points to the default handler the button manager will
* 			invoke if the press does not land on a button
* @param	CallBackRef is the upper layer callback reference passed back
* 			when the touchscreen is touched.
*

******************************************************************************/

int TouchSensorButtons_InitializeManager(ButtonManager_t * Manager, TouchSensor * touchSensor, void * FuncPtr)
{
	Xil_AssertNonvoid(FuncPtr != NULL);
	Xil_AssertNonvoid(touchSensor != NULL);

	Manager->Handler = FuncPtr;
	Manager->Sensor = touchSensor;
	Manager->ButtonList = NULL;

	TouchSensor_EnableInterrupts(touchSensor);
    TouchSensor_EnableDebounce(touchSensor);
    TouchSensor_EnableDebouncing(touchSensor, 0x8FFFF);

    return(XST_SUCCESS);

}

/*****************************************************************************/
/**
*
* Initializes the button manager. Sets the default handler, and hooks in the
* appropriate TouchSensor Instance
*
* @param 	Manager is the manager instance we are going to initialize;
* @param	Inst is the TouchSensor we are linking to the manager
* @param	FuncPtr points to the default handler the button manager will
* 			invoke if the press does not land on a button
* @param	CallBackRef is the upper layer callback reference passed back
* 			when the touchscreen is touched.
*

******************************************************************************/

void TouchSensorButtons_InterruptHandler(void * InstancePtr)
{


		ButtonManager_t * Manager = NULL;

		/*
		 * Verify that each of the inputs are valid.
		 */
		Xil_AssertVoid( InstancePtr != NULL);

		/*
		 * Convert the non-typed pointer to an touchsensor instance pointer
		 * such that there is access to the touchsensor
		 */
		Manager = (ButtonManager_t*) InstancePtr;
		Xil_AssertVoid(Manager->Sensor != NULL);
		TouchSensor * TouchSensorPtr= Manager->Sensor;

		//Check interrupts are enabled.
		Xil_AssertVoid(TouchSensorPtr->InterruptsEnabled == 1);

		// If debouncing is enabled on the first touch we ignore the button press,
		// set the the TS counter, and return to previous execution state.
		if (TouchSensorPtr->DebouncePeriod != 0 && TouchSensorPtr->InitialDetection == 0) {

			TouchSensorPtr->InitialDetection = 1;
			TouchSensor_SetTimerReg(TouchSensorPtr->BaseAddress, TouchSensorPtr->DebouncePeriod);
			return;

		}

		TouchSensorPtr->InitialDetection = 0;

		//Reject spurious detections; check if TouchEn is high.

		int TouchData = TouchSensor_GetPosReg(TouchSensorPtr->BaseAddress);
		if (TouchSensor_GetTouchEn(TouchData)) {
			TouchSensorPtr->LastTouch.x = TouchSensor_GetX(TouchData);
			TouchSensorPtr->LastTouch.y = TouchSensor_GetY(TouchData);
			button_t * FoundButton = ButtonList_Find(Manager,TouchSensorPtr->LastTouch.x, TouchSensorPtr->LastTouch.y);

			if (FoundButton != NULL) {
				Xil_AssertVoid(FoundButton->Handler != NULL);
				FoundButton->Handler(TouchSensorPtr, FoundButton);
			} else {
				//Default Manager
				Manager->Handler(TouchSensorPtr);
			}
		}
}

/*****************************************************************************/
/**
*
*  Checks to see if a press falls within the bounds of a circle button
*
* @param 	Button, a circular button to be compared against the point
* @param 	X, X coord of the touch
* @param    Y, Y coord of the touch
*
*
******************************************************************************/
int Button_CheckCircle(button_t * Button, u16 X, u16 Y)
{
	Xil_AssertNonvoid(Button);
	int disp_sq = (X - Button->PosX)*(X - Button->PosX) +
				  (Y - Button->PosY)*(Y - Button->PosY);

	int rad_sq = (Button->DimA)*(Button->DimA);

	if (disp_sq < rad_sq) {
		return 1;
	} else {
		return 0;
	}
}

/*****************************************************************************/
/**
*
*  Checks to see if a press falls within the bounds of a rect button
*
* @param 	Button, a rectangular button to be compared against the point
* @param 	X, X coord of the touch
* @param    Y, Y coord of the touch
*
*
******************************************************************************/
int Button_CheckRect(button_t * Button, u16 X, u16 Y)
{
	Xil_AssertNonvoid(Button);
	if ((X > (Button->PosX+Button->DimA))  ||
		 (X < (Button->PosX))) {
		return 0;
	}

	if ((Y > (Button->PosY+Button->DimB))  ||
		 (Y < (Button->PosY))) {
		return 0;
	}

	return 1;
}

/*****************************************************************************/
/**
*
* Prepends a button to the beginning of the button list. Buttons at the front
* of the list have the highest prioirty. (Ie, if they overlap and existing one,
* the press is registered to the first).
*
* @param 	Manager, owner of the button list
* @param 	button, to be prepended
*
*
******************************************************************************/
void ButtonList_Prepend(ButtonManager_t * Manager, button_t * Button)
{
	Button->Next = Manager->ButtonList;
	if (Manager->ButtonList)
	{
		Manager->ButtonList->Prev = Button;
	}
	Manager->ButtonList = Button;
	Button ->Prev = NULL;
}

/*****************************************************************************/
/**
*
* Prepends a button to the beginning of the button list. Buttons at the front
* of the list have the highest prioirty. (Ie, if they overlap and existing one,
* the press is registered to the first).
*
* @param 	Manager, owner of the button list
* @param 	button, to be prepended
*
*
******************************************************************************/
button_t * ButtonList_Find(ButtonManager_t * Manager, u16 X, u16 Y)
{
	button_t * Node = Manager->ButtonList;
	while (Node) {
		if (Node->Enabled){
			switch (Node->shape){
				case CIRCLE:
					if (Button_CheckCircle(Node, X, Y))
					{
						return Node;
					}
					break;
				case RECT:
					if (Button_CheckRect(Node, X, Y))
					{
						return Node;
					}
					break;
				default:
					break;
			}
		}
		Node=Node->Next;
	}
	return NULL;
}

/*****************************************************************************/
/**
*
* Adds a button_t pointer to the list of registered buttons in the button
* manager.
*
* @param 	Manager, where the button will be registered
* @param 	button to be registered
*
*
******************************************************************************/
void TouchSensorButtons_RegisterButton(ButtonManager_t * Manager, button_t * Button)
{
	Xil_AssertVoid(Manager != NULL);
	Xil_AssertVoid(Button != NULL);

	//TODO: Check for double registrations.
	ButtonList_Prepend(Manager, Button);

}

/*****************************************************************************/
/**
*
*  Initalizes a button to a circle type, and sets it's origin and radius
*  fields.
*
* @param 	Button, the button to be initialized
* @param    radius, radius of the button
* @param 	X, X coord of the button's origin
* @param    Y, Y coord of the button's origin
*
*
******************************************************************************/
void Button_SetCircleDim(button_t * Button, u16 radius, u16 X, u16 Y)
{
	Xil_AssertVoid(Button);
	Button->shape = CIRCLE;
	Button->PosX = X;
	Button->PosY = Y;
	Button->DimA = radius;

}

/*****************************************************************************/
/**
*
*  Initalizes a button to a rectangle
*
* @param 	Button, the button to be initialized
* @param    radius, radius of the button
* @param 	X, X coord of the button's origin
* @param    Y, Y coord of the button's origin
*
*
******************************************************************************/
void Button_SetRectDim(button_t * Button, u16 LenX, u16 LenY, u16 X, u16 Y)
{
	Xil_AssertVoid(Button);
	Button->shape = RECT;
	Button->PosX = X;
	Button->PosY = Y;
	Button->DimA = LenX;
	Button->DimB = LenY;
}

/*****************************************************************************/
/**
*
* Sets the handler of the button. See the header file for the legal
* signature
*
* @param 	Button, the button whose handler is to be set.
* @param    Funcptr, function pointer to the buttons interrupt handler
*
******************************************************************************/
void Button_AssignHandler(button_t * Button, void * FuncPtr)
{
	Xil_AssertVoid(Button != NULL);
	Xil_AssertVoid(FuncPtr != NULL);
	Button->Handler = (TouchSensorButtons_Handler) FuncPtr;
}

/*****************************************************************************/
/**
*
* Enable the button. See the header file for the legal signature
*
* @param 	Button, the button whose handler is to be set.
*
******************************************************************************/
void TouchSensorButtons_EnableButton(button_t * Button){
	Xil_AssertVoid(Button != NULL);
	Button->Enabled = 1;
}

/*****************************************************************************/
/**
*
* Disable the button. See the header file for the legal signature
*
* @param 	Button, the button whose handler is to be set.
*
******************************************************************************/
void TouchSensorButtons_DisableButton(button_t * Button){
	Xil_AssertVoid(Button != NULL);
	Button->Enabled = 0;
}

/*****************************************************************************/
/**
*
* Removes a button from the button list.
*
* @param 	Manager, owner of the button list
* @param 	Button, button to be removed
*
*
******************************************************************************/
void ButtonList_Remove(ButtonManager_t * Manager, button_t * Button)
{
	button_t * next = Button->Next;
	button_t * prev = Button->Prev;
	if (Manager->ButtonList == Button){
		Manager->ButtonList = next;
	}
	if (next)
		next->Prev = prev;
	if (prev)
		prev->Next = next;
}

/*****************************************************************************/
/**
*
* Removes a button_t pointer to the list of registered buttons in the button
* manager.
*
* @param 	Manager, where the button will be registered
* @param 	Button, button to be removed
*
*
******************************************************************************/
void TouchSensorButtons_RemoveButton(ButtonManager_t * Manager, button_t * Button)
{
	Xil_AssertVoid(Manager != NULL);
	Xil_AssertVoid(Button != NULL);

	//TODO: Check for double registrations.
	ButtonList_Remove(Manager, Button);
}

/*****************************************************************************/
/**
*
* Render the button on the TFT screen
*
* @param 	Button, button to be rendered
* @param 	colour, colour of the button
* @param	TftPtr, pointer to the tft structure on which button is rendered
*
*
******************************************************************************/
void TouchSensorButtons_RenderButton(button_t * Button, u16 colour, TFT * TftPtr){
	Xil_AssertVoid(Button != NULL);
	switch (Button->shape){
					case CIRCLE:
						Button_RenderCircle(Button, colour, TftPtr);
						break;
					case RECT:
						Button_RenderRect(Button, colour, TftPtr);
						break;
					default:
						break;
				}
}

/*****************************************************************************/
/**
*
* Look up sin value from a table
*
* @param 	i, angle in degrees
*
******************************************************************************/
double sin_lookup(double i){
	int sign = 1;
	int index = 0;
	if(i>180){
		sign = -1;
		i = i - 180;
	}
	if (i>90)
		i = 180 - i;
	index = i/SIN_DIV;
	double result = sign*sin_a[index];
	return result;
}

/*****************************************************************************/
/**
*
* Look up cos value from a table
*
* @param 	i, angle in degrees
*
******************************************************************************/
double cos_lookup(double i){
	if(i>270)
		i = i - 270;
	else
		i = i + 90;
	return sin_lookup(i);
}

/*****************************************************************************/
/**
*
* Render a straight line
*
* @param 	x1,y1, starting point coordinates
* @param 	x2,y2, end point coordinates
* @param 	colour, colour of the line
* @param	TftPtr, pointer to the tft instance on which button is rendered
*
******************************************************************************/
void render_line(double x1, double y1, double x2, double y2, u16 colour, TFT *TftPtr){
	double m = (y2-y1)/(x2-x1);
	if (x1>x2){
		draw_line(x2,y2,x1,y1,m,colour, TftPtr);
		return;
	}
	draw_line(x1,y1,x2,y2,m,colour, TftPtr);
}

void draw_line(double x1, double y1, double x2, double y2, double m, u16 colour, TFT *TftPtr){
	double i = x1;
	double j = y1;
	while(i<=x2 ){
		TFT_WriteToPixel((int)i, (int)j, colour, TFT_GetImageAddress(TftPtr));
		i++;
		j = j+m;
	}
}
/*****************************************************************************/
/**
*
* Render a wireframe circle around button on TFT screen
*
* @param 	Button, button to be rendered
* @param 	colour, colour of the button
* @param	TftPtr, pointer to the tft instance on which button is rendered
*
*
******************************************************************************/
void Button_RenderCircle(button_t * Button, u16 colour, TFT *TftPtr){
	double x, y, old_x, old_y;
	int i = 0;
	x = (Button->DimA*(cos_lookup(i))) + Button->PosX;
	y = (Button->DimA*(sin_lookup(i))) + Button->PosY;;
	for(i = SIN_DIV; i<=360; i = i+SIN_DIV){
		old_x = x;
		old_y = y;
		x = (Button->DimA*(cos_lookup(i))) + Button->PosX;
		y = (Button->DimA*(sin_lookup(i))) + Button->PosY;
		render_line(old_x, old_y, x, y, colour, TftPtr);
	}
}

/*****************************************************************************/
/**
*
* Render a wireframe rectangle around button on TFT screen
*
* @param 	Button, button to be rendered
* @param 	colour, colour of the button
* @param	TftPtr, pointer to the tft instance on which button is rendered
*
*
******************************************************************************/
void Button_RenderRect(button_t * Button, u16 colour, TFT *TftPtr){
	int i;
	int BottomX = Button->PosX;
	int TopX = BottomX + Button->DimA;
	int BottomY = Button->PosY;
	int TopY = BottomY + Button->DimB;
	for (i = BottomX; i <= TopX; i++) {

			TFT_WriteToPixel(i, BottomY, colour, TFT_GetImageAddress(TftPtr));
			TFT_WriteToPixel(i, TopY, colour, TFT_GetImageAddress(TftPtr));
	}
	for (i = BottomY; i <= TopY; i++) {
			TFT_WriteToPixel(BottomX, i, colour, TFT_GetImageAddress(TftPtr));
			TFT_WriteToPixel(TopX, i, colour, TFT_GetImageAddress(TftPtr));
	}
}



