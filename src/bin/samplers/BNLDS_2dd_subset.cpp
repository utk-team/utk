#include "../../samplers/SamplerBNLDS.hpp"
#include "../../parameters/ParamParser_getopt.hpp"
#include "../../io/fileIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef double T;
#define D 2
typedef Point<D, T> P;
typedef SamplerBNLDS S;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	S sampler;
	
	//PARSE PARAM
	initParserSampler(&parser);
	
	std::string param_inlutfile;
	parser.addLongOption((char*)"lut-in", &param_inlutfile, 1, assignString, displayString, (char*)"<string> the file containing the lookuptable", (char*)"LUT in");
	
	std::vector<uint> param_sobol_ids;
	param_sobol_ids.resize(D);
	param_sobol_ids[0] = 1;
	param_sobol_ids[1] = 2;
	parser.addLongOption((char*)"sobolD0", &param_sobol_ids[0], 1, assignUInt, displayUInt, (char*)"<uint=1> the sobol indices for dimension 0", (char*)"Sobol Id 0");
	parser.addLongOption((char*)"sobolD1", &param_sobol_ids[1], 1, assignUInt, displayUInt, (char*)"<uint=2> the sobol indices for dimension 1", (char*)"Sobol Id 1");
	
	uint param_K = 4;
	parser.addShortOption('K', &param_K, 1, assignUInt, displayUInt, (char*)"<uint=4> the subdivision factor K", (char*)"K");

	//PARSING
	parser.parse(argc, argv);
	
	if(param_inlutfile.empty())
	{
		if(param_K == 4)
			param_inlutfile = std::string(UTK_DATA_PATH) + "/BNLDS/factorK_4/Sobol_12/allPermutsCache_Sobol+OptPerms_1_2_00000.dat";
		else if (param_K == 8)
			param_inlutfile = std::string(UTK_DATA_PATH) + "/BNLDS/factorK_8/fastoptim_lut_sobol1_2_k8_lvlmax3_sampler_BNLDS_optimizer_PCF_RandomOpti_2dd.dat";
	}
	
	sampler.setLookupTableFile(param_inlutfile);
	sampler.setSobolIds(param_sobol_ids);
	sampler.setSubdivFactorK(param_K);
	int level= 0;
	while( pow( pow(param_K, D), level ) < param_nbsamples )
		level++;
	
	int asked_number_of_samples = param_nbsamples;
	int octave_aligned_number_of_samples = pow( pow(param_K, D), level );
	
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
		Pointset<D, T, P> pts;
		Pointset<D, T, P> pts_big;
		
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		if(!sampler.generateSamples<D, T, P>(pts_big, octave_aligned_number_of_samples))
			return 1;
		
		//take subset
		int invratio = octave_aligned_number_of_samples / asked_number_of_samples;
		double divisor = 1.0/sqrt(invratio);
		
		for(int i=0; i<octave_aligned_number_of_samples; i++)
		{
			if(pts_big[i].pos()[0] < divisor && pts_big[i].pos()[1] < divisor)
			{
				Point<D, T> pt = pts_big[i];
				pt.pos()[0] /= divisor;
				pt.pos()[1] /= divisor;
				pts.push_back(pt);
			}
		}
		
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