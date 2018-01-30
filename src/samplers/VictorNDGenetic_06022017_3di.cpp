#include "../../samplers/SamplerVictorND_06022017_genetic.hpp"
#include "../../parameters/ParamParser_getopt.hpp"
#include "../../io/fileIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef uint T;
#define D 3
typedef Point<D, T> P;
typedef SamplerVictor06022017 S;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	S sampler;
	
	//PARSE PARAM
	initParserSampler(&parser);
	
	std::string param_inlutfile;
	parser.addLongOption((char*)"lut-in", &param_inlutfile, 1, assignString, displayString, (char*)"<string> the file containing the lookuptable", (char*)"LUT in");
	std::string param_outlutfile;
	parser.addLongOption((char*)"lut-out", &param_outlutfile, 1, assignString, displayString, (char*)"<string> the file to write the new lookuptable", (char*)"LUT out");
	uint K=4;
	parser.addShortOption('K', &K, 1, assignUInt, displayUInt, (char*)"<uint=4> the subdivision factor", (char*)"K");
	uint param_level = 0;
	parser.addLongOption((char*)"level", &param_level, 1, assignUInt, NULL, (char*)"<uint(>0)> will generate K^3level samples, if -n and --level are used at the same time, -n will be ignored", (char*)"");	
	std::vector<uint> param_sobol_ids;
	param_sobol_ids.resize(D);
	param_sobol_ids[0] = 1;
	param_sobol_ids[1] = 2;
	param_sobol_ids[2] = 3;
	parser.addLongOption((char*)"sobolD0", &param_sobol_ids[0], 1, assignUInt, displayUInt, (char*)"<uint=1> the sobol indices for dimension 0", (char*)"Sobol Id 0");
	parser.addLongOption((char*)"sobolD1", &param_sobol_ids[1], 1, assignUInt, displayUInt, (char*)"<uint=2> the sobol indices for dimension 1", (char*)"Sobol Id 1");
	parser.addLongOption((char*)"sobolD2", &param_sobol_ids[2], 1, assignUInt, displayUInt, (char*)"<uint=3> the sobol indices for dimension 2", (char*)"Sobol Id 2");
	bool param_generate = false;
	parser.addLongOption((char*)"generate", &param_generate, 0, assignBoolTrue, displayBool, (char*)"if set, the sampler will generate new LUT entries when needed", (char*)"Generate");
	bool param_random_permuts = false;
	parser.addLongOption((char*)"random", &param_random_permuts, 0, assignBoolTrue, displayBool, (char*)"if set, the sampler will generate new LUT entries using random permuts instead of optimized ones (has no effect if not used with --generate)", (char*)"Random");
	bool param_null_permuts = false;
	parser.addLongOption((char*)"null", &param_null_permuts, 0, assignBoolTrue, displayBool, (char*)"if set, the sampler will generate new LUT entries using empty permuts instead of optimized ones (has no effect if not used with --generate)", (char*)"Null");
	
	//PARSING
	parser.parse(argc, argv);
	
	if(!dealParamParserSampler(&parser))
		return 0;
	
	if(param_inlutfile.empty())
	{
		ERROR("parameter --lut-in mandatory");
		std::cout << parser.getHelp() << std::endl;
		return 0;
	}

	sampler.setSubdivK(K);
	sampler.setLookupTableInFile(param_inlutfile);
	
	if(! param_outlutfile.empty() )
		sampler.setLookupTableOutFile(param_outlutfile);
	
	sampler.generatesNewPatterns(param_generate);
	sampler.useNullPermuts(param_null_permuts);
	sampler.useOptimizedPermuts(!param_random_permuts);
	sampler.setSobolIds(param_sobol_ids);
	sampler.setSilent(!param_verbose);
	
	if(param_level != 0)
	{
		param_nbsamples = pow(K, 3*param_level);
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
		
		//WRITE
		writer.writePointset(pts);
		param_nbrealisations--;
	}
	writer.close();
	
	
	
	return 0;
}