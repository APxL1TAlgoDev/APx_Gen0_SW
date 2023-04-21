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

#define CH_ADDR(reg, ch) ((reg) + ((ch)*C_CH_to_CH_ADDR_OFFSET) )
#define MGT_CH_ADDR(reg, ch) ((reg) + ((ch)*MGT_CH_TO_CH_REG_OFFSET))
#define BRAM_CH_ADDR(reg, ch) ((reg) + ((ch)*BRAM_LINK_OFFSET))

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

void ping(const RPCMsg *request, RPCMsg *response)
{
	*response = *request;
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
	std::string binfile = stdsprintf("/tmp/%s.bin", image.c_str());
	std::string bitfile = stdsprintf("/tmp/%s.bit", image.c_str());
	if (access(binfile.c_str(), F_OK) == 0)
		file = binfile;
	else if (access(bitfile.c_str(), F_OK) == 0)
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


	// Invalidate configuration string as part of hard reset
	std::string confstr = "null";
	FILE *fd = fopen("/tmp/CTP7Gen0-configdata.txt", "w");
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
	
	if (!write_word(CH_ADDR(C_LINK_MASK_CH0_ADDR, RXLinkMap[link]), mask)) RETURN_ERROR("Unable to access registers");
	response->set_word("result", 1);
}

// virtual bool alignInputLinks(uint32_t alignLat);
void alignInputLinks(const RPCMsg *request, RPCMsg *response)
{
	PARAM_WORD(alignLat);

	VALIDATE_RANGE(alignLat, 0, 255);

	if (!write_word(C_LINK_ALIGN_LAT, alignLat)) RETURN_ERROR("Unable to access registers");	

	if (!write_word(C_LINK_ALIGN_REQ, 0)) RETURN_ERROR("Unable to access registers");	
	if (!write_word(C_LINK_ALIGN_REQ, 1)) RETURN_ERROR("Unable to access registers");	
	if (!write_word(C_LINK_ALIGN_REQ, 0)) RETURN_ERROR("Unable to access registers");	

	response->set_word("result", 1);
}


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
	response->set_string("result", output);
}

// virtual bool setConfiguration(std::string input);
void setConfiguration(const RPCMsg *request, RPCMsg *response)
{
	REQUIRE_PARAMETER("input");
	std::string input = request->get_string("input");
	FILE *fd = fopen("/tmp/CTP7Gen0-configdata.txt", "w");
	if (!fd)
		RETURN_ERROR("Unable to open configdata file");
	if (fwrite(input.data(), input.size(), 1, fd) != 1)
		RETURN_ERROR("Unable to write to configdata file");
	fclose(fd);
	LOGGER->log_message(LogManager::INFO, stdsprintf("%u bytes of configuration data written", input.size()));
	response->set_word("result", 1);
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

void getModuleBuildInfo(const RPCMsg *request, RPCMsg *response)
{
	response->set_string("result", BUILD_INFO);
}

extern "C" {
	const char *module_version_key = "S3_Summ v1.1.0";
	int module_activity_color = 5;
	void module_init(ModuleManager *modmgr)
	{
		if (memsvc_open(&memsvc) != 0)
		{
			LOGGER->log_message(LogManager::ERROR, stdsprintf("Unable to connect to memory service: %s", memsvc_get_last_error(memsvc)));
			LOGGER->log_message(LogManager::ERROR, "Unable to load module");
			return;
		}

		modmgr->register_method("S3_Summ", "ping", ping);
		modmgr->register_method("S3_Summ", "hardReset", hardReset);

		modmgr->register_method("S3_Summ", "getInputLinkBuffer", getInputLinkBuffer);
		modmgr->register_method("S3_Summ", "setInputLinkBuffer", setInputLinkBuffer);
		modmgr->register_method("S3_Summ", "getOutputLinkBuffer", getOutputLinkBuffer);
		modmgr->register_method("S3_Summ", "setOutputLinkBuffer", setOutputLinkBuffer);
		
//		modmgr->register_method("S3_Summ", "setTxPower", setTxPower);
//		modmgr->register_method("S3_Summ", "configRefClk", configRefClk);
//		modmgr->register_method("S3_Summ", "configMGTs", configMGTs);

//		modmgr->register_method("S3_Summ", "algoReset", algoReset);
// 	 	modmgr->register_method("S3_Summ", "maskRXLink", maskRXLink);
//		modmgr->register_method("S3_Summ", "alignInputLinks", alignInputLinks);
		modmgr->register_method("S3_Summ", "configureRXLinkBuffer", configureRXLinkBuffer);
		modmgr->register_method("S3_Summ", "configureTXLinkBuffer", configureTXLinkBuffer);
		modmgr->register_method("S3_Summ", "reqRXLinkBufferCapture", reqRXLinkBufferCapture);
		modmgr->register_method("S3_Summ", "reqTXLinkBufferCapture", reqTXLinkBufferCapture);
		
		modmgr->register_method("S3_Summ", "getModuleBuildInfo", getModuleBuildInfo);
	}
}

