------------------------------------------------------------------------------
-- user_logic.vhd - entity/architecture pair
------------------------------------------------------------------------------
--
-- ***************************************************************************
-- ** Copyright (c) 1995-2012 Xilinx, Inc.  All rights reserved.            **
-- **                                                                       **
-- ** Xilinx, Inc.                                                          **
-- ** XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"         **
-- ** AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND       **
-- ** SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,        **
-- ** OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,        **
-- ** APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION           **
-- ** THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,     **
-- ** AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE      **
-- ** FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY              **
-- ** WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE               **
-- ** IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR        **
-- ** REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF       **
-- ** INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS       **
-- ** FOR A PARTICULAR PURPOSE.                                             **
-- **                                                                       **
-- ***************************************************************************
--
------------------------------------------------------------------------------
-- Filename:          user_logic.vhd
-- Version:           1.00.a
-- Description:       User logic.
-- Date:              Wed Feb 06 14:29:29 2013 (by Create and Import Peripheral Wizard)
-- VHDL Standard:     VHDL'93
------------------------------------------------------------------------------
-- Naming Conventions:
--   active low signals:                    "*_n"
--   clock signals:                         "clk", "clk_div#", "clk_#x"
--   reset signals:                         "rst", "rst_n"
--   generics:                              "C_*"
--   user defined types:                    "*_TYPE"
--   state machine next state:              "*_ns"
--   state machine current state:           "*_cs"
--   combinatorial signals:                 "*_com"
--   pipelined or register delay signals:   "*_d#"
--   counter signals:                       "*cnt*"
--   clock enable signals:                  "*_ce"
--   internal version of output port:       "*_i"
--   device pins:                           "*_pin"
--   ports:                                 "- Names begin with Uppercase"
--   processes:                             "*_PROCESS"
--   component instantiations:              "<ENTITY_>I_<#|FUNC>"
------------------------------------------------------------------------------

-- DO NOT EDIT BELOW THIS LINE --------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;

library proc_common_v3_00_a;
use proc_common_v3_00_a.proc_common_pkg.all;
use proc_common_v3_00_a.or_bits;

-- DO NOT EDIT ABOVE THIS LINE --------------------
--USER libraries added here

------------------------------------------------------------------------------
-- Entity section
------------------------------------------------------------------------------
-- Definition of Generics:
--   C_NUM_REG                    -- Number of software accessible registers
--   C_SLV_DWIDTH                 -- Slave interface data bus width
--
-- Definition of Ports:
--   Bus2IP_Clk                   -- Bus to IP clock
--   Bus2IP_Resetn                -- Bus to IP reset
--   Bus2IP_Data                  -- Bus to IP data bus
--   Bus2IP_BE                    -- Bus to IP byte enables
--   Bus2IP_RdCE                  -- Bus to IP read chip enable
--   Bus2IP_WrCE                  -- Bus to IP write chip enable
--   IP2Bus_Data                  -- IP to Bus data bus
--   IP2Bus_RdAck                 -- IP to Bus read transfer acknowledgement
--   IP2Bus_WrAck                 -- IP to Bus write transfer acknowledgement
--   IP2Bus_Error                 -- IP to Bus error response
------------------------------------------------------------------------------

