/*
 * tft_perhipheral.h
 *
 *  Created on: 2013-02-24
 *      Author: Rob
 */

#ifndef TFT_H_
#define TFT_H_

// INCLUDES
#include "xbasic_types.h"
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"

// DEFINES
#define TFT_ON 1
#define TFT_OFF 0

// STRUCTS
typedef struct {
	u32 BaseAddress;
	u32 IsReady;
} TFT;

typedef struct {
	u16 x, y;
} TFT_Point;

/**
 * This typedef contains configuration information for the device.
 */
typedef struct {
	u16 DeviceId;		/* Unique ID  of device */
	u32 BaseAddress;	/* Device base address */
} TFT_Config;

// FUNCTIONS
/*
 * Initialization functions in tft_sinit.c
 */
 // Used to initialize a TFT instance
int TFT_Initialize(TFT *InstancePtr, u16 DeviceId);
// Meant for internal use
TFT_Config *TFT_LookupConfig(u16 DeviceId);

/*
 * API Basic functions implemented in tft.c
 */
 // Meant for internal use
int TFT_CfgInitialize(TFT *InstancePtr, TFT_Config * Config,
			u32 EffectiveAddr);
			
// Turns on a connected TFT Peripheral
void TFT_TurnOn(TFT* InstancePtr);
// Turns off a connected TFT Peripheral
void TFT_TurnOff(TFT* InstancePtr);
// Returns TFT_ON if connected TFT Peripheral is on and
// TFT_OFF otherwise
int TFT_IsOn(TFT* InstancePtr);

// Sets brightness of connected TFT Peripheral
// Accepts bightness values from 0 (off) to 7 (brightest)
void TFT_SetBrightness(TFT* InstancePtr, int Brightness);
int TFT_GetBrightness(TFT* InstancePtr);

// Sets Image Adress of connected TFT Peripheral
// This address should correspond to a memory location in DDR memory
void TFT_SetImageAddress(TFT* InstancePtr, u32 ImageAddress);
u32 TFT_GetImageAddress(TFT* InstancePtr);

// Helper function for writing to a pixel locatated in an image in  memory
void TFT_WriteToPixel(int x, int y, u16 color, u32 ImageAddress);



#endif /* TOUCHSENSOR_H_ */
