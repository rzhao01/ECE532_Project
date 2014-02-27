/*
 * tft_perhipheral.c
 *
 *  Created on: 2013-02-24
 *      Author: Rob
 */

// INCLUDES
#include "tft.h"
#include "xil_io.h"

int TFT_CfgInitialize(TFT * InstancePtr, TFT_Config * Config,
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

void TFT_TurnOn(TFT* InstancePtr) {
	u8 currentMSEL = Xil_In32((volatile u32 *)InstancePtr->BaseAddress + 1);
	Xil_Out32((volatile u32 *)InstancePtr->BaseAddress + 1, ((0x1 << 3) | (currentMSEL & 0x7)));
}
void TFT_TurnOff(TFT* InstancePtr) {
	u8 currentMSEL = Xil_In32((volatile u32 *)InstancePtr->BaseAddress + 1);
	Xil_Out32((volatile u32 *)InstancePtr->BaseAddress + 1, ((0x0 << 3) | (currentMSEL & 0x7)));
}
int TFT_IsOn(TFT* InstancePtr) {
	 return ((Xil_In32((volatile u32 *)InstancePtr->BaseAddress + 1) >> 3) & 0x1);
}

void TFT_SetBrightness(TFT* InstancePtr, int Brightness) {
	u8 currentMSEL = Xil_In32((volatile u32 *)InstancePtr->BaseAddress + 1);
	Xil_Out32((volatile u32 *)InstancePtr->BaseAddress + 1, ((currentMSEL & 0x8) | (Brightness & 0x7)));
}
int TFT_GetBrightness(TFT* InstancePtr) {
	return (Xil_In32((volatile u32 *)InstancePtr->BaseAddress + 1) & 0x7);
}

void TFT_SetImageAddress(TFT* InstancePtr, u32 ImageAddress) {
	Xil_Out32((volatile u32 *)InstancePtr->BaseAddress, ImageAddress);
}

u32 TFT_GetImageAddress(TFT* InstancePtr) {
	return Xil_In32((volatile u32 *)InstancePtr->BaseAddress);
}

void TFT_WriteToPixel(int x, int y, u16 color, u32 ImageAddress) {
	if (x > 479 || x < 0 || y > 271 || y < 0) return;
	volatile u16* ddrmem = (u16 *)ImageAddress;
	int pixelAddr = 480 * y + x;
	// Switch order of adjacent pixels to accommodate the FIFO in hardware
	if (pixelAddr % 2 == 0) {
		ddrmem[pixelAddr+1] = (u16)color;
	}
	else {
		ddrmem[pixelAddr-1] = (u16)color;
	}
}
