#include "SamplerKronecker/Kronecker.hpp"
#include "../parameters/ParamParser_getopt.hpp"
#include "../io/fileIO.hpp"
#include <chrono>
#include <cstdint>

#include "runSampler.hpp"

using namespace utk;

typedef float T;
#define D 2
typedef Point<D, T> P;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;

	//PARSE PARAM
	initParserSampler(&parser);
	//PARSING
	parser.parse(argc, argv);

	if(!dealParamParserSampler(&parser))
		return 0;

	PointsetWriter<D, T, P> writer;
	writer.open(param_output.c_str());
	while(param_nbrealisations>0)
	{
		Kronecker::Generator<2> S(Kronecker::K21_2, param_nbrealisations);

		//SAMPLE
		Pointset<D, T, P> pts;

		//load domain & toricity
		for(uint d=0; d<D; d++)
		{
			pts.domain.pMin.pos()[d] = 0;
			pts.domain.pMax.pos()[d] = 1;
		}
		pts.toricity = 1;
		pts.resize(param_nbsamples);

		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

		for(uint32_t i=0; i<param_nbsamples; i++) {
			for(uint8_t d=0; d<D; d++)
				pts[i].pos()[d] = S.get_current_value(d);
			S.next();
    }

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
