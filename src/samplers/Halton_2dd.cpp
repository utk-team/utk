#include "SamplerHalton.hpp"
#include "../parameters/ParamParser_getopt.hpp"
#include "../io/fileIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef double T;
#define D 2
typedef Point<D, T> P;
typedef SamplerHalton S;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	S sampler;
	
	//PARSE PARAM
	initParserSampler(&parser);
	
	uint bases[2] = {2, 3};
	parser.addShortOption('x', &bases[0], 1, assignUInt, displayUInt, (char*)"[uint=2] the basis for the first dimension", (char*)"Basis x");
	parser.addShortOption('y', &bases[1], 1, assignUInt, displayUInt, (char*)"[uint=3] the basis for the second dimension", (char*)"Basis y");
	
	//PARSING
	parser.parse(argc, argv);
	
	if(!dealParamParserSampler(&parser))
		return 0;
	
	sampler.setBaseList(&bases[0], 2);
	
	if(!dealParamParserSampler(&parser))
		return 0;
	
	if(param_randxor)
	{
		ERROR("Cannot randomize using xor a set of floating point samples");
		return false;
	}
	
	PointsetWriter< D, T, P > writer;
	writer.open(param_output.c_str());
	while(param_nbrealisations>0)
	{
		//SAMPLE
		Pointset< D, T, P > pts;
		
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		if(!sampler.generateSamples<D, T, P>(pts, param_nbsamples))
			return 1;
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		if(param_verbose)
			std::cout << std::fixed << std::setprecision(5) << "Generated " << pts.size() << " samples in " << time_span.count() << " secs" << std::endl;
		
		if(param_randcp)
		{	
			Vector<D, double> cp;
			for(int i=0; i<D; i++)
				cp = drand48();
			pts.cranley_patterson(cp);
		}
		
		//WRITE
		writer.writePointset(pts);
		param_nbrealisations--;
	}
	writer.close();
	
	return 0;
}
