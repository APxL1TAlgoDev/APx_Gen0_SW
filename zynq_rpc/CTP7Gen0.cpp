#include <stdlib.h>
#include <unistd.h>
#include "moduleapi.h"
#include <libmemsvc.h>
#include "CTP7Gen0RegMap.hh"

using namespace CTP7Gen0RegMap;

#define RETURN_ERROR(msg) do { \
        LOGGER->log_message(LogManager::ERROR, msg); \
        response->set_word("error", 1); \
        return; \
    } while (0)
#define REQUIRE_PARAMETER(p) if (!request->get_key_exists(p)) RETURN_ERROR(stdsprintf("Missing required parameter %s", (#p)))
#define PARAM_WORD(p) REQUIRE_PARAMETER(#p); uint32_t p = request->get_word(#p)

#define LOG_PARAM_1(a)         LOGGER->log_message(LogManager::INFO, stdsprintf("%s: %u", #a, (a)))
#define LOG_PARAM_2(a,b)       LOGGER->log_message(LogManager::INFO, stdsprintf("%s: %u, %s: %u", #a, (a), #b, (b)))
#define LOG_PARAM_3(a,b,c)     LOGGER->log_message(LogManager::INFO, stdsprintf("%s: %u, %s: %u, %s: %u", #a, (a), #b, (b), #c, (c)))
#define LOG_PARAM_4(a,b,c,d)   LOGGER->log_message(LogManager::INFO, stdsprintf("%s: %u, %s: %u, %s: %u, %s: %u", #a, (a), #b, (b), #c, (c), #d, (d)))


