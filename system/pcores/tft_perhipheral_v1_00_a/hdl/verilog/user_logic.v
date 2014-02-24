//----------------------------------------------------------------------------
// user_logic.v - module
//----------------------------------------------------------------------------
//
// ***************************************************************************
// ** Copyright (c) 1995-2012 Xilinx, Inc.  All rights reserved.            **
// **                                                                       **
// ** Xilinx, Inc.                                                          **
// ** XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"         **
// ** AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND       **
// ** SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,        **
// ** OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,        **
// ** APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION           **
// ** THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,     **
// ** AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE      **
// ** FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY              **
// ** WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE               **
// ** IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR        **
// ** REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF       **
// ** INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS       **
// ** FOR A PARTICULAR PURPOSE.                                             **
// **                                                                       **
// ***************************************************************************
//
//----------------------------------------------------------------------------
// Filename:          user_logic.v
// Version:           1.00.a
// Description:       User logic module.
// Date:              Sun Feb 24 21:30:23 2013 (by Create and Import Peripheral Wizard)
// Verilog Standard:  Verilog-2001
//----------------------------------------------------------------------------
// Naming Conventions:
//   active low signals:                    "*_n"
//   clock signals:                         "clk", "clk_div#", "clk_#x"
//   reset signals:                         "rst", "rst_n"
//   generics:                              "C_*"
//   user defined types:                    "*_TYPE"
//   state machine next state:              "*_ns"
//   state machine current state:           "*_cs"
//   combinatorial signals:                 "*_com"
//   pipelined or register delay signals:   "*_d#"
//   counter signals:                       "*cnt*"
//   clock enable signals:                  "*_ce"
//   internal version of output port:       "*_i"
//   device pins:                           "*_pin"
//   ports:                                 "- Names begin with Uppercase"
//   processes:                             "*_PROCESS"
//   component instantiations:              "<ENTITY_>I_<#|FUNC>"
//----------------------------------------------------------------------------

`uselib lib=unisims_ver
`uselib lib=proc_common_v3_00_a

module user_logic
(
  // -- ADD USER PORTS BELOW THIS LINE ---------------
  // --USER ports added here 
  ul_R_O,
  ul_G_O,
  ul_B_O,
  ul_DE_O,
  ul_CLK_O,
  ul_DISP_O,
  ul_BKLT_O, // PWM backlight control
  ul_VDDEN_O,
  
  TFTClk,
  TFTClk180,
  
    debug_ReadFromFIFO,
    debug_X_I,
    debug_Y_I,

    //debug_TFTClk,
    debug_mst_fifo_valid_write_xfer,
    debug_mst_fifo_valid_read_xfer,
    debug_FIFO_Full,
    debug_FIFO_Empty,
    debug_RGB_I,
    debug_vram_data ,
    debug_reg_wrdata,
	debug_read_address,
	
	debug_ul_state,
	debug_ul_msel,
	debug_ul_slv_reg0,
	debub_ul_slv_reg1,
	debug_mst_cmd_sm_set_done,
	debug_mst_cmd_sm_busy,
	debug_bus2ip_mstrd_d,
	debug_fifo_reset,
  
  // -- ADD USER PORTS ABOVE THIS LINE ---------------

  // -- DO NOT EDIT BELOW THIS LINE ------------------
  // -- Bus protocol ports, do not add to or delete 
  Bus2IP_Clk,                     // Bus to IP clock
  Bus2IP_Resetn,                  // Bus to IP reset
  Bus2IP_Data,                    // Bus to IP data bus
  Bus2IP_BE,                      // Bus to IP byte enables
  Bus2IP_RdCE,                    // Bus to IP read chip enable
  Bus2IP_WrCE,                    // Bus to IP write chip enable
  IP2Bus_Data,                    // IP to Bus data bus
  IP2Bus_RdAck,                   // IP to Bus read transfer acknowledgement
  IP2Bus_WrAck,                   // IP to Bus write transfer acknowledgement
  IP2Bus_Error,                   // IP to Bus error response
  ip2bus_mstrd_req,               // IP to Bus master read request
  ip2bus_mstwr_req,               // IP to Bus master write request
  ip2bus_mst_addr,                // IP to Bus master read/write address
  ip2bus_mst_be,                  // IP to Bus byte enable
  ip2bus_mst_length,              // Ip to Bus master transfer length
  ip2bus_mst_type,                // Ip to Bus burst assertion control
  ip2bus_mst_lock,                // Ip to Bus bus lock
  ip2bus_mst_reset,               // Ip to Bus master reset
  bus2ip_mst_cmdack,              // Bus to Ip master command ack
  bus2ip_mst_cmplt,               // Bus to Ip master trans complete
  bus2ip_mst_error,               // Bus to Ip master error
  bus2ip_mst_rearbitrate,         // Bus to Ip master re-arbitrate for bus ownership
  bus2ip_mst_cmd_timeout,         // Bus to Ip master command time out
  bus2ip_mstrd_d,                 // Bus to Ip master read data
  bus2ip_mstrd_rem,               // Bus to Ip master read data rem
  bus2ip_mstrd_sof_n,             // Bus to Ip master read start of frame
  bus2ip_mstrd_eof_n,             // Bus to Ip master read end of frame
  bus2ip_mstrd_src_rdy_n,         // Bus to Ip master read source ready
  bus2ip_mstrd_src_dsc_n,         // Bus to Ip master read source dsc
  ip2bus_mstrd_dst_rdy_n,         // Ip to Bus master read dest. ready
  ip2bus_mstrd_dst_dsc_n,         // Ip to Bus master read dest. dsc
  ip2bus_mstwr_d,                 // Ip to Bus master write data
  ip2bus_mstwr_rem,               // Ip to Bus master write data rem
  ip2bus_mstwr_src_rdy_n,         // Ip to Bus master write source ready
  ip2bus_mstwr_src_dsc_n,         // Ip to Bus master write source dsc
  ip2bus_mstwr_sof_n,             // Ip to Bus master write start of frame
  ip2bus_mstwr_eof_n,             // Ip to Bus master write end of frame
  bus2ip_mstwr_dst_rdy_n,         // Bus to Ip master write dest. ready
  bus2ip_mstwr_dst_dsc_n          // Bus to Ip master write dest. ready
  // -- DO NOT EDIT ABOVE THIS LINE ------------------
); // user_logic

// -- ADD USER PARAMETERS BELOW THIS LINE ------------
// --USER parameters added here 

parameter H_480_272p_AV = 480;      //active video
parameter V_480_272p_AV = 272;  

// -- DO NOT EDIT BELOW THIS LINE --------------------
// -- Bus protocol parameters, do not add to or delete
parameter C_MST_NATIVE_DATA_WIDTH        = 32;
parameter C_LENGTH_WIDTH                 = 12;
parameter C_MST_AWIDTH                   = 32;
parameter C_NUM_REG                      = 8;
parameter C_SLV_DWIDTH                   = 32;
// -- DO NOT EDIT ABOVE THIS LINE --------------------

