----------------------------------------------------------------------------------
-- Company: Digilent Ro
-- Engineer: Elod Gyorgy
-- 
-- Create Date:    14:07:24 04/11/2011 
-- Design Name: 
-- Module Name:    TFTCtl - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: TFTCtl drives the AT043TN24 V.7 TFT LCD on the VmodTFT. A frame
-- buffer of 480x272 and 2-bit color depth is used. The 2-bits per pixel encode
-- luminousity, 00b being black and 11b being the brightest pixel possible. To
-- demonstrate color
--
-- Dependencies: digilent.Video
--
-- Revision: 
-- Revision 1.00 - 08/10/2011 - Made "Power Down" the start-up state of the
--	sequencer FSM.
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
--use IEEE.STD_LOGIC_ARITH.ALL;

--use IEEE.math_real.all;
--use IEEE.STD_LOGIC_UNSIGNED.ALL;
use ieee.numeric_std.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;


-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
library UNISIM;
use UNISIM.VComponents.all;

entity TFTCtl is
  generic
  (
    -- ADD USER GENERICS BELOW THIS LINE ---------------
    --USER generics added here
    -- ADD USER GENERICS ABOVE THIS LINE ---------------

    -- DO NOT EDIT BELOW THIS LINE ---------------------
    -- Bus protocol parameters, do not add to or delete
    C_MST_NATIVE_DATA_WIDTH        : integer              := 32;
    C_LENGTH_WIDTH                 : integer              := 12;
    C_MST_AWIDTH                   : integer              := 32;
    C_NUM_REG                      : integer              := 5;
    C_SLV_DWIDTH                   : integer              := 32
    -- DO NOT EDIT ABOVE THIS LINE ---------------------
  );
   Port (
		CLK_I : in STD_LOGIC;
		CLK_180_I : in STD_LOGIC;
		RST_I : in STD_LOGIC;
		
		X_I: in std_logic_vector (31 downto 0);-- range 0 to 480;
		Y_I: in std_logic_vector (31 downto 0);-- range 0 to 272;
		
		RGB_I: in  std_logic_vector(15 downto 0);
		WE_I : in std_logic;
		WR_CLK : in std_logic;
		
		ReadFromFIFO : out STD_LOGIC;
		
		R_O : out  STD_LOGIC_VECTOR (7 downto 0);
		G_O : out  STD_LOGIC_VECTOR (7 downto 0);
		B_O : out  STD_LOGIC_VECTOR (7 downto 0);
		DE_O : out  STD_LOGIC;
		CLK_O : out  STD_LOGIC;
		DISP_O : out  STD_LOGIC;
		BKLT_O : out  STD_LOGIC; --PWM backlight control
		VDDEN_O : out STD_LOGIC;
		
		MSEL_I : in STD_LOGIC_VECTOR(3 downto 0); -- Mode selection
        
        debug_vram_data : out std_logic_vector (15 downto 0);
        debug_reg_wrdata: out std_logic_vector (15 downto 0)
	);
end TFTCtl;

architecture Behavioral of TFTCtl is
	signal X_I_int : integer;
	signal Y_I_int : integer;


	constant CLOCKFREQ : natural := 8; --MHZ
	constant TPOWERUP : natural := 1; --ms
	constant TPOWERDOWN : natural := 1; --ms
	constant TLEDWARMUP : natural := 200; --ms
	constant TLEDCOOLDOWN : natural := 200; --ms
	constant TLEDWARMUP_CYCLES : natural := natural(CLOCKFREQ*TLEDWARMUP*1000);
	constant TLEDCOOLDOWN_CYCLES : natural := natural(CLOCKFREQ*TLEDCOOLDOWN*1000);
	constant TPOWERUP_CYCLES : natural := natural(CLOCKFREQ*TPOWERUP*1000);
	constant TPOWERDOWN_CYCLES : natural := natural(CLOCKFREQ*TPOWERDOWN*1000);	

	signal waitCnt : natural range 0 to TLEDCOOLDOWN_CYCLES := 0;
	signal waitCntEn : std_logic;

   type state_type is (stOff, stPowerUp, stLEDWarmup, stLEDCooldown, stPowerDown, stOn); 
   signal state, nstate : state_type := stPowerDown;

	signal cntDyn: integer range 0 to 2**28-1;
	signal VtcHCnt: integer;
	signal VtcVCnt: integer;
	signal VtcRst, VtcVde, VtcHs, VtcVs : std_logic;
	signal int_Bklt, int_De, clkStop : std_logic := '0';
	signal int_R, int_G, int_B : std_logic_vector(7 downto 0);
	
	signal reg_X, reg_Y : natural;
	signal reg_WE : std_logic;

	--video ROM data and address bus
	constant FB_COLOR_DEPTH : natural := 16;
    constant H_480_272p_AV : NATURAL := 480;      --active video
    constant V_480_272p_AV : NATURAL := 272;      --active video

	signal vram_data: std_logic_vector (FB_COLOR_DEPTH-1 downto 0);
	--signal vram_addr, vram_wraddr : INTEGER range 0 to H_480_272p_AV*V_480_272p_AV-1;
	--type vramt is array (0 to H_480_272p_AV*V_480_272p_AV-1) of 
	--	std_logic_vector (FB_COLOR_DEPTH-1 downto 0);
	--signal vram : vramt := (others => (others => '0'));
	--signal vram_we : std_logic;
	--attribute RAM_STYLE : string;
	--attribute RAM_STYLE of vram: signal is "BLOCK";
	
	signal currentRGB: std_logic_vector ((C_MST_NATIVE_DATA_WIDTH/2)-1 downto 0);
	signal nextRGB: std_logic_vector ((C_MST_NATIVE_DATA_WIDTH/2)-1 downto 0);
	
	--signal ReadFromFIFO : std_logic;
	
	type state_fifo is (fifoGoing, fifoWait); 
	signal fifo_cs : state_fifo := fifoWait;
	
