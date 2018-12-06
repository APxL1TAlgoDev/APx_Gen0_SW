#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <CTP7AlgoClient.hh>


#include <stdexcept>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <map>

int main(int argc, char *argv[])
{

	int rc;
	int phi = atoi(argv[1]);
	//std::string bitstream = argv[2];

	CTP7AlgoClient * card;

	try
	{
		card = new CTP7AlgoClient(phi, "CTP7Map.xml");
	}
	catch (std::runtime_error &e)
	{
		printf("Couldnt connect to phi %d: %s\n", phi, e.what());
	}

	rc = card->setTxPower(1); 
	if (rc != 1)
	{
		printf("setTxPower failed. \n\r");
		return 1;
	}
	
	rc = card->configRefClk(); 
	if (rc != 1)
	{
		printf("configRefClk failed. \n\r");
		return 1;
	}

	rc = card->hardReset(argv[2]); 
	if (rc != 1)
	{
		printf("Failed to load requested bitfile. \n\r");
		return 1;
	}

	rc = card->configMGTs(); 
	if (rc != 1)
	{
		printf("configMGTs failed. \n\r");
		return 1;
	}

	return 0;
}
