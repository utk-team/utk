#include "../../samplers/SamplerBNLDS_optimizer.hpp"
#include "../../parameters/ParamParser_getopt.hpp"
#include "../../io/fileIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef double T;
#define D 2
typedef Point<D, T> P;
typedef SamplerBNLDS_optimizer S;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	S sampler;
	
	//PARSE PARAM
	initParserSampler(&parser);
	
	std::string param_inlutfile;
	parser.addLongOption((char*)"lut-in", &param_inlutfile, 1, assignString, displayString, (char*)"<string> the file containing the lookuptable", (char*)"LUT in");
	std::string param_outlutfile;
	parser.addLongOption((char*)"lut-out", &param_outlutfile, 1, assignString, displayString, (char*)"<string> the file to write the lookuptable", (char*)"LUT out");
	
	std::vector<uint> param_sobol_ids;
	param_sobol_ids.resize(D);
	param_sobol_ids[0] = 1;
	param_sobol_ids[1] = 2;
	parser.addLongOption((char*)"sobolD0", &param_sobol_ids[0], 1, assignUInt, displayUInt, (char*)"<uint=1> the sobol indices for dimension 0", (char*)"Sobol Id 0");
	parser.addLongOption((char*)"sobolD1", &param_sobol_ids[1], 1, assignUInt, displayUInt, (char*)"<uint=2> the sobol indices for dimension 1", (char*)"Sobol Id 1");
	
	uint param_K = 4;
	parser.addShortOption('K', &param_K, 1, assignUInt, displayUInt, (char*)"<uint=4> the subdivision factor K", (char*)"K");
	
	int param_level = -1;
	parser.addLongOption((char*)"level", &param_level, 1, assignInt, displayInt, (char*)"<uint=-1> the level of subdivision (overrides the -n parameter)", (char*)"Level");
	
	unsigned long long int param_permut_val = 0;
	parser.addLongOption((char*)"val", &param_permut_val, 1, assignULLInt, displayULLInt, (char*)"<uint=0> the permut to apply", (char*)"Permut");
	
	//PARSING
	parser.parse(argc, argv);
	
	if(!dealParamParserSampler(&parser))
		return 0;
	
	sampler.setLookupTableInfile(param_inlutfile);
	sampler.setLookupTableOutfile(param_outlutfile);
	sampler.setSobolIds(param_sobol_ids);
	sampler.setSubdivFactorK(param_K);
	
	sampler.permut_val = param_permut_val;
	sampler.optiPCF = true;
	sampler.fillEndTreeRandomOpti = true;
	
	sampler.fillEndTreeZero = false;
	sampler.optiMinDist = false;
	sampler.fillEndTreeRandom = false;
	
	
	if(param_level > 0)
		param_nbsamples = pow( pow(param_K, D), param_level );
	
	if(param_randxor)
	{
		ERROR("Cannot randomize using xor a set of floating point samples");
		return false;
	}
	
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