begin

    X_I_int <= to_integer(unsigned(X_I));
    Y_I_int <= to_integer(unsigned(Y_I));

----------------------------------------------------------------------------------
-- Video Timing Controller
-- Generates horizontal and vertical sync and video data enable signals.
----------------------------------------------------------------------------------
	Inst_VideoTimingCtl: entity work.VideoTimingCtl PORT MAP (
		PCLK_I => CLK_I,
		--RSEL_I => R480_272P, --VmodTFT Resolution only
		RST_I => VtcRst,
		VDE_O => VtcVde,
		HS_O => VtcHs,
		VS_O => VtcVs,
		HCNT_O => VtcHCnt,
		VCNT_O => VtcVCnt
	);
	--VtcRst <= '0';
	VtcRst<= RST_I;
	
----------------------------------------------------------------------------------
-- VRAM address counter
----------------------------------------------------------------------------------
-- READ
int_De <= VtcVde;
--ReadFromFIFO <= int_De;
ReadFromFIFO <= '0' when state = stOff or state = stPowerUp or state = stPowerDown or fifo_cs = fifoWait else
				int_De;
process (CLK_I)
begin
	if Rising_Edge(CLK_I) then
		if (fifo_cs = fifoWait) then
			if (state = stOn and VtcHs = '0' and VtcVs = '0' and RST_I = '0') then
				fifo_cs <= fifoGoing;
			else
				fifo_cs <= fifoWait;
			end if;
		end if;
		
		if (fifo_cs = fifoGoing) then
			if (RST_I = '1' or state /= stOn) then
				fifo_cs <= fifoWait;
			else
				fifo_cs <= fifoGoing;
			end if;
		end if;
		--delay DE to account for VRAM registered output delay
		--int_De <= VtcVde;
		
		-- if (VtcRst = '1') then
			-- vram_addr <= 0;
		-- else
			-- if (VtcVde = '1') then
				-- if (vram_addr = H_480_272p_AV*V_480_272p_AV-1) then
					-- vram_addr <= 0;
				-- else
					-- vram_addr <= vram_addr + 1;
				-- end if;
			-- end if;
		-- end if;
	end if;
end process;

-- WRITE ADD
-- process (WR_CLK)
-- begin
   -- if Rising_Edge(WR_CLK) then
		-- if ( X_I_int >= 0 and X_I_int < H_480_272p_AV and
			 -- Y_I_int >= 0 and Y_I_int < V_480_272p_AV) then
			-- vram_we <= 	WE_I;
		-- else
			-- vram_we <= '0';
		-- end if;
		-- vram_wraddr <= (Y_I_int)*H_480_272p_AV + X_I_int;
	
		-- if (WE_I = '1') then
            -- if ((X_I_int mod 2) = 0) then
                -- reg_wrdata <= RGB_I (31 downto 16);
            ----ReadFromFIFO <= 1;
            -- else 
                -- reg_wrdata <= RGB_I (15 downto 0);
                ----ReadFromFIFO <= 0;
            -- end if;
		-- end if;
	
   -- end if;
-- end process;

----------------------------------------------------------------------------------
-- VRAM registered output
----------------------------------------------------------------------------------
process (CLK_I)
begin
	if Rising_Edge(CLK_I) then
        vram_data <= RGB_I (15 downto 0);
		if (int_De = '1') then
			--ReadFromFIFO <= '1';
			--vram_data <= RGB_I (15 downto 0);
        else
            --ReadFromFIFO <= '0';
        end if;
        
	end if;
end process;

-- process (WR_CLK)
-- begin
   -- if Rising_Edge(WR_CLK) then
      -- if (vram_we = '1') then
			-- vram(vram_wraddr) <= reg_wrdata;
		-- end if;
   -- end if;
-- end process;


