#include "../../samplers/SamplerBNLDS.hpp"
#include "../../samplers/SamplerSobolIndices.hpp"
#include "../../parameters/ParamParser_getopt.hpp"
#include "../../io/fileIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef double T;
#define D 4
typedef Point<D, T> P;
typedef SamplerBNLDS S;
typedef SamplerSobolIndices S2;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	S bnld;
	S2 sobol;
	
	//PARSE PARAM
	initParserSampler(&parser);
	
	std::string param_inlutfile;
	parser.addLongOption((char*)"lut-in", &param_inlutfile, 1, assignString, displayString, (char*)"<string> the file containing the lookuptable", (char*)"LUT in");
	
	std::vector<int> param_sobol_ids;
	param_sobol_ids.resize(2);
	param_sobol_ids[0] = 1;
	param_sobol_ids[1] = 2;
	std::vector<uint> param_bnld_ids;
	param_bnld_ids.resize(2);
	param_bnld_ids[0] = 3;
	param_bnld_ids[1] = 7;
	parser.addLongOption((char*)"sobolD0", &param_sobol_ids[0], 1, assignInt, displayInt, (char*)"<uint=1> the sobol indices for dimension 0", (char*)"Sobol Id 0");
	parser.addLongOption((char*)"sobolD1", &param_sobol_ids[1], 1, assignInt, displayInt, (char*)"<uint=2> the sobol indices for dimension 1", (char*)"Sobol Id 1");
	parser.addLongOption((char*)"sobolD2", &param_bnld_ids[0], 1, assignUInt, displayUInt, (char*)"<uint=3> the sobol indices for dimension 3", (char*)"Sobol Id 3");
	parser.addLongOption((char*)"sobolD3", &param_bnld_ids[1], 1, assignUInt, displayUInt, (char*)"<uint=7> the sobol indices for dimension 4", (char*)"Sobol Id 4");
	
	uint param_K = 4;
	parser.addShortOption('K', &param_K, 1, assignUInt, displayUInt, (char*)"<uint=4> the subdivision factor K", (char*)"K");
	
	int level = -1;
	parser.addLongOption((char*)"level", &level, 1, assignUInt, displayUInt, (char*)"<uint=-1> the level of subdivision (overrides the -n parameter)", (char*)"Level");
	
	//PARSING
	parser.parse(argc, argv);
	
	bnld.setLookupTableFile(param_inlutfile);
	bnld.setSobolIds(param_bnld_ids);
	bnld.setSubdivFactorK(param_K);
	if(level > 0)
		param_nbsamples = pow( pow(param_K, D), level );
	
	sobol.setIndices(&param_sobol_ids[0], 2);
	
	if(!dealParamParserSampler(&parser))
		return 0;
	
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
		Pointset< 2, T, Point<2, T> > pts;
		Pointset<D, T, P > pts_final;
		
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		if(!sobol.generateSamples< 2, T, Point<2, T> >(pts, param_nbsamples))
			return 1;
		pts_final.resize(pts.size());
		for(int i=0; i<pts.size(); i++)
		{
			pts_final[i].pos()[0] = pts[i].pos()[0];
			pts_final[i].pos()[1] = pts[i].pos()[1];
		}
		
		if(!bnld.generateSamples< 2, T, Point<2, T> >(pts, param_nbsamples))
			return 1;
		for(int i=0; i<pts.size(); i++)
		{
			pts_final[i].pos()[2] = pts[i].pos()[0];
			pts_final[i].pos()[3] = pts[i].pos()[1];
		}
		
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		
		
		if(param_verbose)
			std::cout << std::fixed << std::setprecision(5) << "Generated " << pts_final.size() << " samples in " << time_span.count() << " secs" << std::endl;
		
		if(param_randcp)
		{	
			Vector<D, double> cp;
			for(int i=0; i<D; i++)
				cp = drand48();
			pts_final.cranley_patterson(cp);
		}
		
		//WRITE
		writer.writePointset(pts_final);
		param_nbrealisations--;
	}
	writer.close();
	
	return 0;
}