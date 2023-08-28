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
	 const uint32_t POS_TO_NEG_REG_ADDR_OFFSET = 0x2E00;

	 

  	 const int C_LINK_STAT_CH0_ADDR         = 0x62000000;
  	 const int C_BXID_LINK_OFFSET_CH0_ADDR  = 0x62000004;
  	 const int C_LINK_MASK_CH0_ADDR         = 0x62000018;
	 const int C_LINK_BP_STAT_CH0_ADDR      = 0x6200001C;
	 const int C_LINK_BP_BX0ERROR_CH0_ADDR  = 0x62000020;

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

 	 const int C_LINK_ALIGN_REQ         = 0x6201A00C;
 	 const int C_LINK_ALIGN_LAT         = 0x6201A010;
 	 const int C_LINK_ALIGN_ERR         = 0x6201A014;

 	 const int C_ALGO_RESET             = 0x6201B000;

  	 const int C_LINK_BUFFER_RX_CAP_nPB     = 0x6201C000;
  	 const int C_LINK_BUFFER_RX_CAP_ARM     = 0x6201C004;
  	 const int C_LINK_BUFFER_RX_MODE        = 0x6201C008;
  	 const int C_LINK_BUFFER_RX_START_BXID  = 0x6201C00C;

  	 const int C_LINK_BUFFER_TX_CAP_nPB     = 0x6201C020;
  	 const int C_LINK_BUFFER_TX_CAP_ARM     = 0x6201C024;
  	 const int C_LINK_BUFFER_TX_MODE        = 0x6201C028;
  	 const int C_LINK_BUFFER_TX_START_BXID  = 0x6201C02c;

	 const uint32_t DAQ_READOUT_MODE       = (0x62010100);     
	 const uint32_t DAQ_BX_SIZE_EXTENDED   = (0x62010104);     
	 const uint32_t DAQ_BX_EXT_INTERVAL    = (0x62010108);     
	 const uint32_t DAQ_DELAY_LINE_RST     = (0x6201010C);     
	 const uint32_t DAQ_DELAY_LINE_DEPTH   = (0x62010110);     
	 const uint32_t DAQ_FIFO_UPPER_THR     = (0x62010114);     
	 const uint32_t DAQ_FIFO_LOWER_THR     = (0x62010118);     
	 const uint32_t DAQ_STATS_RST          = (0x6201011C);     

	 const uint32_t RUN_NUMBER             = (0x62010120);     

	 const uint32_t FIFO_OCC               = (0x62010200);     
	 const uint32_t FIFO_OCC_MAX           = (0x62010204);     
	 const uint32_t FIFO_EMPTY             = (0x62010208);     
	 const uint32_t CTP7_TO_AMC13_BP       = (0x6201020C);     
	 const uint32_t AMC13_TO_CTP7_BP       = (0x62010210);     
	 const uint32_t TTS                    = (0x62010214);     
	 const uint32_t INTERNAL_ERROR         = (0x62010218);

  	 const int C_UPTIME_CNT     = 0x6201FFF0;

	 const uint32_t L1_PHI_REGION     = (0x6201FF00);

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


	// Channel to Channel (LUT) Offset
	const uint32_t CH_TO_CH_LUT_OFFSET = 0x800;

	// Positive to Negative LUT Address Offset
	const uint32_t POS_TO_NEG_LUT_ADDR_OFFSET = 0x100000;

	const uint32_t POS_LUT_ECAL_01 = 0x66000000;
	const uint32_t POS_LUT_ECAL_02 = 0x66000800;
	const uint32_t POS_LUT_ECAL_03 = 0x66001000;
	const uint32_t POS_LUT_ECAL_04 = 0x66001800;
	const uint32_t POS_LUT_ECAL_05 = 0x66002000;
	const uint32_t POS_LUT_ECAL_06 = 0x66002800;
	const uint32_t POS_LUT_ECAL_07 = 0x66003000;
	const uint32_t POS_LUT_ECAL_08 = 0x66003800;
	const uint32_t POS_LUT_ECAL_09 = 0x66004000;
	const uint32_t POS_LUT_ECAL_10 = 0x66004800;
	const uint32_t POS_LUT_ECAL_11 = 0x66005000;
	const uint32_t POS_LUT_ECAL_12 = 0x66005800;
	const uint32_t POS_LUT_ECAL_13 = 0x66006000;
	const uint32_t POS_LUT_ECAL_14 = 0x66006800;
	const uint32_t POS_LUT_ECAL_15 = 0x66007000;
	const uint32_t POS_LUT_ECAL_16 = 0x66007800;
	const uint32_t POS_LUT_ECAL_17 = 0x66008000;
	const uint32_t POS_LUT_ECAL_18 = 0x66008800;
	const uint32_t POS_LUT_ECAL_19 = 0x66009000;
	const uint32_t POS_LUT_ECAL_20 = 0x66009800;
	const uint32_t POS_LUT_ECAL_21 = 0x6600A000;
	const uint32_t POS_LUT_ECAL_22 = 0x6600A800;
	const uint32_t POS_LUT_ECAL_23 = 0x6600B000;
	const uint32_t POS_LUT_ECAL_24 = 0x6600B800;
	const uint32_t POS_LUT_ECAL_25 = 0x6600C000;
	const uint32_t POS_LUT_ECAL_26 = 0x6600C800;
	const uint32_t POS_LUT_ECAL_27 = 0x6600D000;
	const uint32_t POS_LUT_ECAL_28 = 0x6600D800;

	const uint32_t POS_LUT_HCAL_01 = 0x66020000;
	const uint32_t POS_LUT_HCAL_02 = 0x66020800;
	const uint32_t POS_LUT_HCAL_03 = 0x66021000;
	const uint32_t POS_LUT_HCAL_04 = 0x66021800;
	const uint32_t POS_LUT_HCAL_05 = 0x66022000;
	const uint32_t POS_LUT_HCAL_06 = 0x66022800;
	const uint32_t POS_LUT_HCAL_07 = 0x66023000;
	const uint32_t POS_LUT_HCAL_08 = 0x66023800;
	const uint32_t POS_LUT_HCAL_09 = 0x66024000;
	const uint32_t POS_LUT_HCAL_10 = 0x66024800;
	const uint32_t POS_LUT_HCAL_11 = 0x66025000;
	const uint32_t POS_LUT_HCAL_12 = 0x66025800;
	const uint32_t POS_LUT_HCAL_13 = 0x66026000;
	const uint32_t POS_LUT_HCAL_14 = 0x66026800;
	const uint32_t POS_LUT_HCAL_15 = 0x66027000;
	const uint32_t POS_LUT_HCAL_16 = 0x66027800;
	const uint32_t POS_LUT_HCAL_17 = 0x66028000;
	const uint32_t POS_LUT_HCAL_18 = 0x66028800;
	const uint32_t POS_LUT_HCAL_19 = 0x66029000;
	const uint32_t POS_LUT_HCAL_20 = 0x66029800;
	const uint32_t POS_LUT_HCAL_21 = 0x6602A000;
	const uint32_t POS_LUT_HCAL_22 = 0x6602A800;
	const uint32_t POS_LUT_HCAL_23 = 0x6602B000;
	const uint32_t POS_LUT_HCAL_24 = 0x6602B800;
	const uint32_t POS_LUT_HCAL_25 = 0x6602C000;
	const uint32_t POS_LUT_HCAL_26 = 0x6602C800;
	const uint32_t POS_LUT_HCAL_27 = 0x6602D000;
	const uint32_t POS_LUT_HCAL_28 = 0x6622D800;

	const uint32_t POS_LUT_HF_30   = 0x66040000;
	const uint32_t POS_LUT_HF_31   = 0x66041000;
	const uint32_t POS_LUT_HF_32   = 0x66042000;
	const uint32_t POS_LUT_HF_33   = 0x66043000;
	const uint32_t POS_LUT_HF_34   = 0x66044000;
	const uint32_t POS_LUT_HF_35   = 0x66045000;
	const uint32_t POS_LUT_HF_36   = 0x66046000;
	const uint32_t POS_LUT_HF_37   = 0x66047000;
	const uint32_t POS_LUT_HF_38   = 0x66048000;
	const uint32_t POS_LUT_HF_39   = 0x66049000;
	const uint32_t POS_LUT_HF_40   = 0x66a4A000;
	const uint32_t POS_LUT_HF_41   = 0x6604B000;

	const uint32_t CH_TO_CH_LUT2S_OFFSET = 0x8000;

	// Positive to Negative LUT Address Offset
	const uint32_t POS_TO_NEG_LUT2S_ADDR_OFFSET = 0x100000;

	const uint32_t POS_LUT2S_XB_01 = 0x67000000;
	const uint32_t POS_LUT2S_XB_02 = 0x67008000;
	const uint32_t POS_LUT2S_XB_03 = 0x67010000;
	const uint32_t POS_LUT2S_XB_04 = 0x67018000;
	const uint32_t POS_LUT2S_XB_05 = 0x67020000;
	const uint32_t POS_LUT2S_XB_06 = 0x67028000;
	const uint32_t POS_LUT2S_XB_07 = 0x67030000;
	const uint32_t POS_LUT2S_XB_08 = 0x67038000;
	const uint32_t POS_LUT2S_XB_09 = 0x67040000;
	const uint32_t POS_LUT2S_XB_10 = 0x67048000;
	const uint32_t POS_LUT2S_XB_11 = 0x67050000;
	const uint32_t POS_LUT2S_XB_12 = 0x67058000;
	const uint32_t POS_LUT2S_XB_13 = 0x67060000;
	const uint32_t POS_LUT2S_XB_14 = 0x67068000;
	const uint32_t POS_LUT2S_XB_15 = 0x67070000;
	const uint32_t POS_LUT2S_XB_16 = 0x67078000;
	const uint32_t POS_LUT2S_XB_17 = 0x67080000;
	const uint32_t POS_LUT2S_XB_18 = 0x67088000;
	const uint32_t POS_LUT2S_XB_19 = 0x67090000;
	const uint32_t POS_LUT2S_XB_20 = 0x67098000;
	const uint32_t POS_LUT2S_XB_21 = 0x670A0000;
	const uint32_t POS_LUT2S_XB_22 = 0x670A8000;
	const uint32_t POS_LUT2S_XB_23 = 0x670B0000;
	const uint32_t POS_LUT2S_XB_24 = 0x670B8000;
	const uint32_t POS_LUT2S_XB_25 = 0x670C0000;
	const uint32_t POS_LUT2S_XB_26 = 0x670C8000;
	const uint32_t POS_LUT2S_XB_27 = 0x670D0000;
	const uint32_t POS_LUT2S_XB_28 = 0x670D8000;


	


	/////////////////////////////////////////////////////////////////////
	// 2nd Stage LUT BRAM Address Definition
	// Negative LUTs are located at POS_TO_NEG2S_LUT_ADDR_OFFSET offset apart from
	// corresponding positive BRAMs
	// I.e.: NEG_LUT2S_XB_1 = POS_LUT2S_XB_1 + POS_TO_NEG_LUT2S_ADDR_OFFSET
	/////////////////////////////////////////////////////////////////////

	// Channel to Channel (LUT) Offset
	//const uint32_t CH_TO_CH_LUT2S_OFFSET = 0x8000;

	// Positive to Negative LUT Address Offset
	//const uint32_t POS_TO_NEG_LUT2S_ADDR_OFFSET = 0x100000;

	
	// V7 FW Build Details
	const uint32_t FW_BUILD_DATE     = (0x6201FFE0);
	const uint32_t FW_GIT_HASH_CODE  = (0x6201FFE4);
	const uint32_t FW_GIT_HASH_DIRTY = (0x6201FFE8);
	const uint32_t FW_VERSION        = (0x6201FFEC);
	const uint32_t FW_PROJECT_CODE   = (0x6201FFFC);

	// V7 FW Uptime (in seconds since configuration)
	const uint32_t FW_UPTIME         = (0x6201FFF8);

       	
	/////////////////////////////////////////////////////////////////////
	// Input and output capture RAMs
	/////////////////////////////////////////////////////////////////////

	
	const int NUM_OF_RX_LINKS = 36;
	const int NUM_OF_TX_LINKS = 2;

	// Channel to Channel (BRAM) Offset
	
	const uint32_t CH_TO_CH_REG_OFFSET = 0x4;		
	const uint32_t BRAM_LINK_OFFSET = 0x1000;

	const uint32_t INPUT_BRAM_0   = 0x61000000;   // RX link 0

	const uint32_t OUTPUT_BRAM_0  = 0x61000000;   // TX link 0

	/////////////////////////////////////////////////////////////////////

	// board vs algo RX link mapping	
	const int RXLinkMap[36] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
				   12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
				   24,25,26,27,28,29,30,31, 32, 33, 34, 35}; 

	// board vs algo TX link mapping	
	const int TXLinkMap[2] = {0,1};

#endif