----------------------------------------------------------------------------------
-- Screen data, convert from RGB565 to RGB888  NOTE: Data is stored in VRAM as 565
----------------------------------------------------------------------------------
int_R <= vram_data( 15 downto 11) & "000";
int_G <= vram_data( 10 downto 5 ) & "00";
int_B <= vram_data( 4  downto 0 ) & "000";

----------------------------------------------------------------------------------
-- Backlight intensity control
----------------------------------------------------------------------------------	
	Inst_PWM: entity work.PWM 
	generic map (
		C_CLK_I_FREQUENCY => CLOCKFREQ, -- in MHZ
		C_PWM_FREQUENCY => 25000, -- in Hz
		C_PWM_RESOLUTION => 3
	)
	PORT MAP(
		CLK_I => CLK_I,
		--RST_I => '0',
		RST_I => RST_I,
		PWM_O => int_Bklt,
		DUTY_FACTOR_I => MSEL_I(2 downto 0)
	);

----------------------------------------------------------------------------------
-- LCD Power Sequence
----------------------------------------------------------------------------------	
--LCD & backlight power 
VDDEN_O <= 	'0' when state = stOff or state = stPowerDown else
				'1';
--Display On/Off signal
DISP_O <= 	'0' when state = stOff or state = stPowerUp or state = stPowerDown else
				'1';
--Interface signals
DE_O <= 		'0' when state = stOff or state = stPowerUp or state = stPowerDown else
				int_De;
R_O <= 		(others => '0') when state = stOff or state = stPowerUp or state = stPowerDown else
				int_R;
G_O <= 		(others => '0') when state = stOff or state = stPowerUp or state = stPowerDown else
				int_G;
B_O <= 		(others => '0') when state = stOff or state = stPowerUp or state = stPowerDown else
				int_B;
--Clock signal
clkStop <= 	'1' when state = stOff or state = stPowerUp or state = stPowerDown else
				'0';
--Backlight adjust/enable
BKLT_O <= 	int_Bklt when state = stOn else
				'0';
--Wait States
waitCntEn <= 	'1' when (state = stPowerUp or state = stLEDWarmup or state = stLEDCooldown or state = stPowerDown) and 
								(state = nstate) else
					'0';
					
   SYNC_PROC: process (CLK_I)
   begin
      if (CLK_I'event and CLK_I = '1') then
         state <= nstate;
      end if;
   end process;				

   NEXT_STATE_DECODE: process (state, waitCnt, MSEL_I)
   begin
      nstate <= state;
      case (state) is
         when stOff =>
            if (MSEL_I(3) = '1' and RST_I = '0') then
               nstate <= stPowerUp;
            end if;
			when stPowerUp => --turn power on first
				if (waitCnt = TPOWERUP_CYCLES) then
               nstate <= stLEDWarmup;
            end if;
         when stLEDWarmup => --turn on interface signals
            if (waitCnt = TLEDWARMUP_CYCLES) then
               nstate <= stOn;
            end if;
         when stOn => --turn on backlight too
				if (MSEL_I(3) = '0' or RST_I = '1') then
					nstate <= stLEDCooldown;
				end if;
			when stLEDCooldown =>
            if (waitCnt = TLEDCOOLDOWN_CYCLES) then
               nstate <= stPowerDown;
            end if;
			when stPowerDown => --turn off power last
				if (waitCnt = TPOWERDOWN_CYCLES) then
					nstate <= stOff;
				end if;
      end case;      
   end process;
----------------------------------------------------------------------------------
-- Wait Counter
----------------------------------------------------------------------------------	
	process(CLK_I)
	begin
		if Rising_Edge(CLK_I) then
			if waitCntEn = '0' then
				waitCnt <= 0;
			else
				waitCnt <= waitCnt + 1;
			end if;
		end if;
	end process;

----------------------------------------------------------------------------------
-- Clock Forwarding done right
----------------------------------------------------------------------------------
	Inst_ODDR2_MCLK_FORWARD : ODDR2
   generic map(
      DDR_ALIGNMENT => "NONE", -- Sets output alignment to "NONE", "C0", "C1" 
      INIT => '0', -- Sets initial state of the Q output to '0' or '1'
      SRTYPE => "SYNC") -- Specifies "SYNC" or "ASYNC" set/reset
   port map (
      Q => CLK_O, -- 1-bit output data
      C0 => CLK_I, -- 1-bit clock input
      C1 => CLK_180_I, -- 1-bit clock input
      CE => '1',  -- 1-bit clock enable input
      D0 => '1',   -- 1-bit data input (associated with C0)
      D1 => '0',   -- 1-bit data input (associated with C1)
      R => clkStop, -- 1-bit clock reset
      S => '0'     -- 1-bit set input
   );	
   
   
    --debug_vram_data <= vram_data;
    --debug_reg_wrdata<= reg_wrdata;
end Behavioral;