entity user_logic is
  generic
  (
    -- ADD USER GENERICS BELOW THIS LINE ---------------
    --USER generics added here
    -- ADD USER GENERICS ABOVE THIS LINE ---------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol parameters, do not add to or delete
    C_NUM_REG                      : integer              := 1;
    C_SLV_DWIDTH                   : integer              := 32
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );
  port
  (
    -- ADD USER PORTS BELOW THIS LINE ------------------
    --USER ports added here
    TP_CS_O : out  STD_LOGIC;
    TP_DIN_O : out  STD_LOGIC;
    TP_DOUT_I : in  STD_LOGIC;
    TP_DCLK_O : out  STD_LOGIC;
    TP_BUSY_I : in  STD_LOGIC;
    TP_PENIRQ_I : in STD_LOGIC;
    CLK_I : in STD_LOGIC;
    --BTNM_I : in STD_LOGIC;
    
    
    TouchEn_O : out STD_LOGIC;
    TOUCH_X : buffer std_logic_vector(8 downto 0);
    TOUCH_Y : buffer std_logic_vector(8 downto 0);
    
    debugTouchX : out std_logic_vector(11 downto 0);
    
    debug_slv_reg_read_sel : out std_logic;
    debug_slv_ip2bus_data  : out std_logic_vector(C_SLV_DWIDTH-1 downto 0);
      
      
    -- ADD USER PORTS ABOVE THIS LINE ------------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol ports, do not add to or delete
    Bus2IP_Clk                     : in  std_logic;
    Bus2IP_Resetn                  : in  std_logic;
    Bus2IP_Data                    : in  std_logic_vector(C_SLV_DWIDTH-1 downto 0);
    Bus2IP_BE                      : in  std_logic_vector(C_SLV_DWIDTH/8-1 downto 0);
    Bus2IP_RdCE                    : in  std_logic_vector(C_NUM_REG-1 downto 0);
    Bus2IP_WrCE                    : in  std_logic_vector(C_NUM_REG-1 downto 0);
    IP2Bus_Data                    : out std_logic_vector(C_SLV_DWIDTH-1 downto 0);
    IP2Bus_RdAck                   : out std_logic;
    IP2Bus_WrAck                   : out std_logic;
    IP2Bus_Error                   : out std_logic
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );

  attribute MAX_FANOUT : string;
  attribute SIGIS : string;

  attribute SIGIS of Bus2IP_Clk    : signal is "CLK";
  attribute SIGIS of Bus2IP_Resetn : signal is "RST";

  
  
end entity user_logic;


------------------------------------------------------------------------------
-- Architecture section
------------------------------------------------------------------------------

architecture IMP of user_logic is

  --USER signal declarations added here, as needed for user logic
    constant Z_high : natural := 1500;
    constant Z_low : natural := 200;
    constant SCALING_FACTOR : natural := 12;
    constant H_480_272p_AV : NATURAL := 480;      --active video
    constant V_480_272p_AV : NATURAL := 272;      --active video
   
   
   type Coordinate is
		record
			X : natural;
			Y : natural;
		end record;
  
	type PanelCharacteristics is
		record
        TopLeft, TopRight, BottomLeft, BottomRight : Coordinate;
        XPlateR, YPlateR : natural;
    end record;
   constant VmodTFT : PanelCharacteristics := (
		TopLeft => (X => 150, Y => 300),
		TopRight => (X => 3950, Y => 300),
		BottomLeft => (X => 150, Y => 3800),
		BottomRight => (X => 3950, Y => 3800),
		XPlateR => 900,
		YPlateR => 245
	);
    
    constant X_SCALING : natural := natural(real(517.28947));
    constant Y_SCALING : natural := natural(real(318.31771)); 

    signal TouchXNorm1, TouchYNorm1 : std_logic_vector(11 downto 0);
    signal TouchXNorm2, TouchYNorm2 : std_logic_vector(11+SCALING_FACTOR downto 0);
    signal TouchXNorm : std_logic_vector(11 downto 0);
    signal TouchYNorm : std_logic_vector(11 downto 0);
    signal TouchEn : std_logic;

    signal TFTClk, TFTClk180, SysRst, SysClk : std_logic;
	signal TFTMSel : std_logic_vector(3 downto 0);
    signal SW_I : std_logic_vector(7 downto 0);

    signal TouchX, TouchY, TouchZ : std_logic_vector(11 downto 0);
    signal timer_active: std_logic; 
    signal interrupt_en: std_logic; 
    signal debouncing_en: std_logic; 
    attribute KEEP : string;
    attribute KEEP of TouchXNorm: signal is "TRUE";
    attribute KEEP of TouchYNorm: signal is "TRUE";

  ------------------------------------------
  -- Signals for user logic slave model s/w accessible register example
  ------------------------------------------
  signal slv_reg0                       : std_logic_vector(C_SLV_DWIDTH-1 downto 0);
  signal slv_reg1                       : std_logic_vector(C_SLV_DWIDTH-1 downto 0);
  signal slv_reg2                       : std_logic_vector(C_SLV_DWIDTH-1 downto 0);
  signal slv_reg_write_sel              : std_logic_vector(C_NUM_REG-1 downto 0);
  signal slv_reg_read_sel               : std_logic_vector(C_NUM_REG-1 downto 0);
  signal slv_ip2bus_data                : std_logic_vector(C_SLV_DWIDTH-1 downto 0);
  signal slv_read_ack                   : std_logic;
  signal slv_write_ack                  : std_logic;
  signal timer_reg                      : std_logic_vector(C_SLV_DWIDTH-1 downto 0); 
  signal counter_edge: std_logic; 
  signal Bus2IP_Reset : std_logic;

