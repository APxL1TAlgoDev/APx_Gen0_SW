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
	std::vector<CTP7AlgoClient*> cards;

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

	// Request RX buffer capture
	card->reqRXLinkBufferCapture();

	std::map<int, std::vector<uint32_t> > input_link_data;

	for (int link = 0; link < 48; link++)
	{
		// download RX BRAMs
		card->getInputLinkBuffer(link, input_link_data[link]);
	}
	
	FILE *fd = fopen((data_path + "/" +  argv[2]).c_str(), "w");
	if (!fd)
	{
		printf("Error writing output file.\n");
		return -1;
	}

	fprintf(fd, "===================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================== \n");
	fprintf(fd, "WordCnt             LINK_00               LINK_01               LINK_02               LINK_03               LINK_04               LINK_05               LINK_06               LINK_07               LINK_08               LINK_09               LINK_10               LINK_11               LINK_12               LINK_13               LINK_14               LINK_15               LINK_16               LINK_17               LINK_18               LINK_19               LINK_20               LINK_21               LINK_22               LINK_23               LINK_24               LINK_25               LINK_26               LINK_27               LINK_28               LINK_29               LINK_30               LINK_31               LINK_32               LINK_33               LINK_34               LINK_35               LINK_36               LINK_37               LINK_38               LINK_39               LINK_40               LINK_41               LINK_42               LINK_43               LINK_44               LINK_45               LINK_46               LINK_47 \n");
	fprintf(fd, "#BeginData\n");

	uint32_t word32_lsw, word32_msw;
	uint64_t word64;

	for (int word = 0; word < 512; word++)
	{
		fprintf(fd, "0x%04x   ", word);
		for (int link = 0; link < 48; link++)
		{
			word32_lsw = input_link_data[link][word*2];
			word32_msw = input_link_data[link][word*2+1];

			word64 = word32_lsw;
			word64 |= (uint64_t)word32_msw << 32;

			fprintf(fd, "0x%016llx    ", word64);
		}
		fprintf(fd, "\n");
	}

	fclose(fd);

	return 0;
}

