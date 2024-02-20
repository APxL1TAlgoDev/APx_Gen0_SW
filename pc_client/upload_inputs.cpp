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

typedef struct
{
	std::vector<uint32_t> data;
} input_data_t;

std::map<int, input_data_t> load_file(std::string path)
{
	std::ifstream infile(path.c_str(), std::fstream::in);

	if (!infile.is_open())
		throw std::runtime_error(std::string("Unable to open input file: ") + path);

	// std map container to store complete LUT info read from the file
	// 28x8182 uint32_t for 2nd Stage LUTs (iEta=[1-28])
	std::map<int, input_data_t> input_data;

	uint64_t value64;
	uint32_t value32, value32_data;


	std::string dummyLine;

	// skip header lines
	getline(infile, dummyLine);
	getline(infile, dummyLine);
	getline(infile, dummyLine);

	// Read input data from the file
	for (int idx = 0; idx <1024; idx++)
	{
		infile >> std::hex >> value32;
	      //  std::cout<<"***************** next rowdata************ "<< idx << std::endl;
	

		for (int i = 0; i <36; i++)
		{
			//infile >> std::hex >> value64;
                        infile >> std::hex >> value32_data;
		//	std::cout<<" data written to link "<<  std::hex << value32_data<< std::endl;
			
			//value32 = uint32_t(value64 & 0xFFFFFFFF);
			input_data[i].data.push_back(value32_data);

			//value32 = (value64 >> 32);
			//input_data[i].data.push_back(value32);
		}
	}

	return input_data;
}

int main(int argc, char *argv[]) {

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

	std::map<int, input_data_t> input_data;

	try
	{
		input_data = load_file( (data_path + "/" +  argv[2]).c_str()   );
	} 
	catch (std::runtime_error &e)
	{
        std::cout << "Error loading link data for phi " <<  phi <<  e.what() << std::endl;
		return -1 ;
	}

	for (int link = 0; link < 36; link++)
	{
		rc = card->setInputLinkBuffer(link, input_data[link].data);
        if (rc == false) {
           //  std::cout << "setInputLinkBuffer fails for link " << link << std::endl;
             return -1;
        }
	}

	return 0;
}