begin

  --USER logic implementation added here
----------------------------------------------------------------------------------
-- Touch Controller
----------------------------------------------------------------------------------		
Bus2IP_Reset <= not Bus2IP_Resetn;
    -- MIGHT NEED TO CHANGE RST_I
    Inst_TouchCtrl: entity work.TouchCtrl GENERIC MAP (CLOCKFREQ => 100) PORT MAP(
		--CLK_I => SysClk,
        CLK_I => CLK_I,
		--RST_I => SysRst,
        RST_I => Bus2IP_Reset,    
		X_O => TouchX,
		Y_O => TouchY,
		Z_O => TouchZ,
		PENIRQ_I => TP_PENIRQ_I,
		CS_O => TP_CS_O,
		DIN_O => TP_DIN_O,
		DOUT_I => TP_DOUT_I,
		DCLK_O => TP_DCLK_O,
		BUSY_I => TP_BUSY_I
	);

    
----------------------------------------------------------------------------------
-- Process Touch Data
----------------------------------------------------------------------------------		
	
    TouchXNorm1 <= (TouchX - VmodTFT.TopLeft.X);
	TouchYNorm1 <= (TouchY - VmodTFT.TopLeft.Y);
	process (CLK_I) 
	begin 
		if Rising_Edge(CLK_I) then
			TouchXNorm2 <= TouchXNorm1 * std_logic_vector(to_unsigned(X_SCALING, 12));  -- 12 was SCALING_FACTOR
			TouchYNorm2 <= TouchYNorm1 * std_logic_vector(to_unsigned(Y_SCALING, 12));  -- 12 bits
			if TouchZ < Z_high and TouchZ > Z_low and timer_active = '0' then
				TouchEn <= '1';
			else
				TouchEn <= '0';
			end if;
		end if;
	end process;
	TouchXNorm <= TouchXNorm2(TouchXNorm2'high downto 12);  -- 12 was SCALING_FACTOR
	TouchYNorm <= TouchYNorm2(TouchYNorm2'high downto 12);  -- USED TO BE CONV_INTEGER
                                                                          -- 'high gives MSB
    TOUCH_X <= TouchXNorm (8 downto 0);
    TOUCH_Y <= TouchYNorm (8 downto 0);

    TOUCH_EN_PROC : process (interrupt_en, debouncing_en, timer_active, TouchEn, counter_edge) is 
    begin
	    if interrupt_en = '0' then 
		    TouchEn_O <= '0'; 
	    elsif debouncing_en = '1' then 
		    if counter_edge = '1' then
			    TouchEn_O <= '1'; 
		    elsif timer_active = '0' then 
			    TouchEn_O <= TouchEn;
		    else 
			    TouchEn_O <= '0'; 
		    end if;
	    else  
		    TouchEn_O <= TouchEn; 
	    end if;   
    end process; 
    
  ------------------------------------------
  -- Example code to read/write user logic slave model s/w accessible registers
  -- 
  -- Note:
  -- The example code presented here is to show you one way of reading/writing
  -- software accessible registers implemented in the user logic slave model.
  -- Each bit of the Bus2IP_WrCE/Bus2IP_RdCE signals is configured to correspond
  -- to one software accessible register by the top level template. For example,
  -- if you have four 32 bit software accessible registers in the user logic,
  -- you are basically operating on the following memory mapped registers:
  -- 
  --    Bus2IP_WrCE/Bus2IP_RdCE   Memory Mapped Register
  --                     "1000"   C_BASEADDR + 0x0
  --                     "0100"   C_BASEADDR + 0x4
  --                     "0010"   C_BASEADDR + 0x8
  --                     "0001"   C_BASEADDR + 0xC
  -- 
  ------------------------------------------
  slv_reg_write_sel <= Bus2IP_WrCE(C_NUM_REG-1 downto 0);
  slv_reg_read_sel  <= Bus2IP_RdCE(C_NUM_REG-1 downto 0);
  
  COUNTER_EDGE_PROC : process (timer_active, timer_reg) is 
  begin
	  if timer_reg(31 downto 0) = "00000000000000000000000000000001" and timer_active = '0' then 
		  counter_edge <= '1'; 
	  else
		  counter_edge <= '0'; 
	  end if; 
  end process;


  TIMER_ACTIVE_PROC : process (slv_reg1) is 
  begin
	  if slv_reg1(31 downto 0) = "00000000000000000000000000000000" then 
		  timer_active <= '0'; 
	  else
		  timer_active <= '1'; 
	  end if; 
  end process;

  slv_read_ack <= Bus2IP_RdCE(0) or Bus2IP_RdCE(1) or Bus2IP_RdCE(2);	
  slv_write_ack <= Bus2IP_WrCE(0) or Bus2IP_WrCE(1) or Bus2IP_WrCE(2); 
  interrupt_en <= slv_reg2 (0); 
  debouncing_en <= slv_reg2 (1);

  -- implement slave model software accessible register(s)
  SLAVE_REG_WRITE_PROC : process( Bus2IP_Clk ) is
  begin

	  if Bus2IP_Clk'event and Bus2IP_Clk = '1' then
		  if Bus2IP_Resetn = '0' then
			  slv_reg0 <= (others => '0');
		  else  
			  slv_reg0(8 downto 0)			<=	TOUCH_Y (8 downto 0);
			  slv_reg0(24 downto 16)                <=	TOUCH_X (8 downto 0);
			  slv_reg0(31) <= TouchEn;
		  end if;
	  end if;

	  if Bus2IP_Clk'event and Bus2IP_Clk = '1' then
		  if Bus2IP_Resetn = '0' then
			  slv_reg1 <= (others => '0');
		  elsif slv_reg_write_sel = "010" and timer_active = '0' then	
			  slv_reg1(31 downto 0)	  <= Bus2IP_Data (31 downto 0);
		  elsif timer_active = '1' then 
			  slv_reg1(31 downto 0) <= slv_reg1(31 downto 0) - 1;
		  else 
			  slv_reg1(31 downto 0) <= slv_reg1(31 downto 0); 
		  end if;
	  end if;

	  if Bus2IP_Clk'event and Bus2IP_Clk = '1' then
		  if Bus2IP_Resetn = '0' then
			  slv_reg2 <= (others => '0');
		  elsif slv_reg_write_sel = "001" then
			  slv_reg2(1 downto 0)	<= Bus2IP_Data (1 downto 0);
			  slv_reg2(31 downto 2) <= (others => '0');
		  else 
			  slv_reg2(31 downto 0) <= slv_reg2(31 downto 0); 
		  end if;
	  end if;

  end process SLAVE_REG_WRITE_PROC;

  -- implement slave model software accessible register(s) read mux
  SLAVE_REG_READ_PROC : process( slv_reg_read_sel, slv_reg0, slv_reg1, slv_reg2) is
  begin

	  if slv_reg_read_sel = "100" then 
		  slv_ip2bus_data <= slv_reg0;
	  elsif slv_reg_read_sel = "010" then 
		  slv_ip2bus_data <= slv_reg1;
	  elsif slv_reg_read_sel = "001" then 
		  slv_ip2bus_data <= slv_reg2;
	  else 
		  slv_ip2bus_data <= (others => '0'); 
	  end if; 
  end process SLAVE_REG_READ_PROC;

--  SLAVE_REG_READ_PROC : process( slv_reg_read_sel, slv_reg0, slv_reg1, slv_reg2) is
--  begin
--
--    case slv_reg_read_sel is
--      when "100" => slv_ip2bus_data <= slv_reg0;
--      when "010" => slv_ip2bus_data <= slv_reg1; 
--      when "001" => slv_ip2bus_data <= slv_reg2; 
--      when others => slv_ip2bus_data <= (others => '0');
--    end case;
--
--  end process SLAVE_REG_READ_PROC;

  ------------------------------------------
  -- Example code to drive IP to Bus signals
  ------------------------------------------
  IP2Bus_Data  <= slv_ip2bus_data when slv_read_ack = '1' else
                  (others => '0');

  IP2Bus_WrAck <= slv_write_ack;
  IP2Bus_RdAck <= slv_read_ack;
  IP2Bus_Error <= '0';
  
  
  debugTouchX <= slv_reg0 (11 downto 0);
  
  debug_slv_reg_read_sel <= slv_reg_read_sel (0);
  debug_slv_ip2bus_data  <= slv_ip2bus_data;

end IMP;
