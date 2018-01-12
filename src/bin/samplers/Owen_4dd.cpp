#include "../../samplers/SamplerOwenSobol.hpp"
#include "../../parameters/ParamParser_getopt.hpp"
#include "../../io/fileIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef double T;
#define D 4
typedef Point<D, T> P;
typedef SamplerOwenSobol S;

int main(int argc, char** argv)
{
	srand(time(NULL));
	
	ParamParser_getopt parser;
	S sampler;
	
	//PARSE PARAM
	initParserSampler(&parser);
	//PARSING
	parser.parse(argc, argv);
	
	if(!dealParamParserSampler(&parser))
		return 0;
	
	/*if(log2(param_nbsamples) != (int)log2(param_nbsamples))
	{
		ERROR("Owen can only generate power of two number of samples");
		return 1;
	}*/
	
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
		
		/*Vector<2, bool*> permut;
		permut[0] = (bool*)malloc(sizeof(bool)*param_nbsamples);
		permut[1] = (bool*)malloc(sizeof(bool)*param_nbsamples);
		for(uint i=0; i<param_nbsamples; i++)
		{
			permut[0][i] = rand()%2;
			permut[1][i] = rand()%2;
		}
		pts.owen_scrambling(permut);
		
		Pointset<D, double, P> pts2;
		pts2.resize(param_nbsamples);
		
		for(uint i=0; i<param_nbsamples; i++)
		{
			pts2[i].pos()[0] = (double)pts[i].pos()[0] / (double)param_nbsamples;
			pts2[i].pos()[1] = (double)pts[i].pos()[1] / (double)param_nbsamples;
		}*/
		
		/*{
			PointsetWriter<D, T, P> writer2;
			writer2.open("owenx.dat");
			writer2.writePointset(pts2);
		}
		
		for(uint i=0; i<param_nbsamples; i++)
		{
			permut[0][i] = 0;
			permut[1][i] = rand()%2;
		}
		pts.owen_scrambling(permut);
		
		Pointset<D, double, P> pts3;
		pts3.resize(param_nbsamples);
		
		for(uint i=0; i<param_nbsamples; i++)
		{
			pts3[i].pos()[0] = (double)pts[i].pos()[0] / (double)param_nbsamples;
			pts3[i].pos()[1] = (double)pts[i].pos()[1] / (double)param_nbsamples;
		}
		
		{
			PointsetWriter<D, T, P> writer2;
			writer2.open("oweny.dat");
			writer2.writePointset(pts3);
		}*/
		
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
		
		writer.writePointset(pts);
		
		param_nbrealisations--;
	}
	writer.close();
	
	return 0;
}

/***

#include "../../samplers/SamplerSobolIndices.hpp"
#include "../../parameters/ParamParser_getopt.hpp"
#include "../../io/fileIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef double T;
#define D 4
typedef Point<D, T> P;
typedef SamplerSobolIndices S;

int main(int argc, char** argv)
{
	srand(time(NULL));
	
	ParamParser_getopt parser;
	S sampler;
	
	//PARSE PARAM
	initParserSampler(&parser);
	//PARSING
	parser.parse(argc, argv);
	
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
		Pointset<D, uint, P> pts;
		
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		
		if(!sampler.generateSamples<D, uint, P>(pts, param_nbsamples))
			return 1;
		
		Vector<4, bool*> permut;
		for(int d=0; d<D; d++)
			permut[d] = (bool*)malloc(sizeof(bool)*pts.domain.pMax.pos()[0]);
		
		for(uint i=0; i<pts.domain.pMax.pos()[0]; i++)
		for(int d=0; d<D; d++)
			permut[d][i] = rand()%2;

		pts.owen_scrambling(permut);
		
		Pointset<D, double, P> pts2;
		pts2.resize(param_nbsamples);
		
		for(uint i=0; i<param_nbsamples; i++)
		for(int d=0; d<D; d++)
			pts2[i].pos()[d] = (double)pts[i].pos()[d] / (double)pts.domain.pMax.pos()[0];
		
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		if(param_verbose)
			std::cout << std::fixed << std::setprecision(5) << "Generated " << pts2.size() << " samples in " << time_span.count() << " secs" << std::endl;
		
		if(param_randcp)
		{	
			Vector<D, double> cp;
			for(int i=0; i<D; i++)
				cp = drand48();
			pts2.cranley_patterson(cp);
		}
		
		//WRITE
		writer.writePointset(pts2);
		param_nbrealisations--;
	}
	writer.close();
	
	return 0;
}
***/