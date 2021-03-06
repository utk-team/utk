#include "SamplerLDBN.hpp"
#include "../parameters/ParamParser_getopt.hpp"
#include "../io/fileIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef uint T;
#define D 2
typedef Point<D, T> P;
typedef SamplerLDBN S;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	S sampler;
	
	//PARSE PARAM
	initParserSampler(&parser);
	std::string permut_file;
	uint tilesize = 0;
	parser.addShortOption('d', &permut_file, 1, assignString, displayString, "<string>\t\t\tFile containing the permutations", "Permutations");
	parser.addShortOption('t', &tilesize, 1, assignUInt, displayUInt, "<uint>\t\t\tThe size of the permutation tile", "Tile size");
	
	//PARSING
	parser.parse(argc, argv);
	
	if(!dealParamParserSampler(&parser))
		return 0;
	
	if(permut_file.empty())
	{
		ERROR("Missing argument -d");
		std::cout << parser.getHelp() << std::endl;
		return 1;
	}
	if(tilesize == 0)
	{
		ERROR("Missing argument -t");
		std::cout << parser.getHelp() << std::endl;
		return 1;
	}
	
	sampler.setTileSize(tilesize);
	sampler.setPermutFile(permut_file);
	
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
