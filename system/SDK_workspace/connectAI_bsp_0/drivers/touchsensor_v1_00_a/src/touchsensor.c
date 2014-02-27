/*
 * touchsensor.c
 *
 *  Created on: 2013-02-24
 *      Author: Rob
 */

// INCLUDES
#include "touchsensor.h"
#include "xil_io.h"

int TouchSensor_CfgInitialize(TouchSensor * InstancePtr, TouchSensor_Config * Config,
			u32 EffectiveAddr)
{
	/*
	 * Assert arguments
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);

	/*
	 * Set some default values.
	 */
	InstancePtr->BaseAddress = EffectiveAddr;

	/*
	 * Indicate the instance is now ready to use, initialized without error
	 */
	InstancePtr->IsReady = XIL_COMPONENT_IS_READY;
	return (XST_SUCCESS);
}



u8 TouchSensor_GetTouch(TouchSensor *InstancePtr){
	u32 TouchData = TouchSensor_GetPosReg(InstancePtr->BaseAddress);
	u8 touchEn = TouchSensor_GetTouchEn(TouchData);
	if (TouchSensor_GetTouchEn(TouchData)) {
		InstancePtr->LastTouch.x = TouchSensor_GetX(TouchData);
		InstancePtr->LastTouch.y = TouchSensor_GetY(TouchData);
		return TOUCH_DOWN;
	}

	return TOUCH_UP;

}

