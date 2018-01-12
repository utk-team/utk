#include "../../samplers/SamplerVictorOwenPrime.hpp"
#include "../../parameters/ParamParser_getopt.hpp"
#include "../../io/fileIO.hpp"
#include "../../io/imageIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef double T;
#define D 4
typedef Point<D, T> P;
typedef SamplerVictorOwenPrime S;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	S sampler;
	
	//PARSE PARAM
	initParserSampler(&parser);
	uint param_leveldepth = 6;
	int sobolIdX = 1;
	int sobolIdY = 2;
	int sobolIdZ = 3;
	int sobolIdW = 4;
	parser.addShortOption('x', &sobolIdX, 1, assignInt, displayInt, (char*)"<uint=1>\tSobol Indice on X dimension", (char*)"Sobol X");
	parser.addShortOption('y', &sobolIdY, 1, assignInt, displayInt, (char*)"<uint=2>\tSobol Indice on Y dimension", (char*)"Sobol Y");
	parser.addShortOption('z', &sobolIdZ, 1, assignInt, displayInt, (char*)"<uint=3>\tSobol Indice on Z dimension", (char*)"Sobol Z");
	parser.addShortOption('w', &sobolIdW, 1, assignInt, displayInt, (char*)"<uint=4>\tSobol Indice on W dimension", (char*)"Sobol W");
	parser.addLongOption((char*)"lvl", &param_leveldepth, 1, assignUInt, displayUInt, (char*)"[uint=6] the depth of one level", (char*)"Level Depth");
	//PARSING
	parser.parse(argc, argv);
	
	if(!dealParamParserSampler(&parser))
		return 0;
	sampler.setLevelDepth(param_leveldepth);
	int indices[10] = {sobolIdX, sobolIdY, sobolIdZ, sobolIdW, 5, 6, 7 , 8, 9, 10};
	sampler.setIndices(indices, 4);
	
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
	
	/*PointsetIllustrator<D, T, P> illu;
	illu.setBoundingBox(1000);
	illu.open("toto.ps");
	
	Pointset<D, T, P> pts;
	sampler.generateSamples<D, T, P>(pts, 2);
	illu.setColor(0.75, 0.75, 0.75);
	illu.setPointRadius(40);
	illu.drawPointset(pts);
	
	sampler.generateSamples<D, T, P>(pts, 8);
	illu.setColor(0.5, 0.5, 0.5);
	illu.setPointRadius(30);
	illu.drawPointset(pts);
	
	sampler.generateSamples<D, T, P>(pts, 16);
	illu.setColor(0.25, 0.25, 0.25);
	illu.setPointRadius(20);
	illu.drawPointset(pts);
	
	
	illu.setNumbered(1);
	sampler.generateSamples<D, T, P>(pts, 64);
	illu.setColor(0, 0, 0);
	illu.setPointRadius(10);
	illu.drawPointset(pts);
	
	illu.close();*/
	
	return 0;
}