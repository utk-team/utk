#include "../../parameters/ParamParser.hpp"
#include "../../io/messageStream.hpp"

#include <random>

bool param_help = false;
bool param_verbose = true;
bool param_brute = false;
std::string param_input;
std::string param_output;

std::mt19937 m_mersenneTwister;
double getRandom01()
{
    return (double)m_mersenneTwister()/(double)m_mersenneTwister.max();
}

int initParserRandomization(utk::ParamParser* parser)
{
	parser->addShortOption('h', &param_help, 0, utk::assignBoolTrue, NULL, "<string>\t\tDisplays this help message");
	parser->addShortOption('i', &param_input, 1, utk::assignString, utk::displayString, "<string>\t\tThe input pointsets", "Input");
	parser->addShortOption('o', &param_output, 1, utk::assignString, utk::displayString, "<string>\t\tThe output pointsets", "Output");
	parser->addLongOption((char*)"silent", &param_verbose, 0, utk::assignBoolFalse, utk::displayBool, (char*)"\t\tSilent mode", (char*)"Verbose");
	parser->addLongOption((char*)"brute", &param_brute, 0, utk::assignBoolTrue, utk::displayBool, (char*)"\t\tOutput brute values instead of computing the statistics", (char*)"Brute");
    
    return 0;
}

int dealParamParserRandomization(utk::ParamParser* parser)
{
	if(param_input.empty())
	{
		ERROR("Argument -i required");
		std::cout << parser->getHelp() << std::endl;
		return 0;
	}
	if(param_output.empty())
	{
		ERROR("Argument -o required");
		std::cout << parser->getHelp() << std::endl;
		return 0;
	}
	
	if(param_help)
		std::cout << parser->getHelp() << std::endl;
	
	if(param_verbose)
		parser->displayState();
	
	return 1;
}