#define VALIDATE_RANGE(v, min, max) \
    do { \
        if ((v) > (max) || (v) < (min)) \
        RETURN_ERROR(stdsprintf("Parameter %s out of range: failed assertion %u <= %u <= %u", #v, (min), (v), (max))); \
    } while (0)

#define VALIDATE_INPUT_LINK(il) VALIDATE_RANGE(il, 0, 68)
#define VALIDATE_OUTPUT_LINK(ol) VALIDATE_RANGE(ol, 0, 47)

//#define CH_ADDR(reg, ch) ((reg) + ((ch)*C_CH_to_CH_ADDR_OFFSET) )

#define CH_ADDR(reg, ch, negeta) ((reg) + ((ch)*C_CH_to_CH_ADDR_OFFSET) + ((negeta)?POS_TO_NEG_REG_ADDR_OFFSET:0))
#define LUT_CH_ADDR(reg, ch, negeta) ((reg) + ((ch)*CH_TO_CH_LUT_OFFSET) + ((negeta)?POS_TO_NEG_LUT_ADDR_OFFSET:0))
#define LUT2S_CH_ADDR(reg, ch, negeta) ((reg) + ((ch)*CH_TO_CH_LUT2S_OFFSET) + ((negeta)?POS_TO_NEG_LUT2S_ADDR_OFFSET:0))


#define MGT_CH_ADDR(reg, ch) ((reg) + ((ch)*MGT_CH_TO_CH_REG_OFFSET))
#define BRAM_CH_ADDR(reg, ch) ((reg) + ((ch)*BRAM_LINK_OFFSET))

#define SWEET_SPOT_OFFSET 330
static memsvc_handle_t memsvc;

static inline bool write_word(uint32_t addr, uint32_t word)
{
	if (memsvc_write(memsvc, addr, 1, &word) != 0)
		return false;
	return true;
}
static inline bool read_word(uint32_t addr, uint32_t &word)
{
	if (memsvc_read(memsvc, addr, 1, &word) != 0)
		return false;
	return true;
}

static inline bool bram_write_verify(uint32_t addr, const std::vector<uint32_t> &data)
{
	uint32_t datablock[data.size()];
	for (unsigned int i = 0; i < data.size(); i++)
		datablock[i] = data[i];

	if (memsvc_write(memsvc, addr, data.size(), datablock) != 0)
		return false;

	uint32_t verifblock[data.size()];
	if (memsvc_read(memsvc, addr, data.size(), verifblock) != 0)
		return false;

	if (memcmp(datablock, verifblock, data.size()*sizeof(uint32_t)) != 0)
		return false;

	return true;
}


static inline bool bram_read(uint32_t addr, uint32_t nwords, std::vector<uint32_t> &data)
{
	uint32_t datablock[nwords];

	if (memsvc_read(memsvc, addr, nwords, datablock) != 0)
		return false;

	data.clear();
	for (unsigned int i = 0; i < nwords; i++)
		data.push_back(datablock[i]);

	return true;
}
void setRunNumber(const RPCMsg *request, RPCMsg *response)
{
	
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to setRunNumber module "));
	response->set_word("result", 1);
}

void ping(const RPCMsg *request, RPCMsg *response)
{
	*response = *request;
}

void testUptime(const RPCMsg *request, RPCMsg *response)
{

	uint32_t uptimeData;
    /*if (!read_word(CH_ADDR(C_UPTIME_CNT,0),uptimeData))
    {
        LOGGER->log_message(LogManager::ERROR, "Unable to read uptime data from Uptime Address");
        response->set_word("error", 1);
        return;
    }*/
	LOGGER->log_message(LogManager::INFO, stdsprintf("Uptime data successfully captured %d ", uptimeData));
    response->set_word("result", uptimeData);
}

void setDAQConfig(const RPCMsg *request, RPCMsg *response)
{
	

	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to DAQConfig module.  "));

	response->set_word("result", 1);
	
}

static inline bool ttc_dec_rst_stat(void)
{
	return write_word(C_TTC_STAT_RST, 1);
}

static inline bool get_ttc_bit_err_count(uint32_t & ttc_dec_bit_err_cnt)
{

	uint32_t single_bit_err_cnt, double_bit_err_cnt;
	

	read_word(C_TTC_DEC_SINGLE_ERR_CNT, single_bit_err_cnt);
	read_word(C_TTC_DEC_DOUBLE_ERR_CNT, double_bit_err_cnt);

	//ttc_dec_bit_err_cnt = single_bit_err_cnt + double_bit_err_cnt;
	ttc_dec_bit_err_cnt =  double_bit_err_cnt;

	return true; // todo: return value
}

uint32_t find_initial_err_transition(void)
{
	uint32_t next_break;


	ttc_dec_rst_stat();

	usleep(10000);

	uint32_t ttc_dec_bit_err_cnt;

	get_ttc_bit_err_count(ttc_dec_bit_err_cnt);

	if (ttc_dec_bit_err_cnt > 5)
	{
		next_break = 0;
	}
	else
	{
		next_break = 1;
	}

	return  next_break ;
}

static inline bool phase_shift_BC_clock(uint32_t phase_shift_req)
{
	uint32_t phase_shift_curr = 0;

	do
	{
		write_word(C_TTC_MMCM_PHASE_SHIFT, 1);
		phase_shift_curr++;
	}
	while (phase_shift_curr < phase_shift_req);

	return true; // todo: return value
}

uint32_t move_to_next_err_transition(uint32_t err_transition)
{
	uint32_t shift_count = 0;
	uint32_t shift_continue = 1;

	uint32_t ttc_dec_bit_err_cnt;

	do
	{
		phase_shift_BC_clock(1);
		shift_count++;

		ttc_dec_rst_stat();

		usleep(1000);

		get_ttc_bit_err_count(ttc_dec_bit_err_cnt);

		if (err_transition == 0 && ttc_dec_bit_err_cnt == 0)
		{
			shift_continue = 0;
		}
		else if (err_transition != 0 && ttc_dec_bit_err_cnt != 0)
		{
			shift_continue = 0;
		}
	}
	while (shift_continue && shift_count < 5000 ); // bail out after 5000 clock phase shifts

	return shift_count;
}

void alignTTCDecoder(const RPCMsg *request, RPCMsg *response)
{

	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to align TTCDecoder module "));

	// To be looked into if needed
//	uint32_t mmcm_locked;
//	read_word(TTC_MMCM_LOCKED, mmcm_locked);


	//write_word(TTC_MMCM_RST, 1);
	//usleep(50000);

	// disable L1As first
	if (!write_word(C_TTC_L1A_ENABLE, 0))  RETURN_ERROR("Unable to access registers");


	uint32_t err_transition;

	err_transition = find_initial_err_transition();

	phase_shift_BC_clock(10);

	move_to_next_err_transition(err_transition);

	if (err_transition == 0)
	{
		phase_shift_BC_clock(SWEET_SPOT_OFFSET);
	}
	else
	{
		err_transition = 0;

		phase_shift_BC_clock(10);

		move_to_next_err_transition(err_transition);

		phase_shift_BC_clock(SWEET_SPOT_OFFSET);
	}

	usleep(5000);

	// Reset TTC diagnostics before returning
	ttc_dec_rst_stat();

	write_word(C_TTC_L1A_ENABLE, 1); // reenable L1As
    
}


void setInputLinkTowerMask(const RPCMsg *request, RPCMsg *response)
{

	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to setInputLinkTowerMask "));

	response->set_word("result", 1);
    
}

void setTMTCycle(const RPCMsg *request, RPCMsg *response)
{
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to setTMTCycle "));
	response->set_word("result", 1);
}

void getInputLinkTowerMask(const RPCMsg *request, RPCMsg *response)
{

	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to getInputLinkTowerMask "));
	
    
}


void setInputLinkAlignmentMask(const RPCMsg *request, RPCMsg *response)
{
	PARAM_WORD(negativeEta);
	LOG_PARAM_1(negativeEta);

	uint32_t tt_mask; // trigger tower mask

	if (!request->get_key_exists("mask"))
		RETURN_ERROR("Missing required parameter: mask");
	std::vector<uint32_t> mask = request->get_word_array("mask");
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to setInputLinkAlignmentMask command ETA positive or negative coming"));

	VALIDATE_RANGE(mask.size(), 32, 32);
	// CXP
	if (negativeEta==0) 

		{
			LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to setInputLinkAlignmentMask: ETA Negative chk"));
			for (uint32_t link = 0; link < 24; link++)
			{
				// Input link masks
				if (!write_word(CH_ADDR(C_LINK_MASK_CH0_ADDR, link, negativeEta), mask[link]))
					RETURN_ERROR("Unable to access registers");

			}
			
		}
	//back plane
	else if (negativeEta==1) 
	
		{
			LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to setInputLinkAlignmentMask: ETA positive chk"));
			for (uint32_t link = 0; link < 32; link++)
			{
				
				// Input link masks
				if (!write_word(CH_ADDR(C_LINK_MASK_CH0_ADDR, link, negativeEta), mask[link]))
					RETURN_ERROR("Unable to access registers");

			}
			
		}
	
}

void alignInputLinks(const RPCMsg *request, RPCMsg *response)
{
	PARAM_WORD(negativeEta);
	PARAM_WORD(bx);
	PARAM_WORD(sub_bx);
	PARAM_WORD(alignManual);

	LOG_PARAM_4(negativeEta, bx, sub_bx, alignManual);

	VALIDATE_RANGE(bx, 0, 3563);
	VALIDATE_RANGE(sub_bx, 0, 5);

	uint32_t internal_bx_align_point = bx * 4 + sub_bx - 1;  //160 MHz clock

	//if (!write_word(CH_ADDR(L1_POS_LINK_ALIGN_MODE,    0, negativeEta), (uint32_t)alignManual))        RETURN_ERROR("Unable to access registers");
	if (!write_word(CH_ADDR(C_LINK_ALIGN_LAT, 0, negativeEta), internal_bx_align_point))   RETURN_ERROR("Unable to access registers");

	if (!write_word(CH_ADDR(C_LINK_ALIGN_REQ, 0, negativeEta), 1)) RETURN_ERROR("Unable to access registers");
	if (!write_word(CH_ADDR(C_LINK_ALIGN_REQ, 0, negativeEta), 0)) RETURN_ERROR("Unable to access registers");
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to alignInputLinks module updated "));
	response->set_word("result", 1);
	
	
}

void alignOutputLinks(const RPCMsg *request, RPCMsg *response)
{
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to alignoutput module "));
	response->set_word("result", 1);
}

void resetInputLinkBX0ErrorCounters(const RPCMsg *request, RPCMsg *response)
{
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to resetInputLinkBX0ErrorCounters "));
	response->set_word("result", 1);
}

void resetInputLinkChecksumErrorCounters(const RPCMsg *request, RPCMsg *response)
{
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to resetInputLinkChecksumErrorCounters  "));
	response->set_word("result", 1);
}

void getInputLinkAlignmentStatus(const RPCMsg *request, RPCMsg *response)
{
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to getInputLink alignment status  "));
	PARAM_WORD(negativeEta);
	uint32_t alignStatus;
	if (!read_word(CH_ADDR(C_LINK_ALIGN_ERR, 0, negativeEta), alignStatus))
		RETURN_ERROR("Unable to access registers");
	response->set_word("result", alignStatus);

}

void getInputLinkStatus(const RPCMsg *request, RPCMsg *response)
{
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to getInputLink status  "));
	// C_LINK_BP_STAT_CH0_ADDR 
	PARAM_WORD(negativeEta);
	std::vector<uint32_t> rawLinkStatus, checkSumErrorCount, bx0ErrorCount, bx0Latency, alignmentMask, towerMask;
	uint32_t word;
	//CXP
	if (negativeEta==0) 

		{
			LOGGER->log_message(LogManager::INFO, stdsprintf(" Eta positive raw link status push back  "));
			for (int i = 0; i < 24; i++)
				{	
			//LOGGER->log_message(LogManager::INFO, stdsprintf(" Eta positive raw link status push back  %d ", i));

					if (!read_word(CH_ADDR(C_LINK_BP_STAT_CH0_ADDR , i, negativeEta), word))
						RETURN_ERROR("Unable to access registers");
					// Extract the first and second bits of 'word'
					bool bit0 = (word >> 0) & 1; // Extract the 0th bit
					bool bit1 = (word >> 1) & 1; // Extract the 1st bit

					// Extract the 8th, 11th, 12th, 13th, and 14th bits of 'word'
					bool bit8 = (word >> 8) & 1;  // Extract the 8th bit
					bool bit11 = (word >> 11) & 1; // Extract the 11th bit
					bool bit12 = (word >> 12) & 1; // Extract the 12th bit
					bool bit13 = (word >> 13) & 1; // Extract the 13th bit
					bool bit14 = (word >> 14) & 1; // Extract the 14th bit

					// Perform AND operation on the extracted bits
					bool andResult = bit8 | bit11 | bit12 | bit13 | bit14;

						// Perform AND operation on extracted bits
					bool resultBit = bit0 & bit1;

					// Calculate the result based on AND operation and 'resultBit'
					bool finalResult = !andResult && resultBit; // Multiply by negating resultBit


						// Set the 0th bit of 'word' based on the AND operation result
					word &= ~(1 << 0); // Clear the 0th bit
					word |= (finalResult << 0); // Set the 0th bit b	
					rawLinkStatus.push_back(word);
					
					if (!read_word(CH_ADDR(C_LINK_BP_STAT_CH0_ADDR , i, negativeEta), word))
						RETURN_ERROR("Unable to access registers");
					checkSumErrorCount.push_back(word);

					if (!read_word(CH_ADDR(C_LINK_BP_BX0ERROR_CH0_ADDR , i, negativeEta), word))
						RETURN_ERROR("Unable to access registers");
					bx0ErrorCount.push_back(word);

					if (!read_word(CH_ADDR(C_LINK_ALIGN_LAT , i, negativeEta), word))
						RETURN_ERROR("Unable to access registers");
					bx0Latency.push_back(word);

					if (!read_word(CH_ADDR(C_LINK_MASK_CH0_ADDR, i, negativeEta), word))
						RETURN_ERROR("Unable to access registers");
					alignmentMask.push_back(word);

					if (!read_word(CH_ADDR(C_LINK_MASK_CH0_ADDR, i, negativeEta), word))
						RETURN_ERROR("Unable to access registers");
					towerMask.push_back(word);
				}
	
	response->set_word_array("rawLinkStatus", rawLinkStatus);
	response->set_word_array("checkSumErrorCount", checkSumErrorCount);
	response->set_word_array("bx0ErrorCount", bx0ErrorCount);
	response->set_word_array("bx0Latency", bx0Latency);
	response->set_word_array("alignmentMask", alignmentMask);
	response->set_word_array("towerMask", towerMask);
	}
	//backplane
	else if (negativeEta==1) 
			
		{
			std::vector<int> channelsOfInterest;
			channelsOfInterest.push_back(0);
			channelsOfInterest.push_back(2);
			channelsOfInterest.push_back(8);
			channelsOfInterest.push_back(9);
			channelsOfInterest.push_back(20);
			channelsOfInterest.push_back(21);
			channelsOfInterest.push_back(22);
			channelsOfInterest.push_back(24);
			channelsOfInterest.push_back(25);
			channelsOfInterest.push_back(26);
			channelsOfInterest.push_back(28);
			channelsOfInterest.push_back(30);
			LOGGER->log_message(LogManager::INFO, stdsprintf(" Eta negative raw link status push back  "));

			for (int i = 0; i < 32; i++)
				{	
			//LOGGER->log_message(LogManager::INFO, stdsprintf(" Eta positive raw link status push back  %d ", i));
					bool isChannelOfInterest = false;
        			for (size_t j = 0; j < channelsOfInterest.size(); j++) {
            			if (channelsOfInterest[j] == i) {
                			isChannelOfInterest = true;
                			break;
            				}
        			}

					if (isChannelOfInterest)

        			{

						if (!read_word(CH_ADDR(C_LINK_BP_STAT_CH0_ADDR , i, negativeEta), word))
							RETURN_ERROR("Unable to access registers");
							// Extract the first and second bits of 'word'
						bool bit0 = (word >> 0) & 1; // Extract the 0th bit
						bool bit1 = (word >> 1) & 1; // Extract the 1st bit

						// Extract the 8th, 11th, 12th, 13th, and 14th bits of 'word'
						bool bit8 = (word >> 8) & 1;  // Extract the 8th bit
						bool bit11 = (word >> 11) & 1; // Extract the 11th bit
						bool bit12 = (word >> 12) & 1; // Extract the 12th bit
						bool bit13 = (word >> 13) & 1; // Extract the 13th bit
						bool bit14 = (word >> 14) & 1; // Extract the 14th bit

						// Perform AND operation on the extracted bits
						bool andResult = bit8 & bit11 & bit12 & bit13 & bit14;

							// Perform AND operation on extracted bits
						bool resultBit = bit0 & bit1;

						// Calculate the result based on AND operation and 'resultBit'
						bool finalResult = andResult && !resultBit; // Multiply by negating resultBit


							// Set the 0th bit of 'word' based on the AND operation result
						word &= ~(1 << 0); // Clear the 0th bit
						word |= (finalResult << 0); // Set the 0th bit b	
						rawLinkStatus.push_back(word);
									
						
						if (!read_word(CH_ADDR(C_LINK_BP_STAT_CH0_ADDR , i, negativeEta), word))
							RETURN_ERROR("Unable to access registers");
						checkSumErrorCount.push_back(word);

						if (!read_word(CH_ADDR(C_LINK_BP_BX0ERROR_CH0_ADDR , i, negativeEta), word))
							RETURN_ERROR("Unable to access registers");
						bx0ErrorCount.push_back(word);

						if (!read_word(CH_ADDR(C_LINK_ALIGN_LAT , i, negativeEta), word))
							RETURN_ERROR("Unable to access registers");
						bx0Latency.push_back(word);

						if (!read_word(CH_ADDR(C_LINK_MASK_CH0_ADDR, i, negativeEta), word))
							RETURN_ERROR("Unable to access registers");
						alignmentMask.push_back(word);

						if (!read_word(CH_ADDR(C_LINK_MASK_CH0_ADDR, i, negativeEta), word))
							RETURN_ERROR("Unable to access registers");
						towerMask.push_back(word);
					}
					
				}
	
	response->set_word_array("rawLinkStatus", rawLinkStatus);
	response->set_word_array("checkSumErrorCount", checkSumErrorCount);
	response->set_word_array("bx0ErrorCount", bx0ErrorCount);
	response->set_word_array("bx0Latency", bx0Latency);
	response->set_word_array("alignmentMask", alignmentMask);
	response->set_word_array("towerMask", towerMask);
	}
	
	
	
	
}

void resetInputLinkDecoders(const RPCMsg *request, RPCMsg *response)
{
	
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to reset Input LinkDecoders  "));

	response->set_word("result", 1);
}


void getTTCBGoCmdCnt(const RPCMsg *request, RPCMsg *response)
{
	uint32_t value=0;
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to reset getTTCBgoCmdcnt  "));
	response->set_word("L1A", value);
	response->set_word("BX0", value);
	response->set_word("EC0", value);
	response->set_word("Resync", value);

	response->set_word("OC0", value);
	response->set_word("TestSync", value);
	response->set_word("Start", value);
	response->set_word("Stop", value);

	response->set_word("L1ID", value);
	response->set_word("Orbit", value);
}

void getTTCStatus(const RPCMsg *request, RPCMsg *response)
{
	uint32_t value;
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to getTTCStatus module  "));

	if (!read_word(C_TTC_MMCM_LOCKED, value)) RETURN_ERROR("Unable to access registers");
	response->set_word("BCClockLocked", true);

	if (!read_word(C_TTC_BX0_LOCKED, value)) RETURN_ERROR("Unable to access registers");
	response->set_word("BX0Locked", true);

	if (!read_word(C_TTC_BX0_ERR, value)) RETURN_ERROR("Unable to access registers");
	response->set_word("BX0Error", false);

	if (!read_word(C_TTC_BX0_UNLOCKED_CNT, value)) RETURN_ERROR("Unable to access registers");
	response->set_word("BX0UnlockedCnt", false);

	if (!read_word(C_TTC_DEC_SINGLE_ERR_CNT, value)) RETURN_ERROR("Unable to access registers");
	response->set_word("TTCDecoderSingleError", false);

	if (!read_word(C_TTC_DEC_DOUBLE_ERR_CNT, value)) RETURN_ERROR("Unable to access registers");
	response->set_word("TTCDecoderDoubleError", false);
}

void getDAQStatus(const RPCMsg *request, RPCMsg *response)
{
	uint32_t value=0;
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to getDAQStatus module "));

	response->set_word("fifoOcc", value);

	response->set_word("fifoOccMax", value);

	response->set_word("fifoEmpty", value);

	response->set_word("CTP77ToAMC13BP", value);

	response->set_word("AMC13ToCTP7BP", value);

	response->set_word("TTS", value);

	response->set_word("internalError", value);

//	if (!read_word(INTERNAL_ERROR, value))	
//		RETURN_ERROR("Unable to access registers");
	response->set_word("amcCoreReady", 1);

}


void getInputLinkLUT(const RPCMsg *request, RPCMsg *response)
{
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to getInputLINK LUT module  "));
	PARAM_WORD(negativeEta);
	PARAM_WORD(type);
	PARAM_WORD(iEta);
	VALIDATE_RANGE(type, 1, 3);
	if (type == 1 || type == 2)
		VALIDATE_RANGE(iEta, 1, 28);
	else
		VALIDATE_RANGE(iEta, 30, 41);

	uint32_t base;
	uint32_t ch_offset;
	uint32_t lut_size;

	switch (type)
	{
	case 1:
		base = POS_LUT_ECAL_01;
		ch_offset = iEta - 1;
		lut_size = 512;
		break;
	case 2:
		base = POS_LUT_HCAL_01;
		ch_offset = iEta - 1;
		lut_size = 512;
		break;
	case 3:
		base = POS_LUT_HF_30;
		ch_offset = (iEta - 30) * 2; // HF LUTs are twice the size of ECAL/HCAL, hence the offset between 2 HF LUTs twice bigger
		lut_size = 1024;
		break;
	}

	std::vector<uint32_t> data;
	//if (!bram_read(LUT_CH_ADDR(base, ch_offset, negativeEta), lut_size, data))
	//	RETURN_ERROR("Unable to acccess BRAM");
	response->set_word_array("result", data);
}


// virtual bool setInputLinkLUT(bool negativeEta, LUTType type, LUTiEtaIndex iEta, const std::vector<uint32_t> &lut);
void setInputLinkLUT(const RPCMsg *request, RPCMsg *response)
{

	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to setInputLINK LUT module  "));
	/*PARAM_WORD(negativeEta);
	PARAM_WORD(type);
	PARAM_WORD(iEta);
	VALIDATE_RANGE(type, 1, 3);

        uint32_t lut_size;

	if (type == 1 || type == 2) {
		VALIDATE_RANGE(iEta, 1, 28);
		lut_size = 512;
	}
	else {
		VALIDATE_RANGE(iEta, 30, 41);
                lut_size = 1024;
	}

	//if (!request->get_key_exists("lut"))
	//	RETURN_ERROR("Missing required parameter: data");
	std::vector<uint32_t> lut = request->get_word_array("lut");
	VALIDATE_RANGE(lut.size(), lut_size, lut_size);
	LOG_PARAM_3(negativeEta, type, iEta);

	uint32_t base;
	uint32_t ch_offset;
	switch (type)
	{
	case 1:
		base = POS_LUT_ECAL_01;
		ch_offset = iEta - 1;
		break;
	case 2:
		base = POS_LUT_HCAL_01;
		ch_offset = iEta - 1;
		break;
	case 3:
		base = POS_LUT_HF_30;
		ch_offset = (iEta - 30) * 2;  // HF LUTs are twice the size of ECAL/HCAL, hence the offset between 2 HF LUTs twice bigger
		break;
	}*/

	//if (!bram_write_verify(LUT_CH_ADDR(base, ch_offset, negativeEta), lut))
	//	RETURN_ERROR("Unable to access BRAM");
	response->set_word("result", 1);

	
}


// virtual bool getInputLinkLUT2S(bool negativeEta, LUTiEtaIndex iEta, std::vector<uint32_t> &lut);
void getInputLinkLUT2S(const RPCMsg *request, RPCMsg *response)
{
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to getInputLINK LUT2S module  "));

	PARAM_WORD(negativeEta);
	PARAM_WORD(iEta);
	
	VALIDATE_RANGE(iEta, 1, 28);

	uint32_t base;
	uint32_t ch_offset;
	uint32_t lut_size;

	base = POS_LUT2S_XB_01;
	ch_offset = iEta - 1;
	lut_size = 8192;

	std::vector<uint32_t> data;
	//if (!bram_read(LUT2S_CH_ADDR(base, ch_offset, negativeEta), lut_size, data))
	//	RETURN_ERROR("Unable to acccess BRAM");
	response->set_word_array("result", data);
}


// virtual bool setInputLinkLUT2S(bool negativeEta, LUTiEtaIndex iEta, const std::vector<uint32_t> &lut);
void setInputLinkLUT2S(const RPCMsg *request, RPCMsg *response)
{
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to setInputLINK LUT2S module  "));
	/*PARAM_WORD(negativeEta);
	PARAM_WORD(iEta);

        uint32_t lut_size;

	VALIDATE_RANGE(iEta, 1, 28);
	lut_size = 8192;

	//if (!request->get_key_exists("lut"))
	//	RETURN_ERROR("Missing required parameter: data");
	std::vector<uint32_t> lut = request->get_word_array("lut");
	VALIDATE_RANGE(lut.size(), lut_size, lut_size);
	LOG_PARAM_2(negativeEta, iEta);

	uint32_t base;
	uint32_t ch_offset;
	
	base = POS_LUT2S_XB_01;
	ch_offset = iEta - 1;

	//if (!bram_write_verify(LUT2S_CH_ADDR(base, ch_offset, negativeEta), lut))
	//	RETURN_ERROR("Unable to access BRAM");*/
	response->set_word("result", 1);
}

void getInputLinkLUTHcalFb(const RPCMsg *request, RPCMsg *response)
{
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to getInputLINK LUTHCAL Fb  "));
	std::vector<uint32_t> data;
	response->set_word_array("result", data);
}

void setInputLinkLUTHcalFb(const RPCMsg *request, RPCMsg *response)
{
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to setInputLINK LUTHCAL Fb module  "));
	response->set_word("result", 1);
}

// virtual bool hardReset(std::string bitstream = "virtex7");
void hardReset(const RPCMsg *request, RPCMsg *response)
{


	std::string image = request->get_string("bitstream");

	if (image.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_") != std::string::npos)
	{
		LOGGER->log_message(LogManager::ERROR, stdsprintf("hardReset requested illegal bitfile \"%s\"", image.c_str()));
		response->set_word("error", 1);
		return;
	}

	std::string file;
	std::string bitfile = stdsprintf("/mnt/persistent/virtex7/%s.bit", image.c_str());
	if (access(bitfile.c_str(), F_OK) == 0)
		file = bitfile;
	else
	{
		LOGGER->log_message(LogManager::ERROR, stdsprintf("Unable to locate bitfile %s", image.c_str()));
		response->set_word("error", 1);
		return;
	}

	LOGGER->log_message(LogManager::INFO, stdsprintf("loading bitfile %s", file.c_str()));
	int ret;
	for (int retries = 5; retries > 0; retries--)
	{
		ret = system(stdsprintf("/bin/v7load %s #&>/dev/null", file.c_str()).c_str());
		if (WEXITSTATUS(ret) == 0)
			break;
	}
	if (WEXITSTATUS(ret) != 0)
	{
		LOGGER->log_message(LogManager::ERROR, stdsprintf("failed to load bitfile %s: v7load returned %d", file.c_str(), WEXITSTATUS(ret)));
		response->set_word("error", 1);
		return;
	}
	LOGGER->log_message(LogManager::INFO, stdsprintf("loaded bitfile %s: v7load returned %d", file.c_str(), WEXITSTATUS(ret)));

	/*if (!write_word(CH_ADDR(L1_PHI_REGION, 0, 0), phi))
		RETURN_ERROR("Unable to access registers");
	// 0xC0 standardised as the base CTP7 ID for Phi 0, Eta Pos
	if (!write_word(CH_ADDR(L1_POS_CTP7_ID, 0, 0), 0xC0 + 2 * phi))
		RETURN_ERROR("Unable to access registers");
	if (!write_word(CH_ADDR(L1_POS_CTP7_ID, 0, 1), 0xC0 + (2 * phi) + 1))
		RETURN_ERROR("Unable to access registers");*/

        // Invalidate configuration string as part of hard reset
	std::string confstr = "null";
	FILE *fd = fopen("/tmp/UCT2016Layer1-configdata.txt", "w");
	if (!fd)
		RETURN_ERROR("Unable to open configdata file");
	if (fwrite(confstr.data(), confstr.size(), 1, fd) != 1)
		RETURN_ERROR("Unable to write to configdata file");
	fclose(fd);


	response->set_word("result", 1);
}


// virtual bool setTxPower(bool enabled);
void setTxPower(const RPCMsg *request, RPCMsg *response)
{
	PARAM_WORD(enabled);

	int ret = system(stdsprintf("/bin/txpower %s #&>/dev/null", (enabled ? "enable" : "disable")).c_str());
	if (WEXITSTATUS(ret) != 0)
	{
		LOGGER->log_message(LogManager::ERROR, stdsprintf("Failed to set txpower, txpower returned %d", WEXITSTATUS(ret)));
		response->set_word("error", 1);
		return;
	}
	LOGGER->log_message(LogManager::INFO, stdsprintf("Set txpower %s.  txpower returned %d", (enabled ? "enabled" : "disabled"), WEXITSTATUS(ret)));
	response->set_word("result", 1);
}

// virtual bool configRefClk(void);
void configRefClk(const RPCMsg *request, RPCMsg *response)
{

       int ret = system("clockinit 320 160  B1 A0 A0 B1 &>/dev/null");
	
	if (WEXITSTATUS(ret) != 0)
	{
		LOGGER->log_message(LogManager::ERROR, stdsprintf("Failed to configure reference clocks, clockinit returned %d", WEXITSTATUS(ret)));
		response->set_word("error", 1);
		return;
	}
	LOGGER->log_message(LogManager::INFO, stdsprintf("Configure reference clocks. clockinit returned %d", WEXITSTATUS(ret)));
	response->set_word("result", 1);
}


// virtual bool configMGTs(void);
void configMGTs(const RPCMsg *request, RPCMsg *response)
{
	// Power on and reset QPLLs
	for (uint32_t qpll = 0; qpll < 16; qpll++)
	{
		if (!write_word(MGT_CH_ADDR(QPLL_PD, qpll), 0)) RETURN_ERROR("Unable to access registers");
		
		// Reset QPLL
		if (!write_word(MGT_CH_ADDR(QPLL_RST, qpll), 1)) RETURN_ERROR("Unable to access registers");
	}

	// MGT TX Config and reset
	for (uint32_t link = 0; link < 64; link++)
	{
		// Power On, Normal Polarity
		if (!write_word(MGT_CH_ADDR(TX_MAIN_CTRL, link), 0)) RETURN_ERROR("Unable to access registers");
		if (!write_word(MGT_CH_ADDR(TX_RST_CTRL, link), 1)) RETURN_ERROR("Unable to access registers");
		if (!write_word(MGT_CH_ADDR(TX_DRV_CTRL, link), 0x8)) RETURN_ERROR("Unable to access registers");
		if (!write_word(MGT_CH_ADDR(LOOPBACK, link), 0)) RETURN_ERROR("Unable to access registers");
	}

	// MGT RX Config (ch 11 inverted) and reset
	for (uint32_t link = 0; link < 64; link++)
	{
		//CTP7 Ch11 (CXP0) has RX channel inverted
		if (link != 11) {
			// Power On, Normal Polarity, LPM mode
			if (!write_word(MGT_CH_ADDR(RX_MAIN_CTRL, link), 0x20)) RETURN_ERROR("Unable to access registers");
		}
		else {
			// Power On, Inverted Polarity, LPM mode
			if (!write_word(MGT_CH_ADDR(RX_MAIN_CTRL, link), 0x22)) RETURN_ERROR("Unable to access registers");
		}
		
		if (!write_word(MGT_CH_ADDR(RX_RST_CTRL, link), 1)) RETURN_ERROR("Unable to access registers");
	}

	response->set_word("result", 1);
}

// virtual bool algoReset(void);
void algoReset(const RPCMsg *request, RPCMsg *response)
{
	if (!write_word(C_ALGO_RESET, 0)) RETURN_ERROR("Unable to access registers");	
	if (!write_word(C_ALGO_RESET, 1)) RETURN_ERROR("Unable to access registers");
	if (!write_word(C_ALGO_RESET, 0)) RETURN_ERROR("Unable to access registers");
	
	response->set_word("result", 1);
}

// virtual bool maskRXLink(uint32_t link, bool mask);
void maskRXLink(const RPCMsg *request, RPCMsg *response)
{
	PARAM_WORD(link);
	PARAM_WORD(mask);
	
	//if (!write_word(CH_ADDR(C_LINK_MASK_CH0_ADDR, RXLinkMap[link]), mask)) RETURN_ERROR("Unable to access registers");
	response->set_word("result", 1);
}

// virtual bool alignInputLinks(uint32_t alignLat);



// virtual bool configureRXLinkBuffer(bool CAP_nPB);
void configureRXLinkBuffer(const RPCMsg *request, RPCMsg *response)
{
	PARAM_WORD(CAP_nPB);

	if (!write_word(C_LINK_BUFFER_RX_CAP_nPB, CAP_nPB)) RETURN_ERROR("Unable to access registers");	

	response->set_word("result", 1);
}

// virtual bool configureTXLinkBuffer(bool CAP_nPB);
void configureTXLinkBuffer(const RPCMsg *request, RPCMsg *response)
{
	PARAM_WORD(CAP_nPB);

	if (!write_word(C_LINK_BUFFER_TX_CAP_nPB, CAP_nPB)) RETURN_ERROR("Unable to access registers");	

	response->set_word("result", 1);
}

// virtual bool reqRXLinkBufferCapture(void);
void reqRXLinkBufferCapture(const RPCMsg *request, RPCMsg *response)
{

	if (!write_word(C_LINK_BUFFER_RX_CAP_ARM, 0)) RETURN_ERROR("Unable to access registers");	
	if (!write_word(C_LINK_BUFFER_RX_CAP_ARM, 1)) RETURN_ERROR("Unable to access registers");	
	if (!write_word(C_LINK_BUFFER_RX_CAP_ARM, 0)) RETURN_ERROR("Unable to access registers");	

	response->set_word("result", 1);
}

// virtual bool reqTXLinkBufferCapture(void);
void reqTXLinkBufferCapture(const RPCMsg *request, RPCMsg *response)
{

	if (!write_word(C_LINK_BUFFER_TX_CAP_ARM, 0)) RETURN_ERROR("Unable to access registers");	
	if (!write_word(C_LINK_BUFFER_TX_CAP_ARM, 1)) RETURN_ERROR("Unable to access registers");	
	if (!write_word(C_LINK_BUFFER_TX_CAP_ARM, 0)) RETURN_ERROR("Unable to access registers");	

	response->set_word("result", 1);
}

// virtual bool getConfiguration(std::string &output);
void getConfiguration(const RPCMsg *request, RPCMsg *response)
{
	FILE *fd = fopen("/tmp/CTP7Gen0-configdata.txt", "r");
	if (!fd)
		RETURN_ERROR("Unable to open configdata file");

	std::string output;
	char data[1024];
	while (1)
	{
		int ret = fread(data, 1, 1024, fd);
		if (ret < 0)
			RETURN_ERROR("Unable to write to configdata file");
		if (ret == 0)
			break;
		output += std::string(data, ret);
	}
	fclose(fd);
	LOGGER->log_message(LogManager::INFO, stdsprintf("Data copied from Configuration file "));
	response->set_string("result", output);
}

// virtual bool setConfiguration(std::string input);
void setConfiguration(const RPCMsg *request, RPCMsg *response)
{
	REQUIRE_PARAMETER("input");
	std::string input = request->get_string("input");
	LOGGER->log_message(LogManager::INFO, stdsprintf("Data to be written to configuration file %s ", input.data()));
	FILE *fd = fopen("/tmp/CTP7Gen0-configdata.txt", "w");
	if (!fd)
		{
		LOGGER->log_message(LogManager::ERROR, "Unable to openfile");
		RETURN_ERROR("Unable to open configdata file");
		}

	if (fwrite(input.data(), input.size(), 1, fd) != 1)
		{
		LOGGER->log_message(LogManager::ERROR, "Unable to write to config data file");
		RETURN_ERROR("Unable to write to configdata file");
		}
	fclose(fd);
	LOGGER->log_message(LogManager::INFO, stdsprintf("%u bytes of configuration data written", input.size()));
	response->set_word("result", 1);
}





// virtual bool getInputLinkBuffer(bool negativeEta, InputLink link, std::vector<uint32_t> &selectedData);
void getInputLinkBuffer(const RPCMsg *request, RPCMsg *response)
{
	PARAM_WORD(link);
	VALIDATE_INPUT_LINK(link);
	std::vector<uint32_t> data;
	if (!bram_read(BRAM_CH_ADDR(INPUT_BRAM_0, link), 1024, data))
		RETURN_ERROR("Unable to access BRAM");
	response->set_word_array("result", data);
}

// virtual bool setInputLinkBuffer(bool negativeEta, InputLink link, const std::vector<uint32_t> &selectedData);
void setInputLinkBuffer(const RPCMsg *request, RPCMsg *response)
{
	PARAM_WORD(link);
	if (!request->get_key_exists("selectedData"))
		RETURN_ERROR("Missing required parameter: selectedData");
	std::vector<uint32_t> selectedData = request->get_word_array("selectedData");
	VALIDATE_INPUT_LINK(link);
	VALIDATE_RANGE(selectedData.size(), 1024, 1024);
	if (!bram_write_verify(BRAM_CH_ADDR(INPUT_BRAM_0, link), selectedData))
		RETURN_ERROR("Unable to access BRAM");
	response->set_word("result", 1);
}

// virtual bool getOutputLinkBuffer(bool negativeEta, OutputLink link, std::vector<uint32_t> &data);
void getOutputLinkBuffer(const RPCMsg *request, RPCMsg *response)
{
	PARAM_WORD(link);
	VALIDATE_OUTPUT_LINK(link);

	std::vector<uint32_t> selectedData;
	if (!bram_read(BRAM_CH_ADDR(OUTPUT_BRAM_0, link), 1024, selectedData))
		RETURN_ERROR("Unable to acccess BRAM");
	response->set_word_array("result", selectedData);
}

// virtual bool setOutputLinkBuffer(bool negativeEta, OutputLink link, const std::vector<uint32_t> &data); // This function is for testing purpose only
void setOutputLinkBuffer(const RPCMsg *request, RPCMsg *response)
{
	PARAM_WORD(link);
	if (!request->get_key_exists("selectedData"))
		RETURN_ERROR("Missing required parameter: selectedData");
	std::vector<uint32_t> selectedData = request->get_word_array("selectedData");
	VALIDATE_INPUT_LINK(link);
	VALIDATE_RANGE(selectedData.size(), 1024, 1024);
	if (!bram_write_verify(BRAM_CH_ADDR(OUTPUT_BRAM_0, link), selectedData))
		RETURN_ERROR("Unable to access BRAM");
	response->set_word("result", 1);
}

// virtual bool getFWInfo(FWInfo &fwInfo);
void getFWInfo(const RPCMsg *request, RPCMsg *response)
{
	uint32_t value=1;
	LOGGER->log_message(LogManager::INFO, stdsprintf("Connected to getFWinfo module  "));

	//if (!read_word(FW_BUILD_DATE, value))
		//RETURN_ERROR("Unable to access registers");
	response->set_word("buildTimestamp", value);

	//if (!read_word(FW_GIT_HASH_CODE, value))
		//RETURN_ERROR("Unable to access registers");
	response->set_word("gitHashCode", value);

	//if (!read_word(FW_GIT_HASH_DIRTY, value))
		//RETURN_ERROR("Unable to access registers");
	response->set_word("gitHashDirty", value);

	//if (!read_word(FW_VERSION, value))
		//RETURN_ERROR("Unable to access registers");
	response->set_word("version", value);

	//if (!read_word(FW_PROJECT_CODE, value))
	//	RETURN_ERROR("Unable to access registers");
	response->set_word("projectCode", value);

	//if (!read_word(FW_UPTIME, value))
	//	RETURN_ERROR("Unable to access registers");
	response->set_word("uptime", value);

}

void getModuleBuildInfo(const RPCMsg *request, RPCMsg *response)
{
	response->set_string("result", BUILD_INFO);
}

extern "C" {
	const char *module_version_key = "UCT2016Layer1 v1.1.0";
	int module_activity_color = 5;
	void module_init(ModuleManager *modmgr)
	
	{	
		if (memsvc_open(&memsvc) != 0)
		{
			LOGGER->log_message(LogManager::ERROR, stdsprintf("Unable to connect to memory service: %s", memsvc_get_last_error(memsvc)));
			LOGGER->log_message(LogManager::ERROR, "Unable to load module");
			return;
		}

		modmgr->register_method("UCT2016Layer1", "ping", ping);
		modmgr->register_method("UCT2016Layer1", "setRunNumber", setRunNumber);
		modmgr->register_method("UCT2016Layer1", "hardReset", hardReset);
		modmgr->register_method("UCT2016Layer1","setDAQConfig", setDAQConfig);
		modmgr->register_method("UCT2016Layer1","getDAQStatus", getDAQStatus);
		modmgr->register_method("UCT2016Layer1", "getFWInfo", getFWInfo);
		
		modmgr->register_method("UCT2016Layer1","alignTTCDecoder", alignTTCDecoder);

		modmgr->register_method("UCT2016Layer1","setInputLinkTowerMask", setInputLinkTowerMask);
		modmgr->register_method("UCT2016Layer1","getInputLinkTowerMask", getInputLinkTowerMask);
		modmgr->register_method("UCT2016Layer1", "setInputLinkAlignmentMask", setInputLinkAlignmentMask);
		modmgr->register_method("UCT2016Layer1", "setTMTCycle", setTMTCycle);

		modmgr->register_method("UCT2016Layer1", "alignInputLinks", alignInputLinks);
		modmgr->register_method("UCT2016Layer1", "alignOutputLinks", alignOutputLinks);
		modmgr->register_method("UCT2016Layer1", "resetInputLinkChecksumErrorCounters", resetInputLinkChecksumErrorCounters);
		modmgr->register_method("UCT2016Layer1", "resetInputLinkBX0ErrorCounters", resetInputLinkBX0ErrorCounters);
		modmgr->register_method("UCT2016Layer1", "getInputLinkAlignmentStatus", getInputLinkAlignmentStatus);
		modmgr->register_method("UCT2016Layer1", "getInputLinkStatus", getInputLinkStatus);
		modmgr->register_method("UCT2016Layer1", "resetInputLinkDecoders", resetInputLinkDecoders);
		modmgr->register_method("UCT2016Layer1", "getTTCStatus", getTTCStatus);
		modmgr->register_method("UCT2016Layer1", "getTTCBGoCmdCnt", getTTCBGoCmdCnt);


		modmgr->register_method("UCT2016Layer1", "getInputLinkLUT", getInputLinkLUT);
		modmgr->register_method("UCT2016Layer1", "setInputLinkLUT", setInputLinkLUT);

		modmgr->register_method("UCT2016Layer1", "getInputLinkLUT2S", getInputLinkLUT2S);
		modmgr->register_method("UCT2016Layer1", "setInputLinkLUT2S", setInputLinkLUT2S);
		
		modmgr->register_method("UCT2016Layer1", "getInputLinkLUTHcalFb", getInputLinkLUTHcalFb);
		modmgr->register_method("UCT2016Layer1", "setInputLinkLUTHcalFb", setInputLinkLUTHcalFb);
		



		modmgr->register_method("UCT2016Layer1","testUptime", testUptime);
		modmgr->register_method("UCT2016Layer1","setConfiguration", setConfiguration);
		modmgr->register_method("UCT2016Layer1","getConfiguration", getConfiguration);
		
		modmgr->register_method("UCT2016Layer1", "getInputLinkBuffer", getInputLinkBuffer);
		modmgr->register_method("UCT2016Layer1", "setInputLinkBuffer", setInputLinkBuffer);
		modmgr->register_method("UCT2016Layer1", "getOutputLinkBuffer", getOutputLinkBuffer);
		modmgr->register_method("UCT2016Layer1", "setOutputLinkBuffer", setOutputLinkBuffer);
		
		modmgr->register_method("UCT2016Layer1", "setTxPower", setTxPower);
//		modmgr->register_method("UCT2016Layer1", "configRefClk", configRefClk);
//		modmgr->register_method("UCT2016Layer1", "configMGTs", configMGTs);

//		modmgr->register_method("UCT2016Layer1", "algoReset", algoReset);
// 	 	modmgr->register_method("UCT2016Layer1", "maskRXLink", maskRXLink);
//		modmgr->register_method("UCT2016Layer1", "alignInputLinks", alignInputLinks);
		modmgr->register_method("UCT2016Layer1", "configureRXLinkBuffer", configureRXLinkBuffer);
		modmgr->register_method("UCT2016Layer1", "configureTXLinkBuffer", configureTXLinkBuffer);
		modmgr->register_method("UCT2016Layer1", "reqRXLinkBufferCapture", reqRXLinkBufferCapture);
		modmgr->register_method("UCT2016Layer1", "reqTXLinkBufferCapture", reqTXLinkBufferCapture);
		modmgr->register_method("UCT2016Layer1", "getModuleBuildInfo", getModuleBuildInfo);
		}
	}
}	

