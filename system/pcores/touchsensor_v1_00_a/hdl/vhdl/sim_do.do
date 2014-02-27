cd C:/Xilinx/14.2/ISE_DS/edk_user_repository/MyProcessorIPLib/pcores/touchsensor_v1_00_a/hdl/vhdl
vlib work
vcom div.vhd
vcom touchCtrl.vhd
vcom user_logic.vhd
vsim work.user_logic
add wave -position end  sim:/user_logic/CLK_I
add wave -position end  sim:/user_logic/TP_CS_O
add wave -position end  sim:/user_logic/TP_DIN_O
add wave -position end  sim:/user_logic/TP_DOUT_I
add wave -position end  sim:/user_logic/TP_DCLK_O
add wave -position end  sim:/user_logic/TP_BUSY_I
add wave -position end  sim:/user_logic/TP_PENIRQ_I
add wave -position end  sim:/user_logic/TOUCH_X
add wave -position end  sim:/user_logic/TOUCH_Y
add wave -position end  sim:/user_logic/TouchXNorm1
add wave -position end  sim:/user_logic/TouchYNorm1
add wave -position end  sim:/user_logic/TouchXNorm2
add wave -position end  sim:/user_logic/TouchYNorm2
add wave -position end  sim:/user_logic/TouchXNorm
add wave -position end  sim:/user_logic/TouchYNorm
add wave -position end  sim:/user_logic/SysClk
add wave -position end  sim:/user_logic/TouchX
add wave -position end  sim:/user_logic/TouchY
add wave -position end  sim:/user_logic/TouchZ
add wave -position end  sim:/user_logic/Bus2IP_Clk
add wave -position end  sim:/user_logic/Bus2IP_Resetn
add wave -position end  sim:/user_logic/Bus2IP_Data
add wave -position end  sim:/user_logic/Bus2IP_BE
add wave -position end  sim:/user_logic/Bus2IP_RdCE
add wave -position end  sim:/user_logic/Bus2IP_WrCE
force CLK_I 1 50 -r 100
force CLK_I 0 100 -r 100
run 200

run 200
force Bus2IP_Resetn 0
run 100
history