#include <stdexcept>
#include <stdio.h>
#include <CTP7AlgoClient.hh>

#define TEST_PHI_FIRST  0
#define TEST_PHI_LAST   0

#define TEXT_GREEN  "\x1b[32m"
#define TEXT_RED    "\x1b[31;1m"
#define TEXT_RESET  "\x1b[0m"

#define TEST(x) \
    do { \
        if (x) { passed++; last_test_passed = true; printf(TEXT_GREEN "PASS:" TEXT_RESET "\t%s\n", #x); } \
        else { failed++; last_test_passed = false; printf(TEXT_RED "FAIL:" TEXT_RESET "\t%s\n", #x); printf(TEST_INDENT "Test failed on line %u.\n", __LINE__); } \
    } while (0)
#define TEST_INDENT "\t  "

void test_card(CTP7AlgoClient &card, int &passed, int &failed)
{
	std::vector<uint32_t> bram_test_pattern;
	for (int i = 0; i < 1024; i++)
		bram_test_pattern.push_back(i);


	bool last_test_passed;
	{
		TEST(card.checkConnection());
		printf("\n");
	}

	{
		TEST(card.softReset());
		printf("\n");
	}




	{
		TEST(card.setInputLinkBuffer(0, bram_test_pattern));
		TEST(card.setInputLinkBuffer(1, bram_test_pattern));
		printf("\n");
	}
	{
		std::vector<uint32_t> bram_data;
		TEST(card.getInputLinkBuffer(0, bram_data));
		if (last_test_passed)
			TEST(bram_data == bram_test_pattern);
		TEST(card.getInputLinkBuffer(1, bram_data));
		if (last_test_passed)
			TEST(bram_data == bram_test_pattern);
		printf("\n");
	}
	{
		TEST(card.setOutputLinkBuffer(0, bram_test_pattern));
		TEST(card.setOutputLinkBuffer(1, bram_test_pattern));
		printf("\n");
	}

	{
		int tmp_reg; 

		for (int idx = 0; idx < 32; idx++)
		{
			TEST(card.setConfigReg(idx, idx + 32));
			TEST(card.getConfigReg(1, tmp_reg));
		}
		printf("\n");

	}



	{
		std::vector<uint32_t> bram_data;
		TEST(card.getOutputLinkBuffer(0, bram_data));
		if (last_test_passed)
			TEST(bram_data == bram_test_pattern);
		TEST(card.getOutputLinkBuffer( 1, bram_data));
		if (last_test_passed)
			TEST(bram_data == bram_test_pattern);
		printf("\n");
	}




	{
		std::string value;
		TEST(card.getModuleBuildInfo(value));
		if (last_test_passed)
		{
			TEST(value.size());
			if (last_test_passed)
				printf("\n%s\n", value.c_str());
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	std::vector<CTP7AlgoClient*> cards;
	for (int phi = TEST_PHI_FIRST; phi <= TEST_PHI_LAST; phi++)
	{
		printf("Connecting to card=%d\n", phi);
		fflush(stdout);
		try
		{
			cards.push_back(new CTP7AlgoClient(phi, "CTP7Map.xml"));
		}
		catch (std::runtime_error &e)
		{
			printf("Couldnt connect to phi %d: %s\n", phi, e.what());
		}
	}

	int passed = 0;
	int failed = 0;

	for (std::vector<CTP7AlgoClient*>::iterator it = cards.begin(), endit = cards.end(); it != endit; it++)
		test_card(**it, passed, failed);

	printf("Passed: %d, Failed: %d\n", passed, failed);
	return (failed ? 1 : 0);
}
