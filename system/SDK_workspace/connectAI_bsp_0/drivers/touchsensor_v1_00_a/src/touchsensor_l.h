/*
 * touchsensor_l.h
 *
 *  Created on: Feb 2, 2014
 *      Author: David Biancolin
 */

#ifndef TOUCHSENSOR_L_H_
#define TOUCHSENSOR_L_H_




#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files ********************************/

//#include "xil_types.h"
#include "xil_assert.h"
#include "xil_io.h"

/*
/************************** Constant Definitions ****************************/

/* TouchScreene register offsets */

#define TS_POSITION_REG_OFFSET			0	/* Register holding touch_En, X and Y positions */
#define TS_TIMER_REG_OFFSET			4 	/* Time value register */
//#define TS_TIMER_THRES_REG_OFFSET	3	/* Timer Threshold */
#define TS_CONTROL_REG_OFFSET	8	/* control register */


/* Control Register bit positions */

#define TS_CR_ENABLE_INTR		0x1	/* enable interrupt */
#define TS_CR_ENABLE_COUNT		0x2	/* enable debounce counter*/

#define TS_POS_TOUCHEN_OFFSET   31
#define TS_POS_X_OFFSET   		16
#define	TS_POS_COORD_MASK  		0x1FF


/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/

/*
 * Define the appropriate I/O access method to memory mapped I/O or DCR.
 */
/*
#if (XPAR_TouchSensor_USE_DCR_BRIDGE != 0)

#define TouchSensor_In32  XIo_DcrIn
#define TouchSensor_Out32 XIo_DcrOut

#else */

#define TouchSensor_In32  Xil_In32
#define TouchSensor_Out32 Xil_Out32

//#endif


/****************************************************************************/
/**
*
* Write a value to a TouchSensor register. A 32 bit write is performed.
*
* @param	BaseAddress is the base address of the TouchSensor device.
* @param	RegOffset is the register offset from the base to write to.
* @param	Data is the data written to the register.
*
* @return	None.
*
* @note		C-style signature:
*		void TouchSensor_WriteReg(u32 BaseAddress, u32 RegOffset,
*					u32 Data)
*
****************************************************************************/
#define TouchSensor_WriteReg(BaseAddress, RegOffset, Data) \
	TouchSensor_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/****************************************************************************/
/**
*
* Read a value from a TouchSensor register. A 32 bit read is performed.
*
* @param	BaseAddress is the base address of the TouchSensor device.
* @param	RegOffset is the register offset from the base to read from.
*
* @return	Data read from the register.
*
* @note		C-style signature:
*		u32 TouchSensor_ReadReg(u32 BaseAddress, u32 RegOffset)
*
****************************************************************************/
#define TouchSensor_ReadReg(BaseAddress, RegOffset) \
	TouchSensor_In32((BaseAddress) + (RegOffset))


/****************************************************************************/
/**
*
* Set the contents of the control register. Use the TS_CR_* constants defined
* above to create the bit-mask to be written to the register.
*
* @param	BaseAddress is the base address of the device
* @param	Mask is the 32-bit value to write to the control register
*
* @return	None.
*
* @note		C-style Signature:
*		void TouchSensor_SetControlReg(u32 BaseAddress, u32 Mask);
*
*****************************************************************************/
#define TouchSensor_SetControlReg(BaseAddress, Mask) \
	TouchSensor_WriteReg((BaseAddress), TS_CONTROL_REG_OFFSET, (Mask))


/****************************************************************************/
/**
*
* Set the timer threshold/initial value for debouncing
*
* @param	BaseAddress is the base address of the device
* @param	Mask is the 32-bit value to write to the threshold register
*
* @return	None.
*
* @note		C-style Signature:
*		void TouchSensor_SetControlReg(u32 BaseAddress, u32 Mask);
*
*****************************************************************************/
#define TouchSensor_SetTimerReg(BaseAddress, Mask) \
	TouchSensor_WriteReg((BaseAddress), TS_TIMER_REG_OFFSET, (Mask))

/****************************************************************************/
/**
*
* Get the current value of the debounce timer
*
* @param	BaseAddress is the  base address of the device
*
* @return	A 32-bit value representing the present value of the counter
*
* @note		C-style Signature:
*		u32 TouchSensor_GetStatusReg(u32 BaseAddress);
*
*****************************************************************************/
#define TouchSensor_GetTimerReg(BaseAddress) \
		TouchSensor_ReadReg((BaseAddress), TS_TIMER_REG_OFFSET)

