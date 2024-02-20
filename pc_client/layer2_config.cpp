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

	CTP7AlgoClient * card;

	try
	{
		card = new CTP7AlgoClient(phi, "CTP7Map.xml");
	}
	catch (std::runtime_error &e)
	{
		printf("Couldnt connect to phi %d: %s\n", phi, e.what());
	}

	for (int lnk=0; lnk<48; lnk++)
	{
		rc = card->maskRXLink(lnk, 0); 
		if (rc != 1)
		{
			printf("maskRXLink failed. \n\r");
			return 1;
		}
	}
	
	rc = card->alignInputLinks(250); 
	if (rc != 1)
	{
		printf("configRefClk failed. \n\r");
		return 1;
	}

	rc = card->configureRXLinkBuffer(1); 
	if (rc != 1)
	{
		printf("configureRXLinkBuffer failed. \n\r");
		return 1;
	}

	rc = card->configureTXLinkBuffer(1); 
	if (rc != 1)
	{
		printf("configureTXLinkBuffer failed. \n\r");
		return 1;
	}

	rc = card->algoReset(); 
	if (rc != 1)
	{
		printf("algoReset failed. \n\r");
		return 1;
	}

	return 0;
}
