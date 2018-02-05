#include "SamplerForcedRandom.hpp"
#include "../parameters/ParamParser_getopt.hpp"
#include "../io/fileIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef double T;
#define D 2
typedef Point<D, T> P;
typedef SamplerForcedRandom S;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	S sampler;
	
	int param_sparsity = 8;
	int param_matrix_size = 512;
	
	
	//PARSE PARAM
	initParserSampler(&parser);
	parser.addLongOption((char*)"matrix", &param_matrix_size, 1, assignInt, displayInt, (char*)"[int=512] the matrix size [512 | 1024 | 2048]", (char*)"Matrix size");
	parser.addLongOption((char*)"sparsity", &param_sparsity, 1, assignInt, displayInt, (char*)"[int=8] the matrix sparsity", (char*)"Sparsity");
	//PARSING
	parser.parse(argc, argv);
	
	if(!dealParamParserSampler(&parser))
		return 0;
	
	sampler.setMatrixSize(param_matrix_size);
	sampler.setMatrixSparsity(param_sparsity);
	
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
