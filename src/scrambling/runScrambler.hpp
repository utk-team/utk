#include "../parameters/ParamParser.hpp"
#include "../io/messageStream.hpp"
#include "../pointsets/Pointset.hpp"


std::string param_input;
bool param_display_help = false;
std::string param_output="output_pts.dat";
bool param_verbose = true;


int initParserScrambler(utk::ParamParser* parser)
{
	parser->addShortOption('i', &param_input, 1, utk::assignString, utk::displayString, "[string]\tInput file", "Input");
	parser->addShortOption('o', &param_output, 1, utk::assignString, utk::displayString, "[string=output_pts.dat]\tOutput file", "Output");

	parser->addLongOption((char*)"silent", &param_verbose, 0, utk::assignBoolFalse, utk::displayBool, "\t\t\tSilent mode", "Verbose" );
	parser->addShortOption('h', &param_display_help, 0, utk::assignBoolTrue, NULL, "\t\t\t\tDisplays this help message");
    
    return 0;
}

int dealParamParserScrambler(utk::ParamParser* parser)
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
	if(param_display_help)
	{
		std::cout << parser->getHelp() << std::endl;
		return 0;
	}
	if(param_verbose)
	{
		parser->displayState();
	}

	return 1;
}
