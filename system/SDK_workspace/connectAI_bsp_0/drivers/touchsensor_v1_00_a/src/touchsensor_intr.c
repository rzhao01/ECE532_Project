
/***************************** Include Files *********************************/

#include "touchsensor.h"


/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/


/************************** Variable Definitions *****************************/


/*****************************************************************************/
/**
*
* Sets the touchsensor callback function, which the driver calls when the
* touchscreen is touched
*
* @param	InstancePtr is a pointer to the TouchSensor instance .
* @param	CallBackRef is the upper layer callback reference passed back
*		when the callback function is invoked.
* @param	FuncPtr is the pointer to the callback function.
*
* @return	None.
*
* @note
*
* The handler is called within interrupt context so the function that is
* called should either be short or pass the more extensive processing off
* to another task to allow the interrupt to return and normal processing
* to continue.
*
******************************************************************************/
void TouchSensor_SetHandler(TouchSensor * InstancePtr, TouchSensor_Handler FuncPtr,
			void *CallBackRef)
{
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(FuncPtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	InstancePtr->Handler = FuncPtr;
	InstancePtr->CallBackRef = CallBackRef;
}

/*****************************************************************************/
/**
*
* Interrupt Service Routine (ISR) for the driver.  This function only performs
* processing for the device and does not save and restore the interrupt context.
*
* @param	InstancePtr contains a pointer to the touchsensor instance for
*		the interrupt.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void TouchSensor_InterruptHandler(void *InstancePtr)
{
	TouchSensor *TouchSensorPtr = NULL;

	/*
	 * Verify that each of the inputs are valid.
	 */
	Xil_AssertVoid(InstancePtr != NULL);

	/*
	 * Convert the non-typed pointer to an touchsensor instance pointer
	 * such that there is access to the touchsensor
	 */
	TouchSensorPtr = (TouchSensor *) InstancePtr;

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
		TouchSensorPtr->Handler(TouchSensorPtr->CallBackRef);
	}

}

/****************************************************************************/
/**
*
* Sets the debounce period of the touch pad. After <debounce_period> cycles
* the touchpad will generate a second interrupt.
*
* @param	InstancePtr contains the TouchSensor instance
* @param	DebouncePeriod specifies the number of cycles
*
* @return	Nothing
*
****************************************************************************/

void TouchSensor_EnableDebouncing(TouchSensor *InstancePtr, u32 DebouncePeriod) {
	Xil_AssertVoid(InstancePtr != NULL);
	InstancePtr->DebouncePeriod = DebouncePeriod;
}


/****************************************************************************/
/**
*
* Clears the debounce period. When debouce_period = 0, the touchSensors
* counter never produces a secondary interrupt.
*
* @param	InstancePtr contains the TouchSensor instance
*
* @return	Nothing
*
****************************************************************************/

void TouchSensor_DisableDebouncing(TouchSensor *InstancePtr) {
	Xil_AssertVoid(InstancePtr != NULL);
	InstancePtr->DebouncePeriod = 0;
}

/****************************************************************************/
/**
*
* Enables interrupts on the TS, must be enabled to use debouncing capabilities
* Interrupts will be produced on the rising edge of TouchEn
*
* @param	InstancePtr contains the TouchSensor instance
*
* @return	Nothing
*
****************************************************************************/

void TouchSensor_EnableInterrupts(TouchSensor *InstancePtr) {
	Xil_AssertVoid(InstancePtr != NULL);
	InstancePtr->InterruptsEnabled = 1;
	TouchSensor_EnableIntr(InstancePtr->BaseAddress);
}

/****************************************************************************/
/**
*
* Enables interrupts on the TS, must be enabled to use debouncing capabilities
*
* @param	InstancePtr contains the TouchSensor instance
*
* @return	Nothing
*
****************************************************************************/

void TouchSensor_DisableInterrupts(TouchSensor *InstancePtr) {
	Xil_AssertVoid(InstancePtr != NULL);
	InstancePtr->InterruptsEnabled = 0;
	TouchSensor_DisableIntr(InstancePtr->BaseAddress);
}


