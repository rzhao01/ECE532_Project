############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 2012 Xilinx Inc. All rights reserved.
############################################################
open_project board_counts
set_top generate_board_counts
add_files common.h
add_files board.h
add_files board.c -cflags "-DSYNTHESIS"
add_files -tb testbench.c
add_files -tb out.gold.dat
open_solution "solution1"
set_part  {xc6slx45csg324-3}
create_clock -period 10

source "./board_counts/solution1/directives.tcl"
csynth_design