// -- ADD USER PORTS BELOW THIS LINE -----------------
// --USER ports added here 
  output [7 : 0] ul_R_O;
  output [7 : 0] ul_G_O;
  output [7 : 0] ul_B_O;
  output ul_DE_O;
  output ul_CLK_O;
  output ul_DISP_O;
  output ul_BKLT_O; // PWM backlight control
  output ul_VDDEN_O;
  
  input TFTClk;
  input TFTClk180;
  
    output debug_ReadFromFIFO;
    output signed [31 : 0] debug_X_I;
    output signed [31 : 0] debug_Y_I;
    
   //output debug_TFTClk;
   output debug_mst_fifo_valid_write_xfer;
   output debug_mst_fifo_valid_read_xfer;
   output debug_FIFO_Full;
   output debug_FIFO_Empty;
   output [C_MST_NATIVE_DATA_WIDTH - 1:0] debug_RGB_I;
   output [15 : 0] debug_vram_data ;
   output [15 : 0] debug_reg_wrdata;
   output [C_MST_AWIDTH-1 : 0] debug_read_address;
   output [2:0] debug_ul_state;
   output [3:0] debug_ul_msel;
   output [31:0] debug_ul_slv_reg0;
   output [3:0] debub_ul_slv_reg1;
   output debug_mst_cmd_sm_set_done;
   output debug_mst_cmd_sm_busy;
   output [31:0] debug_bus2ip_mstrd_d;
   output debug_fifo_reset;
   
// -- ADD USER PORTS ABOVE THIS LINE -----------------

// -- DO NOT EDIT BELOW THIS LINE --------------------
// -- Bus protocol ports, do not add to or delete
input                                     Bus2IP_Clk;
input                                     Bus2IP_Resetn;
input      [C_SLV_DWIDTH-1 : 0]           Bus2IP_Data;
input      [C_SLV_DWIDTH/8-1 : 0]         Bus2IP_BE;
input      [C_NUM_REG-1 : 0]              Bus2IP_RdCE;
input      [C_NUM_REG-1 : 0]              Bus2IP_WrCE;
output     [C_SLV_DWIDTH-1 : 0]           IP2Bus_Data;
output                                    IP2Bus_RdAck;
output                                    IP2Bus_WrAck;
output                                    IP2Bus_Error;
output                                    ip2bus_mstrd_req;
output                                    ip2bus_mstwr_req;
output     [C_MST_AWIDTH-1 : 0]           ip2bus_mst_addr;
output     [(C_MST_NATIVE_DATA_WIDTH/8)-1 : 0] ip2bus_mst_be;
output     [C_LENGTH_WIDTH-1 : 0]         ip2bus_mst_length;
output                                    ip2bus_mst_type;
output                                    ip2bus_mst_lock;
output                                    ip2bus_mst_reset;
input                                     bus2ip_mst_cmdack;
input                                     bus2ip_mst_cmplt;
input                                     bus2ip_mst_error;
input                                     bus2ip_mst_rearbitrate;
input                                     bus2ip_mst_cmd_timeout;
input      [C_MST_NATIVE_DATA_WIDTH-1 : 0] bus2ip_mstrd_d;
input      [(C_MST_NATIVE_DATA_WIDTH)/8-1 : 0] bus2ip_mstrd_rem;
input                                     bus2ip_mstrd_sof_n;
input                                     bus2ip_mstrd_eof_n;
input                                     bus2ip_mstrd_src_rdy_n;
input                                     bus2ip_mstrd_src_dsc_n;
output                                    ip2bus_mstrd_dst_rdy_n;
output                                    ip2bus_mstrd_dst_dsc_n;
output     [C_MST_NATIVE_DATA_WIDTH-1 : 0] ip2bus_mstwr_d;
output     [(C_MST_NATIVE_DATA_WIDTH)/8-1 : 0] ip2bus_mstwr_rem;
output                                    ip2bus_mstwr_src_rdy_n;
output                                    ip2bus_mstwr_src_dsc_n;
output                                    ip2bus_mstwr_sof_n;
output                                    ip2bus_mstwr_eof_n;
input                                     bus2ip_mstwr_dst_rdy_n;
input                                     bus2ip_mstwr_dst_dsc_n;
// -- DO NOT EDIT ABOVE THIS LINE --------------------

//----------------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------------

  // --USER nets declarations added here, as needed for user logic
  wire [7 : 0] ul_R_O;
  wire [7 : 0] ul_G_O;
  wire [7 : 0] ul_B_O;
  wire ul_DE_O;
  wire ul_CLK_O;
  wire ul_DISP_O;
  wire ul_BKLT_O; // PWM backlight control
  wire ul_VDDEN_O;
  
  // --USER nets declarations added here, as needed for user logic
   wire TFTClk;
   wire TFTClk180;
   wire SysRst;
   wire SysClk;
   wire TFTMSel;
   reg ReadFromFIFO;
   reg signed [31 : 0] ul_X_I;
   reg signed [31 : 0] ul_Y_I;
   reg  [3:0] ul_MSEL;
   //integer ul_X_I;
   //integer ul_Y_I;
   wire [15:0] ul_RGB_I;
   wire ul_FIFO_Full;
   wire ul_FIFO_Empty;
   wire ul_WR_CLK;
   
   
   wire debug_ReadFromFIFO;
   wire [31 : 0] debug_X_I;
   wire [31 : 0] debug_Y_I;
   //still need to add~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   //wire debug_TFTClk;
   wire debug_mst_fifo_valid_write_xfer;
   wire debug_mst_fifo_valid_read_xfer;
   wire debug_FIFO_Full;
   wire debug_FIFO_Empty;
   wire [C_MST_NATIVE_DATA_WIDTH - 1:0] debug_RGB_I;
   wire [15 : 0] debug_vram_data ;
   wire [15 : 0] debug_reg_wrdata;
   
   wire [2:0] debug_ul_state;
   wire [3:0] debug_ul_msel;
   wire [31:0] debug_ul_slv_reg0;
   wire [3:0] debub_ul_slv_reg1;
   wire debug_mst_cmd_sm_set_done;
   wire debug_mst_cmd_sm_busy;
   wire [31:0] debug_bus2ip_mstrd_d;
   wire debug_fifo_reset;
   
