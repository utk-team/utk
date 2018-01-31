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
#define D 10
typedef Point<D, T> P;
typedef SamplerSobolIndices S;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	S sampler;
	
	initParserSampler(&parser);
	
	int param_sobol_ids[D];
	for(int i=0; i<D; i++)
		param_sobol_ids[i] = i+1;
		
	for(int i=0; i<D; i++)
	{
		char c = '0' + i;
		
		std::string str1 = "sobolD";
		str1 += (char)c;
		std::string str2 = "<uint=";
		if (i == 9)
			str2 += "10";
		else
			str2 += (char)(c+1);
		str2 += "> the sobol indices for dimension ";
		str2 += (char)c;
		std::string str3 = "Sobol Id ";
		str3 += (char)c;
		
		parser.addLongOption((char*)str1.c_str(), &param_sobol_ids[i], 1, assignUInt, displayUInt, (char*)str2.c_str(), (char*)str3.c_str());
	}

	//PARSING
	parser.parse(argc, argv);
	
	if(!dealParamParserSampler(&parser))
		return 0;
	
	if(param_randxor)
	{
		ERROR("Cannot randomize using xor a set of floating point samples");
		return false;
	}
	
	sampler.setIndices(param_sobol_ids, D);
	
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
