############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 2012 Xilinx Inc. All rights reserved.
############################################################
set_directive_inline "zero_counts"
set_directive_inline "add_counts"
set_directive_inline "get_square"
set_directive_array_partition -type complete -dim 1 "count_horiz" b
set_directive_array_partition -type complete -dim 1 "count_vert" b
set_directive_array_partition -type complete -dim 1 "count_ne" b
set_directive_array_partition -type complete -dim 1 "count_se" b
set_directive_loop_flatten "count_se/count_se_loop3"
set_directive_loop_flatten "count_ne/count_ne_loop3"
set_directive_pipeline "count_ne/count_ne_loop2"
set_directive_pipeline "count_se/count_se_loop2"
set_directive_pipeline "count_vert/count_vert_loop2"
set_directive_loop_flatten "count_vert/count_vert_loop3"
set_directive_loop_flatten "count_horiz/count_horiz_loop3"
set_directive_pipeline "count_horiz/count_horiz_loop2"
set_directive_unroll "generate_board_counts/generate_board_counts_load_data"
set_directive_unroll "generate_board_counts/generate_board_counts_load_data2"