// -- ADD USER PARAMETERS ABOVE THIS LINE ------------

  // Nets for user logic slave model s/w accessible register example
  reg        [C_SLV_DWIDTH-1 : 0]           slv_reg0;
  reg        [C_SLV_DWIDTH-1 : 0]           slv_reg1;
  reg        [C_SLV_DWIDTH-1 : 0]           slv_reg2;
  reg        [C_SLV_DWIDTH-1 : 0]           slv_reg3;
  wire       [3 : 0]                        slv_reg_write_sel;
  wire       [3 : 0]                        slv_reg_read_sel;
  reg        [C_SLV_DWIDTH-1 : 0]           slv_ip2bus_data;
  wire                                      slv_read_ack;
  wire                                      slv_write_ack;
  integer                                   byte_index, bit_index;


  wire                                       Bus2IP_Clk;
  wire                                       Bus2IP_Resetn;
  wire      [C_SLV_DWIDTH-1 : 0]             Bus2IP_Data;
  wire      [C_SLV_DWIDTH/8-1 : 0]           Bus2IP_BE;
  wire      [C_NUM_REG-1 : 0]                Bus2IP_RdCE;
  wire      [C_NUM_REG-1 : 0]                Bus2IP_WrCE;
  wire      [C_SLV_DWIDTH-1 : 0]             IP2Bus_Data;
  wire                                       IP2Bus_RdAck;
  wire                                       IP2Bus_WrAck;
  wire                                       IP2Bus_Error;
  wire                                       ip2bus_mstrd_req;
  wire                                       ip2bus_mstwr_req;
  wire     [C_MST_AWIDTH-1 : 0]              ip2bus_mst_addr;
  wire     [(C_MST_NATIVE_DATA_WIDTH/8)-1:0] ip2bus_mst_be;
  wire     [C_LENGTH_WIDTH-1 : 0]        ip2bus_mst_length;
  wire                                       ip2bus_mst_type;
  wire                                       ip2bus_mst_lock;
  wire                                       ip2bus_mst_reset;
  wire                                       bus2ip_mst_cmdack;
  wire                                       bus2ip_mst_cmplt;
  wire                                       bus2ip_mst_error;
  wire                                       bus2ip_mst_rearbitrate;
  wire                                       bus2ip_mst_cmd_timeout;
  wire     [C_MST_NATIVE_DATA_WIDTH-1 : 0]   bus2ip_mstrd_d;
  wire     [(C_MST_NATIVE_DATA_WIDTH/8)-1:0] bus2ip_mstrd_rem; 
  wire                                       bus2ip_mstrd_sof_n;
  wire                                       bus2ip_mstrd_eof_n;
  wire                                       bus2ip_mstrd_src_rdy_n;
  wire                                       bus2ip_mstrd_src_dsc_n;
  wire                                       ip2bus_mstrd_dst_rdy_n;
  wire                                       ip2bus_mstrd_dst_dsc_n;
  wire     [C_MST_NATIVE_DATA_WIDTH-1 : 0]   ip2bus_mstwr_d;
  wire     [(C_MST_NATIVE_DATA_WIDTH/8)-1:0] ip2bus_mstwr_rem;
  wire                                       ip2bus_mstwr_src_rdy_n;
  wire                                       ip2bus_mstwr_src_dsc_n;
  wire                                       ip2bus_mstwr_sof_n;
  wire                                       ip2bus_mstwr_eof_n;
  wire                                       bus2ip_mstwr_dst_rdy_n;
  wire                                       bus2ip_mstwr_dst_dsc_n;

// signals for master model control/status registers write/read
  reg        [C_SLV_DWIDTH-1 : 0]            mst_ip2bus_data;
 wire                                       mst_reg_write_req;
 wire                                       mst_reg_read_req;
 wire       [3 : 0]                         mst_reg_write_sel;
 wire       [3 : 0]                         mst_reg_read_sel;
 wire                                       mst_write_ack;
 wire                                       mst_read_ack;
// signals for master model control/status registers
 reg        [7 : 0]                         mst_reg [0 : 15];
 reg        [15 : 0]                        mst_byte_we;
 reg                                       mst_cntl_rd_req;
 wire                                       mst_cntl_wr_req;
 wire                                       mst_cntl_bus_lock;
 wire                                       mst_cntl_burst;
 reg       [C_MST_AWIDTH-1 : 0]            mst_ip2bus_addr;
 wire       [C_LENGTH_WIDTH-1 : 0]      mst_xfer_length;
 wire       [19 : 0]                    mst_xfer_reg_len;
 wire       [15 : 0]                        mst_ip2bus_be;
 reg                                        mst_go;
// signals for master model command interface state machine
 parameter  [1 : 0]                         CMD_IDLE = 2'b00,
                                            CMD_RUN  = 2'b01,
                                            CMD_WAIT_FOR_DATA  = 2'b10,
                                            CMD_DONE  = 2'b11;
 reg        [1 : 0]                         mst_cmd_sm_state;
 reg                                        mst_cmd_sm_set_done;
 reg                                        mst_cmd_sm_set_error;
 reg                                        mst_cmd_sm_set_timeout;
 reg                                        mst_cmd_sm_busy;
 reg                                        mst_cmd_sm_clr_go;
 reg                                        mst_cmd_sm_rd_req;
 reg                                        mst_cmd_sm_wr_req;
 reg                                        mst_cmd_sm_reset;
 reg                                        mst_cmd_sm_bus_lock;
 reg        [C_MST_AWIDTH-1 : 0]            mst_cmd_sm_ip2bus_addr;
 reg        [(C_MST_NATIVE_DATA_WIDTH/8)-1 : 0]mst_cmd_sm_ip2bus_be;
 reg                                        mst_cmd_sm_xfer_type;
 reg        [C_LENGTH_WIDTH-1 : 0]      mst_cmd_sm_xfer_length;
 reg                                        mst_cmd_sm_start_rd_llink;
 reg                                        mst_cmd_sm_start_wr_llink;
 
// signals for master model read locallink interface state machine
 parameter                                  LLRD_IDLE = 1'b0,
                                            LLRD_GO = 1'b1;
 reg                                        mst_llrd_sm_state;
 reg                                        mst_llrd_sm_dst_rdy;
// signals for master model write locallink interface state machine
 parameter   [2 : 0]                        LLWR_IDLE = 3'b000, 
                                            LLWR_SNGL_INIT = 3'b001, 
                             LLWR_SNGL = 3'b010, 
                             LLWR_BRST_INIT = 3'b011, 
                             LLWR_BRST = 3'b100, 
                             LLWR_BRST_LAST_BEAT = 3'b101;
 
 reg         [2 : 0]                        mst_llwr_sm_state;
 reg                                        mst_llwr_sm_src_rdy;
 reg                                        mst_llwr_sm_sof;
 reg                                        mst_llwr_sm_eof;
 integer                                    mst_llwr_byte_cnt;
 wire                                       mst_fifo_valid_write_xfer;
 wire                                       mst_fifo_valid_read_xfer;
 wire                                       fifo_Reset;
 
 parameter                                  BE_WIDTH = C_SLV_DWIDTH/8;
 parameter                                  BYTES_PER_BEAT = C_MST_NATIVE_DATA_WIDTH/8;
 parameter  [7:0]                           GO_DATA_KEY = 8'ha;
 parameter                                  GO_BYTE_LANE = 15;
 
  // --USER logic implementation added here

  // ------------------------------------------------------
  // Example code to read/write user logic slave model s/w accessible registers
  // 
  // Note:
  // The example code presented here is to show you one way of reading/writing
  // software accessible registers implemented in the user logic slave model.
  // Each bit of the Bus2IP_WrCE/Bus2IP_RdCE signals is configured to correspond
  // to one software accessible register by the top level template. For example,
  // if you have four 32 bit software accessible registers in the user logic,
  // you are basically operating on the following memory mapped registers:
  // 
  //    Bus2IP_WrCE/Bus2IP_RdCE   Memory Mapped Register
  //                     "1000"   C_BASEADDR + 0x0
  //                     "0100"   C_BASEADDR + 0x4
  //                     "0010"   C_BASEADDR + 0x8
  //                     "0001"   C_BASEADDR + 0xC
  // 
  // ------------------------------------------------------

  assign
    slv_reg_write_sel = Bus2IP_WrCE[3:0],
    slv_reg_read_sel  = Bus2IP_RdCE[3:0],
    slv_write_ack     = Bus2IP_WrCE[0] || Bus2IP_WrCE[1] || Bus2IP_WrCE[2] || Bus2IP_WrCE[3],
    slv_read_ack      = Bus2IP_RdCE[0] || Bus2IP_RdCE[1] || Bus2IP_RdCE[2] || Bus2IP_RdCE[3];

  // implement slave model register(s)
  always @( posedge Bus2IP_Clk )
    begin: SLAVE_REG_WRITE_PROC

      if ( Bus2IP_Resetn == 0 )
        begin
          slv_reg0 <= 0;
          slv_reg1 <= 0;
          slv_reg2 <= 0;
          slv_reg3 <= 0;
        end
      else
        case ( slv_reg_write_sel )
          4'b1000 :
            for ( byte_index = 0; byte_index <= (C_SLV_DWIDTH/8)-1; byte_index = byte_index+1 )
              if ( Bus2IP_BE[byte_index] == 1 )
                for ( bit_index = byte_index*8; bit_index <= byte_index*8+7; bit_index = bit_index+1 )
                  slv_reg0[bit_index] <= Bus2IP_Data[bit_index];
          4'b0100 :
            for ( byte_index = 0; byte_index <= (C_SLV_DWIDTH/8)-1; byte_index = byte_index+1 )
              if ( Bus2IP_BE[byte_index] == 1 )
                for ( bit_index = byte_index*8; bit_index <= byte_index*8+7; bit_index = bit_index+1 )
                  slv_reg1[bit_index] <= Bus2IP_Data[bit_index];
          4'b0010 :
            for ( byte_index = 0; byte_index <= (C_SLV_DWIDTH/8)-1; byte_index = byte_index+1 )
              if ( Bus2IP_BE[byte_index] == 1 )
                for ( bit_index = byte_index*8; bit_index <= byte_index*8+7; bit_index = bit_index+1 )
                  slv_reg2[bit_index] <= Bus2IP_Data[bit_index];
          4'b0001 :
            for ( byte_index = 0; byte_index <= (C_SLV_DWIDTH/8)-1; byte_index = byte_index+1 )
              if ( Bus2IP_BE[byte_index] == 1 )
                for ( bit_index = byte_index*8; bit_index <= byte_index*8+7; bit_index = bit_index+1 )
                  slv_reg3[bit_index] <= Bus2IP_Data[bit_index];
          default : ;
        endcase

    end // SLAVE_REG_WRITE_PROC

  // implement slave model register read mux
  always @( slv_reg_read_sel or slv_reg0 or slv_reg1 or slv_reg2 or slv_reg3 )
    begin: SLAVE_REG_READ_PROC

      case ( slv_reg_read_sel )
        4'b1000 : slv_ip2bus_data <= slv_reg0;
        4'b0100 : slv_ip2bus_data <= slv_reg1;
        4'b0010 : slv_ip2bus_data <= slv_reg2;
        4'b0001 : slv_ip2bus_data <= slv_reg3;
        default : slv_ip2bus_data <= 0;
      endcase

    end // SLAVE_REG_READ_PROC
	

 // ----------------------------------------------------------------------------------