/****************************************************************************/
/**
*
* Get the current value of the position register
*
* @param	BaseAddress is the  base address of the device
*
* @return	A 32-bit value representing the present value of the register
* 			MSB = TouchEn
* 			17:9 = Y position
* 			8:0 = X position
*
* @note		C-style Signature:
*		u32 TouchSensor_GetStatusReg(u32 BaseAddress);
*
*****************************************************************************/
#define TouchSensor_GetPosReg(BaseAddress) \
		TouchSensor_ReadReg((BaseAddress), TS_POSITION_REG_OFFSET)

/****************************************************************************/
/**
*
* Mask out the Y position from the position register
*
* @param	RegValue is the value of the position register
*
* @return	Y position
*
* @note		C-style Signature:
*		u32 TouchSensor_GetStatusReg(u32 BaseAddress);
*
*****************************************************************************/
#define TouchSensor_GetY(RegValue) \
		RegValue & TS_POS_COORD_MASK

/****************************************************************************/
/**
*
* Shift and mask out the X position from the position register
*
* @param	RegValue is the value of the position register
*
* @return	X position
*
* @note		C-style Signature:
*		u32 TouchSensor_GetStatusReg(u32 BaseAddress);
*
*****************************************************************************/
#define TouchSensor_GetX(RegValue) \
		(RegValue >> TS_POS_X_OFFSET) & TS_POS_COORD_MASK


/****************************************************************************/
/**
*
*  Shift TouchEn from the position register
*
* @param	RegValue is the value of the position register
*
* @return	TouchEn (1 if screen depressed)
*
* @note		C-style Signature:
*		u32 TouchSensor_GetStatusReg(u32 BaseAddress);
*
*****************************************************************************/
#define TouchSensor_GetTouchEn(RegValue) \
		(RegValue >> TS_POS_TOUCHEN_OFFSET)


/****************************************************************************/
/**
*
* Disable debouncing capabilities. The TS will produce interrupts on all
* rising edges of TouchEn.
*
* @param	BaseAddress is the  base address of the device
* @param	Mask include TS_CR_ENABLE_INTR if the device should
* 			continue producing interrupts (CR write-only?)
*
* @return	None.
*
* @note		C-style Signature:
* 		void TouchSensor_EnableIntr(u32 BaseAddress);
*
*****************************************************************************/
#define TouchSensor_DisableDebounce(BaseAddress, Mask) \
		TouchSensor_SetControlReg((BaseAddress), Mask)

/****************************************************************************/
/**
*
* Enable debouncing. Note this also enable interrupts.
*
* @param	BaseAddress is the  base address of the device
*
* @return	None.
*
* @note		C-style Signature:
* 		void TouchSensor_EnableIntr(u32 BaseAddress);
*
*****************************************************************************/
#define TouchSensor_EnableDebounce(BaseAddress) \
		TouchSensor_SetControlReg((BaseAddress), (TS_CR_ENABLE_INTR|TS_CR_ENABLE_COUNT))

/****************************************************************************/
/**
*
* Enable the device interrupt. We cannot read the control register, so we
* just write the enable interrupt bit and clear all others. Since the only
* other ones are the FIFO reset bits, this works without side effects.
*
* @param	BaseAddress is the  base address of the device
*
* @return	None.
*
* @note		C-style Signature:
* 		void TouchSensor_EnableIntr(u32 BaseAddress);
*
*****************************************************************************/
#define TouchSensor_EnableIntr(BaseAddress) \
		TouchSensor_SetControlReg((BaseAddress), TS_CR_ENABLE_INTR)


/****************************************************************************/
/**
*
* Disable the device interrupt. Debouncing is also disabled as debouncing
* requires interrupts
*
* @param	BaseAddress is the  base address of the device
*
* @return	None.
*
* @note		C-style Signature:
* 		void TouchSensor_DisableIntr(u32 BaseAddress);
*
*****************************************************************************/
#define TouchSensor_DisableIntr(BaseAddress) \
		TouchSensor_SetControlReg((BaseAddress), 0)

/************************** Function Prototypes *****************************/



#ifdef __cplusplus
}
#endif


#endif /* TOUCHSENSOR_L_H_ */
