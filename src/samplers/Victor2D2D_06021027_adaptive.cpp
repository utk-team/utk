#include "../../samplers/SamplerVictor2D_06022017_Adaptive.hpp"
#include "../../parameters/ParamParser_getopt.hpp"
#include "../../io/fileIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef double T;
#define D 2
typedef Point<D, T> P;
typedef SamplerVictorND06022017 S;

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
	std::vector<uint> param_sobol_ids;
	param_sobol_ids.resize(D);
	param_sobol_ids[0] = 1;
	param_sobol_ids[1] = 2;
	parser.addLongOption((char*)"sobolD0", &param_sobol_ids[0], 1, assignUInt, displayUInt, (char*)"<uint=1> the sobol indices for dimension 0", (char*)"Sobol Id 0");
	parser.addLongOption((char*)"sobolD1", &param_sobol_ids[1], 1, assignUInt, displayUInt, (char*)"<uint=2> the sobol indices for dimension 1", (char*)"Sobol Id 1");
	bool param_generate = false;
	parser.addLongOption((char*)"generate", &param_generate, 0, assignBoolTrue, displayBool, (char*)"if set, the sampler will generate new LUT entries when needed", (char*)"Generate");
	bool param_random_permuts = false;
	parser.addLongOption((char*)"random", &param_random_permuts, 0, assignBoolTrue, displayBool, (char*)"if set, the sampler will generate new LUT entries using random permuts instead of optimized ones (has no effect if not used with --generate)", (char*)"Random");
	uint param_level = 0;
	parser.addLongOption((char*)"level", &param_level, 1, assignUInt, NULL, (char*)"<uint(>0)> will generate 16^level samples, if -n and --level are used at the same time, -n will be ignored", (char*)"");	
	uint param_K = 4;
	parser.addShortOption('K', &param_K, 1, assignUInt, displayUInt, (char*)"<uint=4> the subdivision factor K", (char*)"K");
	bool param_null_permuts = false;
	parser.addLongOption((char*)"null", &param_null_permuts, 0, assignBoolTrue, displayBool, (char*)"if set, the sampler will generate new LUT entries using empty permuts instead of optimized ones (has no effect if not used with --generate)", (char*)"null");
	std::string param_image;
	parser.addLongOption((char*)"image", &param_image, 1, assignString, displayString, (char*)"<string> the intensity image", (char*)"Image");
	
	//PARSING
	parser.parse(argc, argv);
	
	if(param_inlutfile.empty())
	{
		ERROR("parameter --lut-in mandatory");
		std::cout << parser.getHelp() << std::endl;
		return 0;
	}
	if(param_image.empty())
	{
		ERROR("parameter --image mandatory");
		std::cout << parser.getHelp() << std::endl;
		return 0;
	}
	if(param_level != 0)
	{
		param_nbsamples = pow(param_K, 2*param_level);//(a << (4*param_level));
	}

	sampler.setSubdivFactorK(param_K);
	sampler.setLookupTableInFile(param_inlutfile);
	
	if(! param_outlutfile.empty() )
		sampler.setLookupTableOutFile(param_outlutfile);

	sampler.generatesNewPatterns(param_generate);
	sampler.useNullPermuts(param_null_permuts);
	sampler.useOptimizedPermuts(!param_random_permuts);
	sampler.setSobolIds(param_sobol_ids);
	
	sampler.setIntensityImage(param_image);
	
	sampler.setSilent(!param_verbose);
	
	if(!dealParamParserSampler(&parser))
		return 0;
	
	parser.displayState();
	
	PointsetWriter<D, T, P> writer;
	writer.open(param_output.c_str());
	while(param_nbrealisations>0)
	{
		//SAMPLE
		Pointset<D, T, P> pts;
		
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		if(!sampler.generateSamples<D, T, P>(pts, param_nbsamples))
			return 1;
		Pointset<D, T, P> pts_fin;
		for(int i=0; i<pts.size(); i++)
		{
			//if(pts[i].pos()[1] < 0.15)
			if(pts[i].pos()[1] < 450.0/600.0)
				pts_fin.push_back(pts[i]);
		}
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		if(param_verbose)
			std::cout << std::fixed << std::setprecision(5) << "Generated " << pts_fin.size() << " samples in " << time_span.count() << " secs" << std::endl;
		
		//WRITE
		writer.writePointset(pts_fin);
		param_nbrealisations--;
	}
	writer.close();
	
	
	
	return 0;
}