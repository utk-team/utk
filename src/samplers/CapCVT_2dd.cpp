#include "../../parameters/ParamParser_getopt.hpp"
#include <chrono>
#include <sstream>

#include "runSampler.hpp"

using namespace utk;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;

	//PARSE PARAM
	initParserSampler(&parser);
	//PARSING
	parser.parse(argc, argv);
	
	if(!dealParamParserSampler(&parser))
		return 0;

	std::stringstream ss;
	ss << "rm " << param_output << ";";
	ss << "/home/logarithme/Desktop/Recherche/paper_SIGGASIA_Abdallah_April_2016/BNGLD/src/samplers/CapCVT_code/code/build/binaries/bin/gx_capcvt2d -l 30 -o " << param_output << " -n " << param_nbsamples << std::endl;
	
	system(ss.str().c_str());
	
	return 0;
}