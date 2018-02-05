#include "SamplerSobolIndices.hpp"
#include "SamplerVanDerCorput.hpp"
#include "SamplerHammersley.hpp"
#include "../parameters/ParamParser_getopt.hpp"
#include "../io/fileIO.hpp"
#include "../io/imageIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef double T;
#define D 4
typedef Point<D, T> P;
typedef SamplerSobolIndices S;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	S sampler;
	
	int sobolIdX = 1;
	int sobolIdY = 2;
	int sobolIdZ = 3;
	int sobolIdW = 4;
	
	//PARSE PARAM
	initParserSampler(&parser);
	parser.addShortOption('x', &sobolIdX, 1, assignInt, displayInt, (char*)"<uint=1>\tSobol Indice on X dimension", (char*)"Sobol X");
	parser.addShortOption('y', &sobolIdY, 1, assignInt, displayInt, (char*)"<uint=2>\tSobol Indice on Y dimension", (char*)"Sobol Y");
	parser.addShortOption('z', &sobolIdZ, 1, assignInt, displayInt, (char*)"<uint=3>\tSobol Indice on Z dimension", (char*)"Sobol Z");
	parser.addShortOption('w', &sobolIdW, 1, assignInt, displayInt, (char*)"<uint=4>\tSobol Indice on W dimension", (char*)"Sobol W");

	//PARSING
	parser.parse(argc, argv);
	
	if(!dealParamParserSampler(&parser))
		return 0;
	
	int indices[10] = {sobolIdX, sobolIdY, sobolIdZ, sobolIdW, 5, 6, 7 , 8, 9, 10};
	sampler.setIndices(indices, D);
	
	PointsetWriter<D, T, P> writer;
	writer.open(param_output.c_str());
	while(param_nbrealisations>0)
	{
		//SAMPLE
		Pointset<D, T, P> pts;
		
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		if(!sampler.generateSamples<D, T, P>(pts, param_nbsamples))
			return 1;
		
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		if(param_verbose)
			std::cout << std::fixed << std::setprecision(5) << "Generated " << pts.size() << " samples in " << time_span.count() << " secs" << std::endl;
		
		
		//WRITE
		writer.writePointset(pts);
		param_nbrealisations--;
	}
	writer.close();
	
	return 0;
}
