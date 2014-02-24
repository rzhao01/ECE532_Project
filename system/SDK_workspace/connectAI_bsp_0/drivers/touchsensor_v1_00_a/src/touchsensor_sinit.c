/***************************** Include Files ********************************/

#include "xstatus.h"
#include "xparameters.h"
#include "touchsensor_i.h"

/************************** Constant Definitions ****************************/

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/

/************************** Variable Definitions ****************************/


/************************** Function Prototypes *****************************/


/******************************************************************************/
/**
* Lookup the device configuration based on the unique device ID.  The table
* ConfigTable contains the configuration info for each device in the system.
*
* @param	DeviceId is the device identifier to lookup.
*
* @return
*		 - A pointer of data type TouchSensor_Config which points to the
*		device configuration if DeviceID is found.
* 		- NULL if DeviceID is not found.
*
* @note		None.
*
******************************************************************************/
TouchSensor_Config *TouchSensor_LookupConfig(u16 DeviceId)
{
	TouchSensor_Config *CfgPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_TOUCHSENSOR_NUM_INSTANCES; Index++) {
		if (TouchSensor_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &TouchSensor_ConfigTable[Index];
			break;
		}
	}

	return CfgPtr;
}


/****************************************************************************/
/**
* Initialize the TouchSensor instance provided by the caller based on the
* given DeviceID.
*
* Nothing is done except to initialize the InstancePtr.
*
* @param	InstancePtr is a pointer to a TouchSensor instance. The memory the
*		pointer references must be pre-allocated by the caller. Further
*		calls to manipulate the instance/driver through the TouchSensor API
*		must be made with this pointer.
* @param	DeviceId is the unique id of the device controlled by this TouchSensor
*		instance. Passing in a device id associates the generic TouchSensor
*		instance to a specific device, as chosen by the caller or
*		application developer.
*
* @return
*		- XST_SUCCESS if the initialization was successfull.
* 		- XST_DEVICE_NOT_FOUND  if the device configuration data was not
*		found for a device with the supplied device ID.
*
* @note		None.
*
*****************************************************************************/
int TouchSensor_Initialize(TouchSensor * InstancePtr, u16 DeviceId)
{
	TouchSensor_Config *ConfigPtr;

	/*
	 * Assert arguments
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);

	/*
	 * Lookup configuration data in the device configuration table.
	 * Use this configuration info down below when initializing this
	 * driver.
	 */

	ConfigPtr = TouchSensor_LookupConfig(DeviceId);
	if (ConfigPtr == (TouchSensor_Config *) NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	InstancePtr->InterruptsEnabled = 0;
	InstancePtr->DebouncePeriod = 0;

	return TouchSensor_CfgInitialize(InstancePtr, ConfigPtr,
				   ConfigPtr->BaseAddress);
}
