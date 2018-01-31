#include "SamplerFastPoisson.hpp"
#include "../parameters/ParamParser_getopt.hpp"
#include "../io/fileIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef double T;
#define D 2
typedef Point<D, T> P;
typedef SamplerFastPoisson S;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	S sampler;
	
	std::string param_method;
	int param_multiplier = false;
	int param_minmaxthrows = 1000;
	double param_radius;
	bool param_tiled = false;
	
	//PARSE PARAM
	initParserSampler(&parser);
	parser.addLongOption((char*)"method", &param_method, 1, assignString, displayString, (char*)"\t[string=DartThrowing] The method to use to generate the samples (DartThrowing|Pure|LinearPure)", (char*)"Method");
	//parser.addLongOption((char*)"maximize", &param_maximize, 0, assignBoolTrue, displayBool, (char*)"\tIf set, maximize point set after sampling", (char*)"Maximize");
	parser.addLongOption((char*)"minmaxthrows", &param_minmaxthrows, 1, assignInt, displayInt, (char*)"\t [int=100] set minimum number of maximum throws for DartThrowing sampler", (char*)"MinMaxThrows");
	parser.addLongOption((char*)"mult", &param_multiplier, 1, assignInt, displayInt, (char*)"\t [int=1] set multiplier for DartThrowing sampler", (char*)"Multiplier");
	//parser.addLongOption((char*)"relax", &param_relax_iter, 1, assignInt, displayInt, (char*)"\t [int=0] apply the specified number of relaxations after sampling", (char*)"Relax Iterations");
	parser.addLongOption((char*)"tiled", &param_tiled, 0, assignBoolTrue, displayBool, (char*)"\tIf set, use tiled domain", (char*)"Tiled");
	//parser.addLongOption((char*)"radius", &param_radius, 1, assignDouble, displayDouble, (char*)"\t [double=0.1] the radius", (char*)"Radius");

	//PARSING
	parser.parse(argc, argv);
	
	if(!dealParamParserSampler(&parser))
		return 0;
	
	if(param_method.empty())
		param_method = "DartThrowing";
	
	/*The original method generates a certain amount of samples depending on the given radius
	 * to fit our framework, we try to do the opposition conversion with minor ad hoc tuning
	 * for each method*/
	if (param_method == "DartThrowing")
		param_radius = 1.0 / (1.3*sqrt(param_nbsamples));
	//else if (param_method == "Boundary")
	//	param_radius = 1.0 / (1.3*sqrt(param_nbsamples));
	else if (param_method == "Pure")
		param_radius = 1.0 / (1.22*sqrt(param_nbsamples));
	else if (param_method == "LinearPure")
		param_radius = 1.0 / (1.18*sqrt(param_nbsamples));
	
	/*if(param_method.empty())
	{
		ERROR("Missing argument --method");
		std::cout << parser.getHelp() << std::endl;
		return 1;
	}*/
	
	if(param_randxor)
	{
		ERROR("Cannot randomize using xor a set of floating point samples");
		return false;
	}
	
	sampler.setMaximize(false);
	sampler.setMethod(param_method);
	sampler.setMultiplier(param_multiplier);
	sampler.setRadius(param_radius);
	sampler.setRelaxIter(0);
	sampler.setTiled(param_tiled);
	
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
