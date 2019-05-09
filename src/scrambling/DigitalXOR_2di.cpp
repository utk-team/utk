#include "ScramblingDigitalXOR.hpp"
#include "../parameters/ParamParser_getopt.hpp"
#include "../io/fileIO.hpp"
#include <chrono>
#include <algorithm>
#include "runScrambler.hpp"

using namespace utk;

typedef int T;
#define D 2
typedef Point<D, T> P;
typedef ScramblingDigitalXOR S;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	S scrambler;
	
	//PARSE PARAM
	initParserScrambler(&parser);
	//PARSING
	parser.parse(argc, argv);
	
	if(!dealParamParserScrambler(&parser))
		return 0;
	
	PointsetWriter<D, T, P> writer;
	writer.open(param_output.c_str());
	
	PointsetReader<D, T, P> reader;
	reader.open(param_input.c_str());
	
	Pointset<D, T, P> pts;
	Pointset<D, T, P> pts_scrambled;
	
	while(reader.readPointset(pts))
	{
    int xmin=11212346, ymin=121212, xmax=0,ymax=0;
    for(auto i = 0; i < pts.size(); ++i)
    {
      xmin = std::min(xmin, pts[i].pos()[0]);
      ymin = std::min(xmin, pts[i].pos()[1]);
      xmax = std::max(xmax, pts[i].pos()[0]);
      ymax = std::max(ymax, pts[i].pos()[1]);
    }
    pts.domain.pMin = {xmin, ymin};
    pts.domain.pMax = {xmax, ymax};
    
		//SAMPLE
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		if(!scrambler.scramble<D, T, P>(pts, pts_scrambled))
			return 1;
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		if(param_verbose)
			std::cout << std::fixed << std::setprecision(5) << "Scrambled " << pts.size() << " samples in " << time_span.count() << " secs" << std::endl;
	
		//WRITE
		writer.writePointset(pts_scrambled);
	}
	writer.close();
	
	return 0;
}
