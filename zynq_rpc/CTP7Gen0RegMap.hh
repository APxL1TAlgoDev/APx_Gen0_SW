#ifndef CTP7HLSRegMap_hh
#define CTP7HLSRegMap_hh

#include <stdint.h>

namespace CTP7Gen0RegMap {

	/////////////////////////////////////////////////////////////////////
	//
	// Common Register File
	//
	/////////////////////////////////////////////////////////////////////

  	 const int C_CH_to_CH_ADDR_OFFSET = 4 * 64;	

  	 const int C_LINK_STAT_CH0_ADDR         = 0x62000000;
  	 const int C_BXID_LINK_OFFSET_CH0_ADDR  = 0x62000004;
  	 const int C_LINK_MASK_CH0_ADDR         = 0x62000008;

  	 const int C_TTC_MMCM_RST           = 0x62010000;
  	 const int C_TTC_MMCM_LOCKED        = 0x62010004;
  	 const int C_TTC_MMCM_PHASE_SHIFT   = 0x62010008;
  	 const int C_TTC_STAT_RST           = 0x6201000c;
 	 const int C_TTC_BX0_LOCKED         = 0x62010010;
  	 const int C_TTC_BX0_ERR            = 0x62010014;
 	 const int C_TTC_BX0_UNLOCKED_CNT   = 0x62010018;
 	 const int C_TTC_BX0_UDF_CNT        = 0x6201001c;
 	 const int C_TTC_BX0_OVF_CNT        = 0x62010020;
 	 const int C_TTC_DEC_SINGLE_ERR_CNT = 0x62010024;
 	 const int C_TTC_DEC_DOUBLE_ERR_CNT = 0x62010028;
 	 const int C_TTC_BX0_CMD            = 0x6201002c;
 	 const int C_TTC_EC0_CMD            = 0x62010030;
 	 const int C_TTC_RESYNC_CMD         = 0x62010034;
 	 const int C_TTC_OC0_CMD            = 0x62010038;
 	 const int C_TTC_TEST_SYNC_CMD      = 0x6201003c;
 	 const int C_TTC_START_CMD          = 0x62010040;
 	 const int C_TTC_STOP_CMD           = 0x62010044;
 	 const int C_TTC_ORBIT_DELAY        = 0x62010048;
    	 const int C_TTC_L1A_ENABLE         = 0x6201004c;
 	 const int C_TTC_L1A_CNT            = 0x62010050;
 	 const int C_TTC_BX0_CNT            = 0x62010054;
  	 const int C_TTC_EC0_CNT            = 0x62010058;
 	 const int C_TTC_RESYNC_CNT         = 0x6201005c;
  	 const int C_TTC_OC0_CNT            = 0x62010060;
 	 const int C_TTC_TEST_SYNC_CNT      = 0x62010064;
 	 const int C_TTC_START_CNT          = 0x62010068;
 	 const int C_TTC_STOP_CNT           = 0x6201006c;
  	 const int C_TTC_L1ID_CNT           = 0x62010070;
  	 const int C_TTC_ORBIT_CNT          = 0x62010074;

 	 const int C_LINK_ALIGN_REQ         = 0x6201A000;
 	 const int C_LINK_ALIGN_LAT         = 0x6201A004;
 	 const int C_LINK_ALIGN_ERR         = 0x6201A008;

 	 const int C_ALGO_RESET             = 0x6201B000;

  	 const int C_LINK_BUFFER_RX_CAP_nPB     = 0x6201C000;
  	 const int C_LINK_BUFFER_RX_CAP_ARM     = 0x6201C004;
  	 const int C_LINK_BUFFER_RX_MODE        = 0x6201C008;
  	 const int C_LINK_BUFFER_RX_START_BXID  = 0x6201C00C;

  	 const int C_LINK_BUFFER_TX_CAP_nPB     = 0x6201C020;
  	 const int C_LINK_BUFFER_TX_CAP_ARM     = 0x6201C024;
  	 const int C_LINK_BUFFER_TX_MODE        = 0x6201C028;
  	 const int C_LINK_BUFFER_TX_START_BXID  = 0x6201C02c;

  	 const int C_UPTIME_CNT     = 0x6201FFF0;

	/////////////////////////////////////////////////////////////////////
	//
	// MGT Register File
	//
	/////////////////////////////////////////////////////////////////////

	const int MGT_CH_TO_CH_REG_OFFSET = 0x100;
	
	const int QPLL_PD = 0x69010008;
	const int QPLL_RST = 0x69010000;
	const int QPLL_STAT = 0x69010004;

	const int CPLL_PD = 0x69000028;
	const int CPLL_RST = 0x69000020;
	const int CPLL_STAT = 0x69000004;

	const int RX_MAIN_CTRL = 0x69000030;
	const int TX_MAIN_CTRL = 0x69000034;
	const int RX_RST_CTRL = 0x69000010;
	const int TX_RST_CTRL = 0x69000018;

	const int TX_DRV_CTRL = 0x69000038;
	const int LOOPBACK = 0x6900003C;
       	
	/////////////////////////////////////////////////////////////////////
	// Input and output capture RAMs
	/////////////////////////////////////////////////////////////////////

	
	const int NUM_OF_RX_LINKS = 36;
	const int NUM_OF_TX_LINKS = 2;

	// Channel to Channel (BRAM) Offset
	
	const uint32_t CH_TO_CH_REG_OFFSET = 0x4;		
	const uint32_t BRAM_LINK_OFFSET = 0x1000;

	const uint32_t INPUT_BRAM_0   = 0x61000000;   // RX link 0

	const uint32_t OUTPUT_BRAM_0  = 0x61018000;   // TX link 0

	/////////////////////////////////////////////////////////////////////

	// board vs algo RX link mapping	
	const int RXLinkMap[36] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
				   12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
				   24,25,26,27,28,29,30,31, 32, 33, 34, 35}; 

	// board vs algo TX link mapping	
	const int TXLinkMap[2] = {0, 1}; 
};

#endif

