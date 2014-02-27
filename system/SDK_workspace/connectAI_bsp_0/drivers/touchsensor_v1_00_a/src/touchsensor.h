/*
 * touchsensor.h
 *
 *  Created on: 2013-02-24
 *      Author: Rob
 */

#ifndef TOUCHSENSOR_H_
#define TOUCHSENSOR_H_

// INCLUDES
#include "xbasic_types.h"
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "touchsensor_l.h"

#define TOUCH_DOWN 1
#define TOUCH_UP 0

/**
 * Signature for the callback function.
 *
 * @param	CallBackRef is a callback reference passed in by the upper layer
 *		when setting the callback functions, and passed back to the
 *		upper layer when the callback is invoked. Its type is
 *		 unimportant to the driver, so it is a void pointer.
 */
typedef void (*TouchSensor_Handler) (void *CallBackRef);

typedef struct {
	u16 x, y;
} TouchSensor_Touch;

// STRUCTS
typedef struct {
	u32 BaseAddress;
	u32 IsReady;
	int InterruptsEnabled;

	int DebouncePeriod;
	int InitialDetection;  //Flag used to check if a timer interrupt is expected

	TouchSensor_Touch LastTouch;

	TouchSensor_Handler Handler;
	void *CallBackRef;
} TouchSensor;


/**
 * This typedef contains configuration information for the device.
 */
typedef struct {
	u16 DeviceId;		/* Unique ID  of device */
	u32 BaseAddress;	/* Device base address */

} TouchSensor_Config;


// FUNCTIONS
/*
 * Initialization functions in xtouchsensor_sinit.c
 */
int TouchSensor_Initialize(TouchSensor *InstancePtr, u16 DeviceId);
TouchSensor_Config *TouchSensor_LookupConfig(u16 DeviceId);

/*
 * API Basic functions implemented in xgpio.c
 */
int TouchSensor_CfgInitialize(TouchSensor *InstancePtr, TouchSensor_Config * Config,
			u32 EffectiveAddr);

/*
 * Accepts a toucheSensor instance pointer to read from.
 * returns TOUCH_DOWN if screen is currently being touched and TOUCH_UP otherwise.
 * If TOUCH_DOWN, an updated copy of X and Y are saved to the last touch member
 * of the touchSensor instance.
 */
u8 TouchSensor_GetTouch(TouchSensor *touchSensor);


void TouchSensor_SetHandler(TouchSensor * InstancePtr, TouchSensor_Handler FuncPtr,
			void *CallBackRef);

/*
 *  Configures the debouncing settings for interrupts.
 */

void TouchSensor_EnableDebouncing(TouchSensor *InstancePtr, u32 DebouncePeriod);
void TouchSensor_DisableDebouncing(TouchSensor *InstancePtr);

/*
 *   Configures the touchpad to enable or disable the generation of interrupts.
 */

void TouchSensor_EnableInterrupts(TouchSensor *);
void TouchSensor_DisableInterrupts(TouchSensor *);

void TouchSensor_InterruptHandler(void *InstancePtr);


#endif /* TOUCHSENSOR_H_ */
