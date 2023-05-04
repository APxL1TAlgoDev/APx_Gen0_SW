#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <wiscrpcsvc.h>
#include "CTP7AlgoClient.hh"

// Including the files included by tinyxml2.h here as well, to make sure we
// don't get them subsequently imported with the wrong visibility and having it
// matter somehow, later.  (Hail header include guards!)
#include <cctype>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#pragma GCC visibility push(hidden)
#include "tinyxml2.h"
#pragma GCC visibility pop

using namespace wisc;

static const std::string module_name = "S3_Summ";
static const std::string module_key = "S3_Summ v1.1.0";

#define RPC_CATCH_RETURN catch (wisc::RPCSvc::RPCException &e) {  return false; }

#define TYPE_GENERIC_CALL(rtype, result, rpcreq) \
    do { \
        if (!this->checkConnection()) \
            return false; \
        try { \
            RPCMsg rpcrsp = this->rpc.call_method(rpcreq); \
            if (rpcrsp.get_key_exists("error")) \
                return false; \
            if (rpcrsp.get_key_exists("result")) \
                result = rpcrsp.get_ ## rtype("result"); \
            return true; \
        } \
        RPC_CATCH_RETURN; \
    } while (0)

#define WORD_GENERIC_CALL(result, rpcreq) TYPE_GENERIC_CALL(word, result, rpcreq)
#define GENERIC_CALL(rpcreq) WORD_GENERIC_CALL(__attribute__((unused)) uint32_t discard, rpcreq)

CTP7AlgoClient::CTP7AlgoClient(uint32_t phi, const std::string connect_string, enum ConnectStringType connect_type)
	: phi(phi)
{
	// Find out the correct address to access.

	if (connect_type == CONNECTSTRING_PHIMAPXML) {
		tinyxml2::XMLDocument phiMapDoc;
		if (phiMapDoc.LoadFile(connect_string.c_str()))
			throw std::runtime_error("Unable to read address configuration XML");

		tinyxml2::XMLElement *config_elem = phiMapDoc.FirstChildElement("layer1_hw_config");
		if (!config_elem)
			throw std::runtime_error("Unable to read address configuration XML");

		this->hostname = "";

		for (tinyxml2::XMLElement *phi_elem = config_elem->FirstChildElement("phi_config"); phi_elem; phi_elem = phi_elem->NextSiblingElement("phi_config"))
		{
			int phival;
			if (phi_elem->QueryIntAttribute("phi", &phival) != 0)
				continue;
			if (phival != phi)
				continue;
			tinyxml2::XMLElement *ba_elem = phi_elem->FirstChildElement("board_address");
			if (!ba_elem)
				continue;
			const char *ba_text = ba_elem->GetText();
			if (!ba_text)
				continue;
			this->hostname = ba_text;
			break;
		}

		if (!this->hostname.size())
			throw std::runtime_error("Unable to locate address configuration for the requested phi");
	}
	else if (connect_type == CONNECTSTRING_IPHOST) {
		this->hostname = connect_string;
	}
	else {
		throw std::runtime_error("Invalid connect_type provided");
	}

	try
	{
		this->rpc.connect(this->hostname);
		if (!this->rpc.load_module(module_name, module_key))
			throw std::runtime_error("Unable to load the appropriate rpcsvc module");
	}
	catch (wisc::RPCSvc::RPCException &e)
	{
		throw std::runtime_error(std::string("RPCSvc Exception: ") + e.message);
	}
}


CTP7AlgoClient::~CTP7AlgoClient()
{

}

bool CTP7AlgoClient::checkConnection()
{
	try
	{
		this->rpc.call_method(RPCMsg(module_name + ".ping"));
		return true;
	}
	catch (wisc::RPCSvc::RPCException &e)
	{
		//
	}

	// Bad connection.  Let's try reconnecting.
	this->rpc.disconnect();

	try
	{
		this->rpc.connect(this->hostname);
		if (!this->rpc.load_module(module_name, module_key))
			return false;
		return true;
	}
	catch (wisc::RPCSvc::RPCException &e)
	{
		return false;
	}
}

