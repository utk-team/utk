#include "SamplerCapCVT.hpp"
#include "../parameters/ParamParser_getopt.hpp"
#include "../io/fileIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef double T;
#define D 2
typedef Point<D, T> P;
typedef SamplerCapCVT S;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	S sampler;
	
	std::string boundary_file=std::string(UTK_DATA_PATH)+"/CapCVT/square.line";
	parser.addLongOption((char*)"boundary", &boundary_file, 1, assignString, displayString, "[string]\t\tThe file with the boundary of the domain", "Boundary");
	int nb_iter=100;
	parser.addLongOption((char*)"iter", &nb_iter, 1, assignInt, displayInt, "[int]\t\tThe numer of iterations", "Iterations");
	double lambda=0.0;

	//PARSE PARAM
	initParserSampler(&parser);
	//PARSING
	parser.parse(argc, argv);

	if(!dealParamParserSampler(&parser))
		return 0;
	
	sampler.setBoundaryFile(boundary_file);
	sampler.setIter(nb_iter);
	sampler.setLambda(lambda);
	
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
