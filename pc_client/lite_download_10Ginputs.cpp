#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <CTP7AlgoClient.hh>

#include <stdexcept>
#include <stdio.h>
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
    
	CTP7AlgoClient * card;

	try
	{
		card = new CTP7AlgoClient(0, "CTP7Map.xml");
	}
	catch (std::runtime_error &e)
	{
        std::cout << "Couldnt connect to phi: " << 0 <<  e.what() << std::endl;
        return -1;
	}
	

	std::map<int, std::vector<uint32_t> > output_link_data;

	for (int link = 0; link <12; link++)
	{
		//printf("In the loop %d.\n",link);
		// download TX BRAMs
		rc = card->getOutput10GLinkBuffer(link, output_link_data[link]);
        if (rc == false) {
              std::cout << "getOutput LinkBuffer fails for link " << link << std::endl;
              return -1;
        }
	}
	
	FILE *fd = fopen((data_path + "/" +  argv[1]).c_str(), "w");
	if (!fd)
	{
		printf("Error writing output file.\n");
		return -1;
	}

	fprintf(fd, "===================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================== \n");
        //fprintf(fd, "WordCnt             LINK_00               LINK_01               LINK_02               LINK_03               LINK_04               LINK_05               LINK_06               LINK_07               LINK_08               LINK_09               LINK_10               LINK_11               LINK_12               LINK_13               LINK_14               LINK_15               LINK_16               LINK_17               LINK_18               LINK_19               LINK_20               LINK_21               LINK_22               LINK_23               LINK_24               LINK_25               LINK_26               LINK_27               LINK_28               LINK_29               LINK_30               LINK_31               LINK_32               LINK_33               LINK_34               LINK_35              \n");
        fprintf(fd, "WordCnt      LINK_00      LINK_01      LINK_02      LINK_03      LINK_04      LINK_05      LINK_06      LINK_07      LINK_08      LINK_09      LINK_10      LINK_11      LINK_12      \n");
		fprintf(fd, "#BeginData\n");

	
	uint32_t word32_lsw, word32_msw;
	uint64_t word64;

	for (int word = 0; word < 1024; word++)
	{
		fprintf(fd, "0x%04x       ", word);
		for (int link = 0; link < 12; link++)
		{
		         word32_lsw = output_link_data[link][word];
			
			//printf("Before writing data to file %d %d\n",link,word);
			//if (word32_lsw !=0x000000bc && word32_lsw !=0x000000f7 && word32_lsw !=0x00000000){ 
			fprintf(fd, "0x%08x  ", word32_lsw);
			//}
			
		}
		fprintf(fd, "\n");
	}

	fclose(fd);
	printf("Downloading inputs done\n");

	return 0;
}

