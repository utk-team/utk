#include "../../parameters/ParamParser.hpp"
#include "../../io/messageStream.hpp"

bool param_help = false;
bool param_verbose = true;
bool param_brute = false;
std::string param_input;
std::string param_output;
unsigned int param_nbpts = 0;

class DiscrepancyStatistics
{
public:
	double iter;
	double nbpts;
	double mean_discrepancy;
	double var_discrepancy;
	double max_discrepancy;
	double min_discrepancy;
	
	DiscrepancyStatistics() {
		iter = 0;
		nbpts = 0;
		mean_discrepancy = 0;
		var_discrepancy = 0;
		min_discrepancy = 0;
		max_discrepancy = 0;
	}
	void addValue(double discrepancy)
	{
		iter++;
		
		double delta = discrepancy - mean_discrepancy;
		mean_discrepancy += delta/iter;
		var_discrepancy += delta*(discrepancy - mean_discrepancy);
		
		if(iter == 1) {
			min_discrepancy = discrepancy;
			max_discrepancy = discrepancy;
		} else {
			if(discrepancy > max_discrepancy)
				max_discrepancy = discrepancy;
			if(discrepancy < min_discrepancy)
				min_discrepancy = discrepancy;
		}
	}
	void computeStatistics()
	{
		var_discrepancy /= iter-1;
		if(iter <= 1)
			var_discrepancy=0;
	}
};
std::ostream& operator<<(std::ostream& os, const DiscrepancyStatistics& e) 
{ 
	os << "#Nbpts\t\t#Mean\t\t#Var\t\t#Min\t\t#Max\t\t#NbPtsets" << std::endl;
	os << e.nbpts << "\t\t";
	os << e.mean_discrepancy << "\t\t";
	os << e.var_discrepancy << "\t\t";
	os << e.min_discrepancy << "\t\t";
	os << e.max_discrepancy << "\t\t";
	os << e.iter;
	return os; 
}

int initParserDiscrepancy(utk::ParamParser* parser)
{
	parser->addShortOption('h', &param_help, 0, utk::assignBoolTrue, NULL, "<string>\t\tDisplays this help message");
	parser->addShortOption('i', &param_input, 1, utk::assignString, utk::displayString, "<string>\t\tThe input pointsets", "Input");
	parser->addShortOption('o', &param_output, 1, utk::assignString, utk::displayString, "<string>\t\tThe output discrepancies", "Output");
	parser->addShortOption('s', &param_nbpts, 1, utk::assignInt, utk::displayInt, "<uint>\t\tThe number of samples to read from the file", "NbPts");
	parser->addLongOption((char*)"silent", &param_verbose, 0, utk::assignBoolFalse, utk::displayBool, (char*)"\t\tSilent mode", (char*)"Verbose");
	parser->addLongOption((char*)"brute", &param_brute, 0, utk::assignBoolTrue, utk::displayBool, (char*)"\t\tOutput brute values instead of computing the statistics", (char*)"Brute");
    
    return 0;
}

int dealParamParserDiscrepancy(utk::ParamParser* parser)
{
	if(param_input.empty())
	{
		ERROR("Argument -i required");
		std::cout << parser->getHelp() << std::endl;
		return 0;
	}
	
	if(param_help)
		std::cout << parser->getHelp() << std::endl;
	
	if(param_verbose)
		parser->displayState();
	
	return 1;
}
