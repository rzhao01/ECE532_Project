cd C:/Xilinx/14.2/ISE_DS/edk_user_repository/MyProcessorIPLib/pcores/touchsensor_v1_00_a/hdl/vhdl
vlib work
vcom div.vhd
vcom touchCtrl.vhd
vcom user_logic.vhd
vsim work.user_logic
add wave -position end  sim:/user_logic/CLK_I

force CLK_I 1 50 -r 100
force CLK_I 0 100 -r 100
