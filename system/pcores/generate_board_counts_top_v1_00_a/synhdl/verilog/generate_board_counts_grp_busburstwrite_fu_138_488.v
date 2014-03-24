// ------------------------------------------------------
// This file is generated for Xilinx by AutoPilot automatically.
// Copyright (C) 2011 XILINX, Inc.
// 
// ------------------------------------------------------


`timescale 1 ns / 1 ps
module generate_board_counts_grp_busburstwrite_fu_138_488 (clk, reset, bus_req_din, bus_req_full_n,  bus_req_write, bus_rsp_empty_n, bus_rsp_read,  bus_address, bus_size, bus_addressin, mem_addressin, bus_datain,  bus_dataout, mem_address0, mem_ce0, mem_we0, mem_d0, mem_q0, sizein, command, start, done);

parameter INTERNAL_WFIFO_DEPTH = 4;
parameter INTERNAL_WFIFO_ADDRBW = 2;
parameter MEM_LATENCY = 1;
parameter BW = 32;
parameter MEMAW = 3;

input clk;
input reset;
output bus_req_din;
input  bus_req_full_n;
output bus_req_write;
input  bus_rsp_empty_n;
output bus_rsp_read;
output[31:0] bus_address;
output[31:0] bus_size;
input[31:0]  bus_addressin;
input[MEMAW - 1:0] mem_addressin;
input[BW - 1:0] bus_datain;
output[BW - 1:0] bus_dataout;
output[MEMAW - 1:0] mem_address0;
output mem_ce0;
output mem_we0;
output[BW - 1:0] mem_d0;
input[BW - 1:0]  mem_q0;
input[31:0] sizein;
input command;
input start;
output done;


parameter idle = 4'b0000;
parameter buswr_hold = 4'b0001;
parameter buswr_addr = 4'b0010;
parameter buswr_addr_data = 4'b0011;
parameter buswr_data = 4'b0100;
parameter buswr_flush = 4'b0101;
parameter busrd_prepare = 4'b0110;
parameter busrd_addr = 4'b0111;
parameter busrd_data = 4'b1000;
parameter busrd_flush = 4'b1001;
reg[3:0] cs, ns;
reg[31:0] sig_counter;
reg[2:0] sig_mem_latency;
reg[MEMAW - 1:0] sig_mem_addr;
wire[31:0] sig_normalized_size;
wire sig_mem_we;

reg[31:0] sig_counter2;
reg internal_req_fifo_full_n_buff[0:MEM_LATENCY-1];
reg[BW-1:0] internal_wfifo_data[0:INTERNAL_WFIFO_DEPTH-1];
reg[INTERNAL_WFIFO_ADDRBW-1:0] internal_wfifo_ptr_in, internal_wfifo_ptr_out;
wire internal_wfifo_empty;
wire internal_wfifo_read, internal_wfifo_write;
wire[BW-1:0] internal_wfifo_din, internal_wfifo_dout;

assign internal_wfifo_dout =  internal_wfifo_data[internal_wfifo_ptr_out];
assign internal_wfifo_empty =  (internal_wfifo_ptr_in == internal_wfifo_ptr_out)? 1'b1: 1'b0;

always @ (posedge clk) begin
    if ( reset == 1'b1 ) begin
        internal_req_fifo_full_n_buff[0] <= 1'b1;
    end else begin
        internal_req_fifo_full_n_buff[0] <=  bus_req_full_n;
    end
end

always @ (posedge clk)
begin
    if (internal_wfifo_write) begin
        internal_wfifo_data[internal_wfifo_ptr_in] <= internal_wfifo_din;
    end
end

always @ (posedge clk) begin
    if ( reset == 1'b1 ) begin
        internal_wfifo_ptr_in <= 2'd0;
    end else begin
        if (internal_wfifo_write) begin
            internal_wfifo_ptr_in <= internal_wfifo_ptr_in + 1;
        end
    end
end

always @ (posedge clk) begin
    if ( reset == 1'b1 ) begin
        internal_wfifo_ptr_out <= 2'd0;
    end else begin
        if (internal_wfifo_read) begin
            internal_wfifo_ptr_out <= internal_wfifo_ptr_out + 1;
        end
    end
end

always @ (posedge clk) begin
    if ( reset == 1'b1 ) begin
        cs <= idle;
    end else begin
        cs <= ns;
    end
end

always @ (cs or sig_counter or sig_mem_latency or sizein or bus_req_full_n or start or bus_rsp_empty_n or command or sig_mem_we
 or sig_counter2 or internal_req_fifo_full_n_buff[0])
begin
    ns = cs;
    case(cs)
    idle: begin
        if (start && sizein != 32'b0) begin
            if (command == 1'b0) begin
                ns = busrd_prepare;
            end else begin
                if (bus_req_full_n) begin
                    ns = buswr_addr;
                end else begin
                    ns = buswr_hold;
                end
            end
         end
    end

    buswr_hold: begin
        if (bus_req_full_n) begin
            ns = buswr_addr;
        end
    end

    buswr_addr: begin
        if (sig_mem_latency == 3'b001) begin
            if (sizein == 32'b1) begin
              ns = buswr_data;
            end else begin
              ns = buswr_addr_data;
            end
        end
    end

    buswr_addr_data: begin
        if (sig_counter == 32'H00000001 & bus_req_full_n) begin
            ns = buswr_data;
        end
    end

    buswr_data: begin
        if (sig_counter2 == 32'H00000001 & internal_req_fifo_full_n_buff[0]) begin
            ns = buswr_flush;
        end
    end

    buswr_flush: begin
        if (bus_req_full_n) begin
            ns = idle;
        end
    end

    busrd_prepare: begin
        if (bus_req_full_n == 1'b1) begin
            ns = busrd_addr;
        end
    end

    busrd_addr: begin
        ns = busrd_data;
    end

    busrd_data: begin
        if (sig_mem_we == 1'b1 & sig_counter == 32'h00000001)  begin
            ns = busrd_flush;
        end
    end

    busrd_flush: begin
        ns = idle;
    end

    default: begin
        ns = cs;
    end
    endcase
end


always @ (posedge clk)
begin
    //sig_counter
    if (cs == idle & ns != idle) begin
        sig_counter <= sig_normalized_size;
    end else if (((cs == buswr_addr | cs == buswr_addr_data)  & bus_req_full_n) | (cs == busrd_data & sig_mem_we)) begin
        sig_counter <= sig_counter - 1;
    end

    //sig_counter2
    if (cs == idle & ns != idle) begin
        sig_counter2 <= sig_normalized_size;
    end else if ((cs == buswr_addr_data | cs == buswr_data)  & internal_req_fifo_full_n_buff[0]) begin
        sig_counter2 <= sig_counter2 - 1;
    end

    //sig_mem_latency
    if (cs == idle & ns != idle) begin
        sig_mem_latency <= MEM_LATENCY;
    end else if (cs == buswr_addr) begin
        sig_mem_latency <= sig_mem_latency - 1;
    end

    //sig_mem_addr
    if (cs == idle & ns != idle) begin
        sig_mem_addr <= mem_addressin;
    end else if (((cs == buswr_addr | cs == buswr_addr_data) &  bus_req_full_n) | (cs == busrd_data & sig_mem_we)) begin
        sig_mem_addr <= sig_mem_addr + 1;
    end

end

assign done = (cs == idle && internal_wfifo_empty == 1'b1)? 1'b1: 1'b0;

assign sig_normalized_size = (sizein[31:1] ==  31'b0000000000000000000000000000000)? 32'h00000001: sizein;
assign sig_mem_we = (cs == busrd_data && bus_rsp_empty_n  == 1'b1)? 1'b1: 1'b0;
assign bus_req_din = (cs == busrd_prepare | cs == busrd_addr | cs == busrd_data)? 1'b0: 1'b1;
assign bus_req_write = ((~internal_wfifo_empty & bus_req_full_n) | cs == busrd_addr)? 1'b1: 1'b0;
assign bus_rsp_read = sig_mem_we;
assign bus_address = bus_addressin;
assign bus_size = sizein;
assign bus_dataout = internal_wfifo_dout;
assign internal_wfifo_din = mem_q0;
assign mem_address0 = sig_mem_addr;
assign mem_ce0 = 1'b1;
assign mem_we0 = sig_mem_we;
assign mem_d0 = bus_datain;
assign internal_wfifo_write = ((cs == buswr_addr_data | cs == buswr_data) & internal_req_fifo_full_n_buff[0])? 1'b1: 1'b0;
assign internal_wfifo_read = (~internal_wfifo_empty & bus_req_full_n)? 1'b1: 1'b0;


endmodule

