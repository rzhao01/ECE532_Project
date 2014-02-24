----------------------------------------------------------------------------------
-- Company: Digilent Ro
-- Engineer: Elod Gyorgy
-- 
-- Create Date:    14:35:21 02/23/2009 
-- Design Name: 
-- Module Name:    VideoTimingCtl - Behavioral 
-- Project Name:
-- Target Devices: 
-- Tool versions: 
-- Description: VideoTimingCtl generates the proper synchronization signals
-- according to the selected resolution.
--
-- Dependencies: digilent.Video
--
-- Revision: 
-- Revision 0.03 - Moved the Active Video area to the first part of the counter
-- Revision 0.02 - Added resolution 480x272 progressive
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;


---- Uncomment the following library declaration if instantiating
---- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

--type RESOLUTION is (R480_272P, R640_480P, R720_480P, R1280_720P, R1600_900P, R800_600P);

entity VideoTimingCtl is
	Port (
			  PCLK_I : in  STD_LOGIC; --variable depending on RSEL_I
           RST_I : in  STD_LOGIC; --reset
			  --RSEL_I : in RESOLUTION;
			  VDE_O : out STD_LOGIC; --data enable for pixel bus
			  HS_O : out STD_LOGIC;
			  VS_O : out STD_LOGIC;
			  HCNT_O : out NATURAL;
			  VCNT_O : out NATURAL);
end VideoTimingCtl;

architecture Behavioral of VideoTimingCtl is
----------------------------------------------------------------------------------
-- VGA Timing Signals
----------------------------------------------------------------------------------
constant H_MAX : NATURAL := 1600;
constant V_MAX : NATURAL := 900;

signal HCnt : NATURAL range 0 to H_MAX := 0; --horizontal counter
signal VCnt : NATURAL range 0 to V_MAX := 0; --vertical counter

signal H_AV, V_AV, H_AV_FP, V_AV_FP, H_AV_FP_S, V_AV_FP_S, H_AV_FP_S_BP, V_AV_FP_S_BP : NATURAL;

signal hs, vs: std_logic; -- horizontal/vertical sync
signal vde : std_logic; -- data enable
signal SRst : std_logic;

signal H_POL, V_POL : BOOLEAN;

----------------------------------------------------------------------------------
-- Timing Constants for 480x272 @60Hz
----------------------------------------------------------------------------------
--horizontal constants
constant H_480_272p_S : NATURAL := 45;		--sync
constant H_480_272p_FP : NATURAL := 0; 	--front porch
constant H_480_272p_AV : NATURAL := 480; 	--active video
constant H_480_272p_BP : NATURAL := 0;	--back porch
--vertical constants
constant V_480_272p_S : NATURAL := 16;		--sync
constant V_480_272p_FP : NATURAL := 0; 	--front porch
constant V_480_272p_AV : NATURAL := 272; 	--active video
constant V_480_272p_BP : NATURAL := 0;	--back porch

constant H_480_272p_AV_FP : NATURAL := H_480_272p_AV + H_480_272p_FP;
constant H_480_272p_AV_FP_S : NATURAL := H_480_272p_AV + H_480_272p_FP + H_480_272p_S;
constant H_480_272p_AV_FP_S_BP : NATURAL := H_480_272p_AV + H_480_272p_FP + H_480_272p_S + H_480_272p_BP;

constant V_480_272p_AV_FP : NATURAL := V_480_272p_AV + V_480_272p_FP;
constant V_480_272p_AV_FP_S : NATURAL := V_480_272p_AV + V_480_272p_FP + V_480_272p_S;
constant V_480_272p_AV_FP_S_BP : NATURAL := V_480_272p_AV + V_480_272p_FP + V_480_272p_S + V_480_272p_BP;

constant H_480_272p_POL : BOOLEAN := false; -- negative polarity
constant V_480_272p_POL : BOOLEAN := false; -- negative polarity

begin

----------------------------------------------------------------------------------
-- Resolution Selector
----------------------------------------------------------------------------------

H_AV <= 	H_480_272p_AV;
V_AV <= 	V_480_272p_AV;
H_AV_FP <= 	H_480_272p_AV_FP;
V_AV_FP <= 	V_480_272p_AV_FP;
H_AV_FP_S <= 	H_480_272p_AV_FP_S;			
V_AV_FP_S <= 	V_480_272p_AV_FP_S;
H_AV_FP_S_BP <=	    H_480_272p_AV_FP_S_BP;
V_AV_FP_S_BP <=		V_480_272p_AV_FP_S_BP;
H_POL <=		H_480_272p_POL;
V_POL <=		V_480_272p_POL;
				
----------------------------------------------------------------------------------
-- Local Reset
----------------------------------------------------------------------------------
Inst_LocalRst: entity work.LocalRst PORT MAP(
		RST_I => RST_I,
		CLK_I => PCLK_I,
		SRST_O => SRst
	);
----------------------------------------------------------------------------------
-- Video Timing Counter
----------------------------------------------------------------------------------
process (PCLK_I)
begin
	if Rising_Edge(PCLK_I) then
		if (SRst = '1') then
			HCnt <= H_AV_FP_S_BP - 1; -- 0 is an active pixel
			VCnt <= V_AV_FP_S_BP - 1;
			vde <= '0';
			hs <= '1';
			vs <= '1';
		else
			--pixel/line counters and video data enable
			if (HCnt = H_AV_FP_S_BP - 1) then
				HCnt <= 0;
				if (VCnt = V_AV_FP_S_BP - 1) then
					VCnt <= 0;
				else
					VCnt <= VCnt + 1;
				end if;
			else
				HCnt <= HCnt + 1;
			end if;
			
			--sync pulse in sync phase
			if (HCnt >= H_AV_FP-1) and (HCnt < H_AV_FP_S-1) then -- one cycle earlier (registered)
				hs <= '0';
				if (VCnt >= V_AV_FP) and (VCnt < V_AV_FP_S) then
					vs <= '0';
				else
					vs <= '1';
				end if;				
			else
				hs <= '1';
			end if;
			
			--video data enable
			if ((HCnt = H_AV_FP_S_BP - 1 and (VCnt = V_AV_FP_S_BP - 1 or VCnt < V_AV - 1)) or -- first pixel in frame
				 (HCnt < H_AV - 1 and VCnt < V_AV)) then
				vde <= '1';
			else
				vde <= '0';
			end if;
		end if;
	end if;
end process;

HCNT_O <= HCnt;
VCNT_O <= VCnt;
HS_O <= 	not hs	when H_POL else
			hs;
VS_O <= 	not vs	when V_POL else
			vs;
VDE_O <= vde;

end Behavioral;

