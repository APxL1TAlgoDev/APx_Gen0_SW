#ifndef CTP7AlgoClient_hh
#define CTP7AlgoClient_hh

//
// This header file specifies functional interface to the
// Upgraded Calorimeter Trigger Layer-1 CTP7.
//
// The ZYNQ based softare encapsulates all details of the
// firmware operation so addresses, internal state transition
// details, sequencing of commands, etc. are not exposed.
//

#include <stdint.h>
#include <vector>
#include <string>

#include <wiscrpcsvc.h>

class CTP7AlgoClient
{

public:

	enum ConnectStringType {
		CONNECTSTRING_PHIMAPXML = 0,
		CONNECTSTRING_IPHOST    = 1
	};
	CTP7AlgoClient(uint32_t phi, const std::string connect_string, enum ConnectStringType connect_type = CONNECTSTRING_PHIMAPXML);

	virtual ~CTP7AlgoClient();


	virtual bool checkConnection();

	// Resets
	// These functions reset the CTP7 firmware to a known status

	virtual bool hardReset(std::string bitstream = "test.bit");

	//
	virtual bool getPhi(uint32_t &phi)
	{
		phi = this->phi;
		return true;
	};


	// Configuration string for conveniently storing user information
	// This is just an arbitrary string that can be set and retrieved
	// and is left to the user to maintain -- it has no bearing on the
	// CTP7 or its functionality

	virtual bool getConfiguration(std::string &user_config_string);
	virtual bool setConfiguration(std::string user_config_string);


	virtual bool getInputLinkBuffer( int link, std::vector<uint32_t> &selectedData);
        virtual bool setInputLinkBuffer( int link, const std::vector<uint32_t> &selectedData);


	virtual bool getOutputLinkBuffer( int link, std::vector<uint32_t> &data);
	virtual bool setOutputLinkBuffer( int link, const std::vector<uint32_t> &data); // This function is for testing purpose only


	virtual bool setTxPower( bool enabled );
	virtual bool configRefClk( void );
	virtual bool configMGTs( void );


	virtual bool algoReset( void );
	virtual bool maskRXLink(uint32_t link, bool mask);
	virtual bool alignInputLinks(uint32_t alignLat);
	virtual bool configureRXLinkBuffer(bool CAP_nPB);
	virtual bool configureTXLinkBuffer(bool CAP_nPB);
	virtual bool reqRXLinkBufferCapture( void );
	virtual bool reqTXLinkBufferCapture( void );



	// Get a string containing build information about the RPCSvc module.
	virtual bool getModuleBuildInfo(std::string &value);

protected:
	uint32_t phi;
	std::string hostname;
	wisc::RPCSvc rpc;

private:
	/* We disable copying and assignment, because it is complex to maintain
	 * an extra internal structure to handle the management of shared socket
	 * connections to the server, and giving each object its own separate
	 * connection will quickly explode unless the library user is
	 * specifically aware of it and careful in their handling.
	 *
	 * Use pointers or single instances.
	 *
	 * It is easy to just allocate with
	 *   CTP7AlgoClient *card = new CTP7AlgoClient(0);
	 *
	 * or to still use
	 *   CTP7AlgoClient card(0);
	 * but only pass around pointers or references after that.
	 */
	CTP7AlgoClient(const CTP7AlgoClient &v);
	CTP7AlgoClient& operator=(const CTP7AlgoClient &v);
};

#endif

