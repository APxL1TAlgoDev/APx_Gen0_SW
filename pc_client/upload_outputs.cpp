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
	uint64_t value32,value32_data;


	std::string dummyLine;

	// skip header lines
	getline(infile, dummyLine);
	getline(infile, dummyLine);
	getline(infile, dummyLine);

	// Read input data from the file
	for (int idx = 0; idx < 1024; idx++)
	{
		infile >> std::hex >> value32;

		for (int i = 0; i < 1; i++)
		{
			infile >> std::hex >> value32_data;
			input_data[i].data.push_back(value32_data);
		}
	}

	return input_data;
}

int main(int argc, char *argv[]) {

    bool rc; // RPC return code

   std::vector<CTP7AlgoClient*> cards;

	int phi;
    std::cout << "Enter the phi for connection: ";
    std::cin >> phi;

	std::string filename;
    std::cout << "Enter the filename: ";
    std::cin >> filename;

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

	std::map<int, input_data_t> output_data;

	try
	{
		output_data = load_file( (data_path + "/" +  filename).c_str()   );
	} 
	catch (std::runtime_error &e)
	{
        std::cout << "Error loading link data for phi " <<  phi <<  e.what() << std::endl;
		return -1 ;
	}

	for (int link = 0; link < 1; link++)
	{
		rc = card->setOutputLinkBuffer(link, output_data[link].data);
        if (rc == false) {
             std::cout << "setOutputLinkBuffer fails for link " << link << std::endl;
             return -1;
        }
	}

	return 0;
}