bool CTP7AlgoClient::hardReset(std::string bitstream)
{
	GENERIC_CALL(RPCMsg(module_name + ".hardReset")
	             .set_string("bitstream", bitstream)
	             .set_word("phi", this->phi)
	            );
}

bool CTP7AlgoClient::setTxPower(bool enabled)
{
	GENERIC_CALL(RPCMsg(module_name + ".setTxPower")
	             .set_word("enabled", enabled)
	            );
}

bool CTP7AlgoClient::configRefClk(void)
{
	GENERIC_CALL(RPCMsg(module_name + ".configRefClk"));
}

bool CTP7AlgoClient::configMGTs(void)
{
	GENERIC_CALL(RPCMsg(module_name + ".configMGTs"));
}

bool CTP7AlgoClient::algoReset(void)
{
	GENERIC_CALL(RPCMsg(module_name + ".algoReset"));
}

bool CTP7AlgoClient::maskRXLink(uint32_t link, bool mask)
{
	GENERIC_CALL(RPCMsg(module_name + ".maskRXLink")
	             .set_word("link", link)
	             .set_word("mask", mask)
	            );
}

bool CTP7AlgoClient::alignInputLinks(uint32_t alignLat)
{
	GENERIC_CALL(RPCMsg(module_name + ".alignInputLinks")
	             .set_word("alignLat", alignLat)
	            );
}

bool CTP7AlgoClient::configureRXLinkBuffer(bool CAP_nPB)
{
	std::cout << "calling here  configureRXLinkBuffer "  << std::endl;


	GENERIC_CALL(RPCMsg(module_name + ".configureRXLinkBuffer")
	             .set_word("CAP_nPB", CAP_nPB)
	            );
}

bool CTP7AlgoClient::configureTXLinkBuffer(bool CAP_nPB)
{
	GENERIC_CALL(RPCMsg(module_name + ".configureTXLinkBuffer")
	             .set_word("CAP_nPB", CAP_nPB)
	            );
}

bool CTP7AlgoClient::reqRXLinkBufferCapture(void)
{
	std::cout << "calling here  reqRXLinkBuffercapture "  << std::endl;
	GENERIC_CALL(RPCMsg(module_name + ".reqRXLinkBufferCapture"));
}

bool CTP7AlgoClient::reqTXLinkBufferCapture(void)
{

	std::cout << "calling here  TXLinkBuffercapture "  << std::endl;
	GENERIC_CALL(RPCMsg(module_name + ".reqTXLinkBufferCapture"));
}

bool CTP7AlgoClient::getConfiguration(std::string &output)
{
	TYPE_GENERIC_CALL(string, output, RPCMsg(module_name + ".getConfiguration"));
}

bool CTP7AlgoClient::setConfiguration(std::string input)
{
	GENERIC_CALL(RPCMsg(module_name + ".setConfiguration")
	             .set_string("input", input)
	            );
}


bool CTP7AlgoClient::getInputLinkBuffer(int link, std::vector<uint32_t> &selectedData)
{
	TYPE_GENERIC_CALL(word_array, selectedData,
	                  RPCMsg(module_name + ".getInputLinkBuffer")
	                  .set_word("link", link)
	                 );
}

bool CTP7AlgoClient::setInputLinkBuffer( int link, const std::vector<uint32_t> &selectedData)
{
	
	GENERIC_CALL(RPCMsg(module_name + ".setInputLinkBuffer")
	             .set_word("link", link)
	             .set_word_array("selectedData", selectedData)
	            );
}


bool CTP7AlgoClient::getOutputLinkBuffer(int link, std::vector<uint32_t> &selectedData)
{
	TYPE_GENERIC_CALL(word_array, selectedData,
	                  RPCMsg(module_name + ".getOutputLinkBuffer")
	                  .set_word("link", link)
	                 );
}

bool CTP7AlgoClient::setOutputLinkBuffer( int link, const std::vector<uint32_t> &selectedData)
{
	GENERIC_CALL(RPCMsg(module_name + ".setOutputLinkBuffer")
	             .set_word("link", link)
	             .set_word_array("selectedData", selectedData)
	            );
}


bool CTP7AlgoClient::getModuleBuildInfo(std::string &value)
{
	TYPE_GENERIC_CALL(string, value, RPCMsg(module_name + ".getModuleBuildInfo"));
}
