#include "SamplerLDBN.hpp"
#include "../parameters/ParamParser_getopt.hpp"
#include "../io/fileIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef double T;
#define D 4
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
	
	if(param_randxor)
	{
		ERROR("Cannot randomize using xor a set of floating point samples");
		return false;
	}
	
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
		Pointset<D, T, Point<D, T> > pts;
		
		Pointset<2, T, Point<2, T> > pts2d;
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		if(!sampler.generateSamples<2, T, Point<2, T> >(pts2d, param_nbsamples))
			return 1;
		
		pts.resize(pts2d.size());
		
		std::vector<uint> ind;
		ind.resize(pts2d.size());
		for(int i=0; i<pts2d.size(); i++)
			ind[i] = i;
		
		for(int i=0; i<pts2d.size(); i++)
		{
			int idrandomchoice = rand()%ind.size();
			int randomchoice = ind[idrandomchoice];
			//std::cout << randomchoice << std::endl;
			pts[i].pos()[0] = pts2d[i].pos()[0];
			pts[i].pos()[1] = pts2d[i].pos()[1];
			pts[i].pos()[2] = pts2d[randomchoice].pos()[0];
			pts[i].pos()[3] = pts2d[randomchoice].pos()[1];
			ind.erase(ind.begin()+idrandomchoice);
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
