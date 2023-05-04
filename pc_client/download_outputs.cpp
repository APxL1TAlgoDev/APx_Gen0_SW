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

const std::string data_path = "data";

int main(int argc, char *argv[])
{
     bool rc; // RPC return code
         
	std::vector<CTP7AlgoClient*> cards;

	int phi = atoi(argv[1]);

	CTP7AlgoClient * card;

	try
	{
		card = new CTP7AlgoClient(phi, "CTP7Map.xml");
	}
	catch (std::runtime_error &e)
	{
        std::cout << "Couldnt connect to phi: " << phi <<  e.what() << std::endl;
        return -1;
	}

	// Request RT buffer capture
	card->reqTXLinkBufferCapture();

	std::map<int, std::vector<uint32_t> > output_link_data;

	for (int link = 0; link < 36; link++)
	{
		// download TX BRAMs
		rc = card->getOutputLinkBuffer(link, output_link_data[link]);
        if (rc == false) {
              std::cout << "getOutputLinkBuffer fails for link " << link << std::endl;
              return -1;
        }
	}
	
	FILE *fd = fopen((data_path + "/" +  argv[2]).c_str(), "w");
	if (!fd)
	{
		printf("Error writing output file.\n");
		return -1;
	}

	fprintf(fd, "===================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================== \n");
	fprintf(fd, "WordCnt             LINK_00               LINK_01                                        \n");
	fprintf(fd, "#BeginData\n");

	uint32_t word32_lsw, word32_msw;
	uint64_t word64;

	for (int word = 0; word < 1024; word++)
	{
		fprintf(fd, "0x%04x   ", word);
		for (int link = 0; link < 36; link++)
		{
		         word32_lsw = output_link_data[link][word];
			//word32_lsw = output_link_data[link][word*2];
			//word32_msw = output_link_data[link][word*2+1];

			//word64 = word32_lsw;
			//word64 |= (uint64_t)word32_msw << 32;

			fprintf(fd, "0x%08llx    ", word32_lsw);
		}
		fprintf(fd, "\n");
	}

	fclose(fd);

	return 0;
}

