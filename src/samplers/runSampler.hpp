#include "../parameters/ParamParser.hpp"
#include "../io/messageStream.hpp"
#include "../pointsets/Pointset.hpp"

unsigned int param_nbrealisations = 1;
unsigned long long int param_nbsamples = 1024;
bool param_display_help = false;
std::string param_output="output_pts.dat";
bool param_verbose = true;
bool param_randcp = false;
bool param_randxor = false;


int initParserSampler(utk::ParamParser* parser)
{
	parser->addShortOption('o', &param_output, 1, utk::assignString, utk::displayString, "[string=output_pts.dat]\tOutput file", "Output");
	
	parser->addShortOption('m', &param_nbrealisations, 1, utk::assignUInt, utk::displayUInt, "[int=1]\t\t\tNumber of poinset realisations", "Nb Realisations");
	parser->addShortOption('n', &param_nbsamples, 1, utk::assignULLInt, utk::displayULLInt, "[ullint=1024]\t\tNumber of samples to generate", "Nb Samples");
	
	parser->addLongOption((char*)"silent", &param_verbose, 0, utk::assignBoolFalse, utk::displayBool, "\t\t\tSilent mode", "Verbose" );
	parser->addShortOption('h', &param_display_help, 0, utk::assignBoolTrue, NULL, "\t\t\t\tDisplays this help message");
	
	parser->addLongOption((char*)"cp", &param_randcp, 0, utk::assignBoolTrue, NULL, "\t\t\t\tRandomize using cranley patterson");
	parser->addLongOption((char*)"xor", &param_randxor, 0, utk::assignBoolTrue, NULL, "\t\t\t\tRandomize using xor");
    
    return 0;
}

int dealParamParserSampler(utk::ParamParser* parser)
{
	if(param_output.empty())
	{
		ERROR("Argument -o required");
		std::cout << parser->getHelp() << std::endl;
		return 0;
	}
	if(param_display_help)
	{
		std::cout << parser->getHelp() << std::endl;
		return 0;
	}
	if(param_verbose)
	{
		parser->displayState();
	}
	if(param_randcp && param_randxor)
	{
		WARNING("Both xor and cranley patterson shift required, applying xor before cranley patterson");
	}
	return 1;
}
