##############################################################################
## Filename:          C:\Xilinx\14.2\ISE_DS\edk_user_repository\MyProcessorIPLib/drivers/tft_perhipheral_v1_00_a/data/tft_perhipheral_v1_00_a.tcl
## Description:       Microprocess Driver Command (tcl)
## Date:              Wed Feb 06 14:29:30 2013 (by Create and Import Peripheral Wizard)
##############################################################################

#uses "xillib.tcl"

proc generate {drv_handle} {
  xdefine_include_file $drv_handle "xparameters.h" "TFT" "NUM_INSTANCES" "DEVICE_ID" "C_BASEADDR" "C_HIGHADDR"
  xdefine_config_file $drv_handle "tft_g.c" "TFT"  "DEVICE_ID" "C_BASEADDR"
}