//[sanket] TFT Controller
//----------------------------------------------------------------------------------	
	
    assign SysClk = Bus2IP_Clk;
    
 // --USER logic implementation added here
  	// SysCon Inst_SysCon(
		// .CLK_I(Bus2IP_Clk),
		// .CLK_O(SysClk),
		// .TFT_CLK_O(TFTClk), //this is a 9MHz clock, need to input from CLK generator
		// .TFT_CLK_180_O(TFTClk180),
		////.MSEL_O(TFTMSel),
		// .RSTN_I(BTNM_I),
		////.SW_I(SW_I),
		// .ASYNC_RST(SysRst)
	// );
	
	reg [2:0] ul_state;
	parameter   [2 : 0]     UL_IDLE = 3'b000, 
							UL_FILL = 3'b001, 
							UL_READING = 3'b010, 
                            UL_REST = 3'b011;
	
	always @ (posedge Bus2IP_Clk)
	begin
		ul_MSEL[2:0] <= slv_reg1[2:0];
		
		if (Bus2IP_Resetn == 1'b0 || slv_reg1[3] == 1'b0)
		begin
			ul_MSEL[3] <= 1'b0;
			ul_state <= UL_IDLE;
			mst_go <= 1'b0;
			mst_ip2bus_addr <= slv_reg0;
		end
		else
		begin
			case (ul_state)

				UL_IDLE:
				begin
					ul_MSEL[3] <= 1'b0;
					mst_ip2bus_addr <= slv_reg0;
					mst_go <= 1'b0;
					if (slv_reg1[3] == 1'b1)
					begin
						ul_state <= UL_FILL;
					end
					else
					begin
						ul_state <= UL_IDLE;
					end
				end
				
				UL_FILL:
				begin
					mst_ip2bus_addr <= mst_ip2bus_addr;
					if (mst_cmd_sm_clr_go == 1'b1 )
					begin
						mst_go <= 1'b0;
					end
					else if ( mst_cmd_sm_busy == 1'b0 && (!ul_FIFO_Full) ) // every 64 pixels go get more data
					begin
						mst_go   <= 1'b1;
						ul_state <= UL_READING;
					end
					else
					begin
						ul_state <= UL_FILL;
						mst_go <= 1'b0;
					end
					
					if (ul_FIFO_Full && !ul_FIFO_Empty && slv_reg1[3] == 1'b1)
					begin
						ul_MSEL[3] <= 1'b1;
					end
				end
				
				UL_READING:
				begin
					mst_go   <= 1'b0;
					if (mst_cmd_sm_set_done == 1'b1)
					begin
						if (mst_ip2bus_addr >= slv_reg0 + 261120 - 128 )//32'hC003FB7E)//slv_reg0 + 65248) // magic number weeee! # of words in tft panel (480pixels*272pixels*2bytesperpixel)/4bytesperword - 32words
						begin
							mst_ip2bus_addr <= slv_reg0;
							ul_state <= UL_FILL;
						end
						else
						begin
							mst_ip2bus_addr <= mst_ip2bus_addr + 128; //128 bytes
							//mst_ip2bus_addr <= mst_ip2bus_addr;
							ul_state <= UL_FILL;
						end
					end
					else
					begin
						ul_state <= UL_READING;
					end
				end
				
			
				default:
				begin
				 ul_state <= UL_IDLE;
				 ul_MSEL[3] <= 1'b0;
				 mst_ip2bus_addr <= slv_reg0;
				 mst_go <= 1'b0;
				end
			
			endcase
		end
	end
	
	always @(posedge TFTClk)
	begin
		if (Bus2IP_Resetn == 1'b0 || ul_MSEL[3] == 0)
		begin
			ul_X_I <= 0;
			ul_Y_I <= 0;
		end
		else if (ul_DE_O == 1'b1)
		begin
			if (ul_X_I == H_480_272p_AV-1)
			begin
				ul_X_I <= 0;
				if (ul_Y_I == V_480_272p_AV-1)
				begin
					ul_Y_I <= 0;
				end
				else
				begin
					ul_Y_I <= ul_Y_I + 1;
				end
			end
			else
			begin
				ul_X_I <= ul_X_I + 1; 
			end
		end
		else
		begin
			ul_X_I <= ul_X_I;
			ul_Y_I <= ul_Y_I;
		end
	end
	
    // always @(posedge TFTClk)
    // begin
        // if (((ul_X_I >= H_480_272p_AV-1) && (ul_Y_I >= V_480_272p_AV-1) && ul_DE_O == 1'b1) || (!Bus2IP_Resetn))
        // begin
            // ul_X_I <= 0;
            // ul_Y_I <= 0;
            // mst_ip2bus_addr <= slv_reg0;
        // end
        // else if ((ul_X_I >= (H_480_272p_AV-1)) && (ul_FIFO_Full == 0) && ul_DE_O == 1'b1)
        // begin
            // ul_X_I <= 0;
            // ul_Y_I <= ul_Y_I + 1; // Read from memory- burst size - 240 required
            // mst_ip2bus_addr <= mst_ip2bus_addr + 240; // Probably 240 words
        // end
		// else if (ul_DE_O == 1'b0)
		// begin
			// ul_X_I <= ul_X_I;
			// ul_Y_I <= ul_Y_I;
		// end
        // else 
        // begin
            // ul_X_I <= ul_X_I + 1; 
            
        // end
    // end
  
    // implement master model write only 'go' port to trigger start of master rd/wr transaction


  
  TFTCtl TFTCtl_1
  (
		.CLK_I(TFTClk),
		.CLK_180_I(TFTClk180),
		.RST_I(~Bus2IP_Resetn),
		
		.X_I(ul_X_I),
		.Y_I(ul_Y_I),
		.RGB_I(ul_RGB_I),
		.WE_I(~ul_FIFO_Empty),
		.WR_CLK(Bus2IP_Clk),
		
		.ReadFromFIFO(ReadFromFIFO),
		.R_O(ul_R_O),
		.G_O(ul_G_O),
		.B_O(ul_B_O),
		.DE_O(ul_DE_O),
		.CLK_O(ul_CLK_O),
		.DISP_O(ul_DISP_O),
		.BKLT_O(ul_BKLT_O), // PWM backlight control
		.VDDEN_O(ul_VDDEN_O),
		.MSEL_I(ul_MSEL),
        .debug_vram_data(debug_vram_data),
        .debug_reg_wrdata(debug_reg_wrdata)
	);

//  ------------------------------------------
// Example code to demonstrate user logic master model functionality
// 
// Note:
// The example code presented here is to show you one way of instatiating
// the AXI4 (Burst) master interface under user control. It is provided for
// demonstration purposes only and allows the user to exercise the AXI4 (Burst)
// master interface during test and evaluation of the template.
// This user logic master model contains a 16-byte flattened register and
// the user is required to initialize the value to desire and then write to
// the model's 'Go' port to initiate the user logic master operation.
//
//    Control Register     (C_BASEADDR + OFFSET + 0x0):
//       bit 0    - Rd     (Read Request Control)
//       bit 1    - Wr     (Write Request Control)
//       bit 2    - BL     (Bus Lock Control)
//       bit 3    - Brst   (Burst Assertion Control)
//       bit 4-7  - Spare  (Spare Control Bits)
//    Status Register      (C_BASEADDR + OFFSET + 0x1):
//       bit 0    - Done   (Transfer Done Status)
//       bit 1    - Busy   (User Logic Master is Busy)
//       bit 2    - Error  (User Logic Master request got error response)
//       bit 3    - Tmout  (User Logic Master request is timeout)
//       bit 2-7  - Spare  (Spare Status Bits)
//    Addrress Register    (C_BASEADDR + OFFSET + 0x4):
//       bit 0-31 - Target Address (This 32-bit value is used to populate the
//                  ip2bus_Mst_Addr(0:31) address bus during a Read or Write
//                  user logic master operation)
//    Byte Enable Register (C_BASEADDR + OFFSET + 0x8):
//       bit 0-15 - Master BE (This 16-bit value is used to populate the
//                  ip2bus_Mst_BE byte enable bus during a Read or Write user
//                  logic master operation for single data beat transfer)
//    Length Register      (C_BASEADDR + OFFSET + 0xC):
//       bit 0-3  - Reserved
//       bit 4-15 - Transfer Length (This 12-bit value is used to populate the
//                  ip2bus_Mst_Length(0:11) transfer length bus which specifies
//                  the number of bytes (1 to 4095) to transfer during user logic
//                  master Read or Write fixed length burst operations)
//    Go Register          (C_BASEADDR + OFFSET + 0xF):
//       bit 0-7  - Go Port (Write to this byte address initiates the user
//                  logic master transfer, data key value of 0x0A must be used)
// 
//    Note: OFFSET may be different depending on your address space configuration,
//          by default it's 0x100. Refer to IPIF address range array
//          for actual value.
// 
// Here's an example procedure in your software application to initiate a 64-byte
// read operation (single data beat) of this master model:
//    1. write 0x09 to the control register to perform write operation.
//    2. write the target address to the address register.
//    3. write valid byte lane value to the be register
//      - note: this value must be aligned with ip2bus address  
//    4. write 0x040 to the length register
//    5. write 0x0a to the go register, this will start the master write operation
//
// Here's an example procedure in your software application to initiate a 64-byte
// write operation (single data beat) of this master model:
//   1. write 0x0A to the control register to perform write operation.
//   2. write the target address to the address register
//   3. write valid byte lane value to the be register
//      - note: this value must be aligned with ip2bus address
//   4. write 0x0040 to the length register
//   5. write 0x0a to the go register, this will start the master write operation
// 
//----------------------------------------

  //CHANGE BELOW THIS LINE TO MODIFY MEMORY
  
  assign mst_reg_write_req = Bus2IP_WrCE[4] || Bus2IP_WrCE[5] || Bus2IP_WrCE[6] || Bus2IP_WrCE[7];
  assign mst_reg_read_req  = Bus2IP_RdCE[4] || Bus2IP_RdCE[5] || Bus2IP_RdCE[6] || Bus2IP_RdCE[7];
  assign mst_reg_write_sel = Bus2IP_WrCE[7 : 4];
  assign mst_reg_read_sel  = Bus2IP_RdCE[7 : 4];
  assign mst_write_ack     = mst_reg_write_req;
  assign mst_read_ack      = mst_reg_read_req;

  // rip control bits from master model registers
  always @(posedge Bus2IP_Clk or negedge Bus2IP_Resetn)
  begin
	  if (Bus2IP_Resetn == 1'b0)
	  begin
	    mst_cntl_rd_req = 1'b0;
	  end
	  else if (bus2ip_mst_cmdack == 1'b1 && bus2ip_mst_cmplt == 1'b0)
	  begin
	    mst_cntl_rd_req = 1'b0;
	  end
	  else
	  begin
	    mst_cntl_rd_req = 1'b1;
	  end
  end
  //assign mst_cntl_rd_req   = mst_reg[0][0]; // should be modified with the logic
  assign mst_cntl_wr_req   = 0;
  assign mst_cntl_bus_lock = 1'b0; // Ignored by Master Burst
  assign mst_cntl_burst    = 1'b1; // Fixed Length Burst
  //assign mst_ip2bus_addr   = slv_reg0;//FIXME: FROM DA SLAVE_REG;
  assign mst_ip2bus_be     = 0; // Only used for a single data beat
  assign mst_xfer_length = 128; // 128 bytes, 64 pixels
  
  
  //CHANGE ABOVE THIS LINE TO MODIFY MEMORY

  // implement byte write enable for each byte slice of the master model registers
  always @ (Bus2IP_BE or mst_reg_write_req or mst_reg_write_sel)
    begin
      for ( byte_index = 0; byte_index <= 15; byte_index = byte_index+1 )
        mst_byte_we[byte_index] <= mst_reg_write_req & mst_reg_write_sel[3 - (byte_index/BE_WIDTH)] & Bus2IP_BE[byte_index - ((byte_index/BE_WIDTH)*BE_WIDTH)];
    end // MASTER_REG_BYTE_WR_EN
  // implement master model registers
  // The master registers are written to initialize master transfer.
  always @ ( posedge Bus2IP_Clk)
    begin
    if (Bus2IP_Resetn == 1'b0 )
      begin
        for ( byte_index = 0; byte_index <= 14; byte_index = byte_index+1 ) 
          mst_reg[byte_index] <= 0;
      end
    else 
      begin 
          if ( mst_byte_we[0] == 1'b1 )
            begin
              mst_reg[0][7:0] <= Bus2IP_Data[7 : 0];
            end
          
    		 mst_reg[1][1] <= mst_cmd_sm_busy;  
      
          if (mst_byte_we[1] == 1'b1 )
          // allows a clear of the 'Done'/'error'/'timeout'
            begin
              mst_reg[1][0] <= Bus2IP_Data[(1-(1/BE_WIDTH)*BE_WIDTH)*8];
              mst_reg[1][2] <= Bus2IP_Data[(1-(1/BE_WIDTH)*BE_WIDTH)*8+2];
              mst_reg[1][3] <= Bus2IP_Data[(1-(1/BE_WIDTH)*BE_WIDTH)*8+3];
            end
        else
          // 'Done'/'error'/'timeout' from master control state machine
          begin
             mst_reg[1][0]  <= mst_cmd_sm_set_done | mst_reg[1][0];
             mst_reg[1][2]  <= mst_cmd_sm_set_error | mst_reg[1][2];
             mst_reg[1][3]  <= mst_cmd_sm_set_timeout | mst_reg[1][3];
           end
             // byte 2 and 3 are reserved
             // address register (byte 4 to 7)
             // be register (byte 8 to 9)
             // length register (byte 12 to 13)
             // byte 10, 11 and 14 are reserved
         for ( byte_index = 4; byte_index <= 14; byte_index = byte_index+1 )
           if ( mst_byte_we[byte_index] == 1'b1 )
             begin
               mst_reg[byte_index] <= Bus2IP_Data[(byte_index-(byte_index/BE_WIDTH)*BE_WIDTH)*8 +: 8];
             end
      end
    end // MASTER_REG_WRITE_PROC

  // implement master model register read mux
  always @ ( mst_reg_read_sel, mst_reg ) 
  begin

    case(mst_reg_read_sel)
      4'b1000:
        for(byte_index = 0; byte_index <= BE_WIDTH-1;byte_index = byte_index+1)
          mst_ip2bus_data[byte_index*8 +:8] <= mst_reg[byte_index];
      4'b0100:
        for(byte_index = 0; byte_index <= BE_WIDTH-1;byte_index = byte_index+1)
          mst_ip2bus_data[byte_index*8 +:8] <= mst_reg[BE_WIDTH+byte_index];
      4'b0010:
        for(byte_index = 0; byte_index <= BE_WIDTH-1;byte_index = byte_index+1)
          mst_ip2bus_data[byte_index*8 +:8] <= mst_reg[BE_WIDTH*2+byte_index];
      4'b0001:
        for(byte_index = 0; byte_index <= BE_WIDTH-1; byte_index = byte_index+1)
          if ( byte_index == (BE_WIDTH-1) ) 
            begin
              // go port is not readable
              mst_ip2bus_data[byte_index*8 +: 8] <= 8'b0;
            end
          else
            begin
              mst_ip2bus_data[byte_index*8 +: 8] <= mst_reg[BE_WIDTH*3+byte_index];
            end
        default : 
                   begin
                       mst_ip2bus_data <= 0;
                   end
      endcase
    end //MASTER_REG_READ_PROC	
  // user logic master command interface assignments
  assign ip2bus_mstrd_req  = mst_cmd_sm_rd_req;
  assign ip2bus_mstwr_req  = mst_cmd_sm_wr_req;
  assign ip2bus_mst_addr   = mst_cmd_sm_ip2bus_addr;
  assign ip2bus_mst_be     = mst_cmd_sm_ip2bus_be;
  assign ip2bus_mst_type   = mst_cmd_sm_xfer_type;
  assign ip2bus_mst_length = mst_cmd_sm_xfer_length;
  assign ip2bus_mst_lock   = mst_cmd_sm_bus_lock;
  assign ip2bus_mst_reset  = mst_cmd_sm_reset;

 //implement master command interface state machine
  always @ ( posedge Bus2IP_Clk)
    begin
      if (Bus2IP_Resetn == 1'b0 )
        begin
          // reset condition
          mst_cmd_sm_state          <= CMD_IDLE;
          mst_cmd_sm_clr_go         <= 0;
          mst_cmd_sm_rd_req         <= 0;
          mst_cmd_sm_wr_req         <= 0;
          mst_cmd_sm_bus_lock       <= 0;
          mst_cmd_sm_reset          <= 0;
          mst_cmd_sm_ip2bus_addr    <= 0;
          mst_cmd_sm_ip2bus_be      <= 0;
        mst_cmd_sm_xfer_type      <= 0;
        mst_cmd_sm_xfer_length    <= 0;
          mst_cmd_sm_set_done       <= 0;
          mst_cmd_sm_set_error      <= 0;
          mst_cmd_sm_set_timeout    <= 0;
          mst_cmd_sm_busy           <= 0;
          mst_cmd_sm_start_rd_llink <= 1'b0;
          mst_cmd_sm_start_wr_llink <= 1'b0;
        end
      else  
        begin
          // default condition
          mst_cmd_sm_clr_go         <= 0;
          mst_cmd_sm_rd_req         <= 0;
          mst_cmd_sm_wr_req         <= 0;
          mst_cmd_sm_bus_lock       <= 0;
          mst_cmd_sm_reset          <= 0;
          mst_cmd_sm_ip2bus_addr    <= 0;
          mst_cmd_sm_ip2bus_be      <= 0;
          mst_cmd_sm_xfer_type      <= 0;
          mst_cmd_sm_xfer_length    <= 0;
          mst_cmd_sm_set_done       <= 0;
          mst_cmd_sm_set_error      <= 0;
          mst_cmd_sm_set_timeout    <= 0;
          mst_cmd_sm_busy           <= 1'b1;
          mst_cmd_sm_start_rd_llink <= 1'b0;
          mst_cmd_sm_start_wr_llink <= 1'b0;
          // state transition
          case (mst_cmd_sm_state)

            CMD_IDLE:
               if ( mst_go == 1'b1 ) 
                 begin
                   // 'Go' register is triggerred.
                   mst_cmd_sm_state  <= CMD_RUN;
                   mst_cmd_sm_clr_go <= 1'b1;
                   mst_cmd_sm_busy   <= 1'b1;

                   if ( mst_cntl_rd_req == 1'b1 )
                     begin
                       // Master read local link
                       mst_cmd_sm_start_rd_llink <= 1'b1;
                     end
                   else if ( mst_cntl_wr_req == 1'b1 )
                     begin
    						  // Master write local link
                       mst_cmd_sm_start_wr_llink <= 1'b1;
                     end
                 end
               else
                 begin
                   mst_cmd_sm_state  <= CMD_IDLE;
                   mst_cmd_sm_busy   <= 1'b0;
                 end

            CMD_RUN:
               if ( bus2ip_mst_cmdack == 1'b1 && bus2ip_mst_cmplt == 1'b0 )
                 begin
                   mst_cmd_sm_state <= CMD_WAIT_FOR_DATA;
		           mst_cmd_sm_rd_req <= 1'b0;
                 end
               else if ( bus2ip_mst_cmplt == 1'b1 )
                 begin
    				    // Bus to Ip data reception complete
                   mst_cmd_sm_state <= CMD_DONE;

                   if ( bus2ip_mst_cmd_timeout == 1'b1)
                     begin
                     // AXI4LITE address phase timeout
                       mst_cmd_sm_set_error   <= 1'b1;
                       mst_cmd_sm_set_timeout <= 1'b1;
                     end
                   else if ( bus2ip_mst_error == 1'b1)
                     begin
                       mst_cmd_sm_set_error   <= 1'b1;
                     end
                   end
               else
                 begin
                   mst_cmd_sm_state       <= CMD_RUN;
                   mst_cmd_sm_rd_req      <= mst_cntl_rd_req;
                   mst_cmd_sm_wr_req      <= mst_cntl_wr_req;
                   mst_cmd_sm_ip2bus_addr <= mst_ip2bus_addr;
                   mst_cmd_sm_ip2bus_be   <= mst_ip2bus_be[15 : 16-C_MST_NATIVE_DATA_WIDTH/8 ];
                   mst_cmd_sm_xfer_type   <= mst_cntl_burst;
                   mst_cmd_sm_xfer_length <= mst_xfer_length;
                   mst_cmd_sm_bus_lock    <= mst_cntl_bus_lock;
                 end
 
            CMD_WAIT_FOR_DATA:
               if ( bus2ip_mst_cmplt == 1'b1 )
                 begin
                   mst_cmd_sm_state <= CMD_DONE;
                   if ( bus2ip_mst_cmd_timeout == 1'b1 )
                     begin
                       // AXI4LITE address phase timeout
                       mst_cmd_sm_set_error   <= 1'b1;
                       mst_cmd_sm_set_timeout <= 1'b1;
                     end
                   else if ( bus2ip_mst_error == 1'b1 )
                     begin
                       // AXI4LITE data transfer error
                       mst_cmd_sm_set_error   <= 1'b1;
                     end
                   end
               else
                 begin
                   mst_cmd_sm_state <= CMD_WAIT_FOR_DATA;
                 end

            CMD_DONE:
               begin
    				  // Completion of read/write transaction.
                 mst_cmd_sm_state    <= CMD_IDLE;
                 mst_cmd_sm_set_done <= 1'b1;
                 mst_cmd_sm_busy     <= 1'b0;
               end
            default :
               begin
                 mst_cmd_sm_state    <= CMD_IDLE;
                 mst_cmd_sm_busy     <= 1'b0;
               end
          endcase
        end	
    end //MASTER_CMD_SM_PROC

  // user logic master read locallink interface assignments
  assign ip2bus_mstrd_dst_rdy_n = !(mst_llrd_sm_dst_rdy),
         ip2bus_mstrd_dst_dsc_n = 1'b1; // do not throttle data
 // implement a simple state machine to enable the
 // read locallink interface to transfer data
 
 always @ ( posedge Bus2IP_Clk)
   begin
     if (Bus2IP_Resetn == 1'b0 )
       begin
       // reset condition
         mst_llrd_sm_state   <= LLRD_IDLE;
         mst_llrd_sm_dst_rdy <= 1'b0;
       end  
     else
       begin
         // default condition
         mst_llrd_sm_state   <= LLRD_IDLE;
         mst_llrd_sm_dst_rdy <= 1'b0;

         // state transition
         case (mst_llrd_sm_state)

             CMD_IDLE:
                 if ( mst_cmd_sm_start_rd_llink == 1'b1 )
                   begin
                     mst_llrd_sm_state <= LLRD_GO;
                   end
                 else
                   begin
                     mst_llrd_sm_state <= LLRD_IDLE;
                   end

             LLRD_GO:
                 // done, end of packet
                 if ( mst_llrd_sm_dst_rdy == 1'b1 && bus2ip_mstrd_src_rdy_n == 1'b0 &&
                      bus2ip_mstrd_eof_n == 1'b0 )
                   begin
                     mst_llrd_sm_state   <= LLRD_IDLE;
                   end
                 // not done yet, continue receiving data
                 else
                   begin
                     mst_llrd_sm_state   <= LLRD_GO;
                     mst_llrd_sm_dst_rdy <= 1'b1;
                   end

             default : 
                 begin
                   mst_llrd_sm_state <= LLRD_IDLE;
                 end
         endcase
       end	
   end // LLINK_RD_SM_PROCESS
 // user logic master write locallink interface assignments
 assign ip2bus_mstwr_src_rdy_n = !mst_llwr_sm_src_rdy,
        ip2bus_mstwr_src_dsc_n = 1'b1; // do not throttle data
 
 assign ip2bus_mstwr_rem       = 'b0;
 assign ip2bus_mstwr_sof_n     = !mst_llwr_sm_sof;
 assign ip2bus_mstwr_eof_n     = !mst_llwr_sm_eof;
  // implement a simple state machine to enable the
  // write locallink interface to transfer data
  
  always @ ( posedge Bus2IP_Clk)
    begin
      if (Bus2IP_Resetn == 1'b0 )
        begin
          // reset condition
          mst_llwr_sm_state   <= LLWR_IDLE;
          mst_llwr_sm_src_rdy <= 1'b0;
          mst_llwr_sm_sof     <= 1'b0;
          mst_llwr_sm_eof     <= 1'b0;
          mst_llwr_byte_cnt   <= 0;
        end  
      else
        begin
          // default condition
          mst_llwr_sm_state   <= LLWR_IDLE;
          mst_llwr_sm_src_rdy <= 1'b0;
          mst_llwr_sm_sof     <= 1'b0;
          mst_llwr_sm_eof     <= 1'b0;
          mst_llwr_byte_cnt   <= 0;

          // state transition
          case (mst_llwr_sm_state)

              LLWR_IDLE:
                  if ( mst_cmd_sm_start_wr_llink == 1'b1 && mst_cntl_burst == 1'b0 )
                    begin
                      mst_llwr_sm_state <= LLWR_SNGL_INIT;
                    end
                  else if ( mst_cmd_sm_start_wr_llink == 1'b1 && mst_cntl_burst == 1'b1 ) 
                    begin
                      mst_llwr_sm_state <= LLWR_BRST_INIT;
                    end  
                  else
                    begin
                      mst_llwr_sm_state <= LLWR_IDLE;
                    end
 
             LLWR_SNGL_INIT:
                  begin
                    mst_llwr_sm_state   <= LLWR_SNGL;
                    mst_llwr_sm_src_rdy <= 1'b1;
                    mst_llwr_sm_sof     <= 1'b1;
                    mst_llwr_sm_eof     <= 1'b1;
                  end 	

              LLWR_SNGL:
                  // destination discontinue write
                  if ( bus2ip_mstwr_dst_dsc_n == 1'b0 && bus2ip_mstwr_dst_rdy_n == 1'b0 )
                    begin
                      mst_llwr_sm_state   <= LLWR_IDLE;
                      mst_llwr_sm_src_rdy <= 1'b0;
                      mst_llwr_sm_eof     <= 1'b0;
                    end  
                  // single data beat transfer complete
                  else if ( mst_fifo_valid_read_xfer == 1'b1 )
                    begin
                      mst_llwr_sm_state   <= LLWR_IDLE;
                      mst_llwr_sm_src_rdy <= 1'b0;
                      mst_llwr_sm_sof     <= 1'b0;
                      mst_llwr_sm_eof     <= 1'b0;
                    end  
                  // wait on destination
                  else
                    begin
                      mst_llwr_sm_state   <= LLWR_SNGL;
                      mst_llwr_sm_src_rdy <= 1'b1;
                      mst_llwr_sm_sof     <= 1'b1;
                      mst_llwr_sm_eof     <= 1'b1;
                    end

              LLWR_BRST_INIT:
                  begin
                    mst_llwr_sm_state   <= LLWR_BRST;
                    mst_llwr_sm_src_rdy <= 1'b1;
                    mst_llwr_sm_sof     <= 1'b1;
                    mst_llwr_byte_cnt   <=  (mst_xfer_length); // convert to integer
                  end

              LLWR_BRST:
                  begin
                    if ( mst_fifo_valid_read_xfer == 1'b1)
                      begin
                        mst_llwr_sm_sof <= 1'b0;
                      end
                    else
                      begin
                        mst_llwr_sm_sof <= mst_llwr_sm_sof;
                      end
 
                    // destination discontinue write
                    if ( bus2ip_mstwr_dst_dsc_n == 1'b0 &&
                         bus2ip_mstwr_dst_rdy_n == 1'b0 )
                      begin 
                        mst_llwr_sm_state   <= LLWR_IDLE;
                        mst_llwr_sm_src_rdy <= 1'b1;
                        mst_llwr_sm_eof     <= 1'b1;
                      end  
                    // last data beat write
                    else if ( mst_fifo_valid_read_xfer == 1'b1 &&
                            ( mst_llwr_byte_cnt-BYTES_PER_BEAT) <= BYTES_PER_BEAT )
                      begin
                        mst_llwr_sm_state   <= LLWR_BRST_LAST_BEAT;
                        mst_llwr_sm_src_rdy <= 1'b1;
                        mst_llwr_sm_eof     <= 1'b1;
                      end  
                    // wait on destination
                    else
                      begin
                        mst_llwr_sm_state   <= LLWR_BRST;
                        mst_llwr_sm_src_rdy <= 1'b1;
                        // decrement write transfer counter if it's a valid write
                        if ( mst_fifo_valid_read_xfer == 1'b1 ) 
                          begin
                            mst_llwr_byte_cnt <= mst_llwr_byte_cnt - BYTES_PER_BEAT;
                          end
                        else
                          begin
                            mst_llwr_byte_cnt <= mst_llwr_byte_cnt;
                          end 	
                      end 
                  end
 
              LLWR_BRST_LAST_BEAT:
                  // destination discontinue write
                  if ( bus2ip_mstwr_dst_dsc_n == 1'b0 &&
                       bus2ip_mstwr_dst_rdy_n == 1'b0 ) 
                    begin		
                      mst_llwr_sm_state   <= LLWR_IDLE;
                      mst_llwr_sm_src_rdy <= 1'b0;
                    end  
                  // last data beat done
                  else if ( mst_fifo_valid_read_xfer == 1'b1 )
                    begin
                      mst_llwr_sm_state   <= LLWR_IDLE;
                      mst_llwr_sm_src_rdy <= 1'b0;
                    end  
                  // wait on destination
                  else
                    begin
                      mst_llwr_sm_state   <= LLWR_BRST_LAST_BEAT;
                      mst_llwr_sm_src_rdy <= 1'b1;
                      mst_llwr_sm_eof     <= 1'b1;
                    end	  
 
              default : 
                  begin
                    mst_llwr_sm_state <= LLWR_IDLE;
                  end
          endcase
        end	
    end // LLINK_WR_SM_PROC
  // local srl fifo for data storage
  assign mst_fifo_valid_write_xfer = !bus2ip_mstrd_src_rdy_n & mst_llrd_sm_dst_rdy;
  assign mst_fifo_valid_read_xfer  = ReadFromFIFO;
  assign fifo_Reset   = (~Bus2IP_Resetn) || (ul_state == UL_IDLE);
 
  // FIFO depth is 128 words. User can modify the depth based on their requirement.
   // srl_fifo_f #(
     // .C_DWIDTH(C_MST_NATIVE_DATA_WIDTH),
     // .C_DEPTH(240))
   // DATA_CAPTURE_FIFO_I (
     // .Clk(Bus2IP_Clk),
     // .Reset(Bus2IP_Resetn),
     // .FIFO_Write(mst_fifo_valid_write_xfer),
     // .Data_In(bus2ip_mstrd_d),
     // .FIFO_Read(mst_fifo_valid_read_xfer),
     // .Data_Out(ul_RGB_I),
     // .FIFO_Full(ul_FIFO_Full),
     // .FIFO_Empty(ul_FIFO_Empty),
     // .Addr()); // DATA_CAPTURE_FIFO_I
     

    FIFO_TFT INST_FIFO_TFT
    (
        .rst (fifo_Reset),
        .wr_clk (Bus2IP_Clk),
        .rd_clk (TFTClk),
        .din (bus2ip_mstrd_d),
        .wr_en (mst_fifo_valid_write_xfer),
        .rd_en (mst_fifo_valid_read_xfer),
        .dout (ul_RGB_I),
        .full (),
        .almost_full (),
        .empty (ul_FIFO_Empty),
        .almost_empty (),
		.prog_full(ul_FIFO_Full)
    );
	 
	 /*
	     rst : IN STD_LOGIC;
    wr_clk : IN STD_LOGIC;
    rd_clk : IN STD_LOGIC;
    din : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    wr_en : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    dout : OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
    full : OUT STD_LOGIC;
    almost_full : OUT STD_LOGIC;
    empty : OUT STD_LOGIC;
    almost_empty : OUT STD_LOGIC
	 */
 // If FIFO_Full stop reading from the memory
 
  // ------------------------------------------------------------
  // Example code to drive IP to Bus signals
  // ------------------------------------------------------------

  assign IP2Bus_Data = (slv_read_ack == 1'b1) ? slv_ip2bus_data : (mst_read_ack == 1'b1) ? mst_ip2bus_data :  0 ;

  assign IP2Bus_WrAck = slv_write_ack || mst_write_ack;
  assign IP2Bus_RdAck = slv_read_ack || mst_read_ack;
  assign IP2Bus_Error = 0;

  
  
  //DEBUG
    assign debug_ReadFromFIFO = ReadFromFIFO;
    assign debug_X_I = ul_X_I;
    assign debug_Y_I = ul_Y_I;
    
    //assign debug_TFTClk = TFTClk;
    assign debug_mst_fifo_valid_write_xfer = mst_fifo_valid_write_xfer;
    assign debug_mst_fifo_valid_read_xfer = mst_fifo_valid_read_xfer;
    assign debug_FIFO_Full = ul_FIFO_Full;
    assign debug_FIFO_Empty = ul_FIFO_Empty;
    assign debug_RGB_I = ul_RGB_I;
	assign debug_read_address = mst_ip2bus_addr;
	
	assign debug_ul_state = ul_state;
   assign debug_ul_msel = ul_MSEL;
   assign debug_ul_slv_reg0 = slv_reg0;
   assign debub_ul_slv_reg1 = slv_reg1[3:0];
   assign debug_mst_cmd_sm_set_done = mst_cmd_sm_set_done;
   assign debug_mst_cmd_sm_busy = mst_cmd_sm_busy;
   assign debug_bus2ip_mstrd_d = bus2ip_mstrd_d;
   assign debug_fifo_reset = fifo_Reset;

endmodule
