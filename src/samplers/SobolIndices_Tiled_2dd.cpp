#include "../../samplers/SamplerSobolIndices.hpp"
#include "../../parameters/ParamParser_getopt.hpp"
#include "../../io/fileIO.hpp"
#include "../../io/imageIO.hpp"
#include <chrono>

#include "runSampler.hpp"

using namespace utk;

typedef double T;
#define D 2
typedef Point<D, T> P;
typedef SamplerSobolIndices S;

int main(int argc, char** argv)
{
	srand48(time(NULL));
	
	ParamParser_getopt parser;
	S sampler;
	
	int sobolIdX = 1;
	int sobolIdY = 2;
	int tilesize = 16;
	
	//PARSE PARAM
	initParserSampler(&parser);
	parser.addShortOption('x', &sobolIdX, 1, assignInt, displayInt, (char*)"<uint=1>\tSobol Indice on X dimension", (char*)"Sobol X");
	parser.addShortOption('y', &sobolIdY, 1, assignInt, displayInt, (char*)"<uint=2>\tSobol Indice on Y dimension", (char*)"Sobol Y");
	parser.addLongOption((char*)"tilesize", &tilesize, 1, assignInt, displayInt, (char*)"<uint=16>\tSamples per tile", (char*)"Samples per tile");

	//PARSING
	parser.parse(argc, argv);
	
	if(!dealParamParserSampler(&parser))
		return 0;
	
	int indices[10] = {sobolIdX, sobolIdY, 3, 4, 5, 6, 7 , 8, 9, 10};
	sampler.setIndices(indices, D);
	
	if(param_randxor)
	{
		ERROR("Cannot randomize using xor a set of floating point samples");
		return false;
	}
	
	int nbtiles = param_nbsamples/tilesize;
	int tiles_x = sqrt(nbtiles);
	param_nbsamples = tiles_x*tiles_x*tilesize;
	std::cout << "Nb tiles " << tiles_x << " - " << tiles_x<<"*"<<tiles_x<<"*"<<tilesize<<"=" << tiles_x*tiles_x*tilesize << std::endl;
	
	PointsetWriter<D, T, P> writer;
	writer.open(param_output.c_str());
	while(param_nbrealisations>0)
	{
		//SAMPLE
		Pointset<D, T, P> pts_tile;
		
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		if(!sampler.generateSamples<D, T, P>(pts_tile, tilesize))
			return 1;
		
		Pointset<D, T, P> pts;
		for(int i=0; i<tiles_x; i++)
		for(int j=0; j<tiles_x; j++)
		{
			double offsetx = (double)i/(double)tiles_x;
			double offsety = (double)j/(double)tiles_x;
			
			double cp_x = drand48();
			double cp_y = drand48();
			
			for(int p=0; p<tilesize; p++)
			{
				Point<D, T> pt;
				pt.pos()[0] = pts_tile[p].pos()[0] + cp_x;
				pt.pos()[1] = pts_tile[p].pos()[1] + cp_y;
				if(pt.pos()[0] >= 1) pt.pos()[0] -= 1;
				if(pt.pos()[1] >= 1) pt.pos()[1] -= 1;
				pt.pos()[0]/=tiles_x;
				pt.pos()[1]/=tiles_x;
				pt.pos()[0]+=offsetx;
				pt.pos()[1]+=offsety;
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