#include "../../samplers/SamplerSobolIndices.hpp"
#include "../../samplers/SamplerVanDerCorput.hpp"
#include "../../samplers/SamplerHammersley.hpp"
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
	
	//PARSE PARAM
	initParserSampler(&parser);
	parser.addShortOption('x', &sobolIdX, 1, assignInt, displayInt, (char*)"<uint=1>\tSobol Indice on X dimension", (char*)"Sobol X");
	parser.addShortOption('y', &sobolIdY, 1, assignInt, displayInt, (char*)"<uint=2>\tSobol Indice on Y dimension", (char*)"Sobol Y");

	//PARSING
	parser.parse(argc, argv);
	
	if(!dealParamParserSampler(&parser))
		return 0;
	
	int indices[10] = {sobolIdX, sobolIdY, 3, 4, 5, 6, 7 , 8, 9, 10};
	sampler.setIndices(indices, D);
	
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
		
		if(param_randxor)
		{	
			Vector<D, double> shift;
			for(int i=0; i<D; i++)
				shift[i] = drand48();
			pts.digit_xor(shift);
		}
		if(param_randcp)
		{	
			Vector<D, double> shift;
			for(int i=0; i<D; i++)
				shift[i] = drand48();
			pts.cranley_patterson(shift);
		}
		
		//WRITE
		writer.writePointset(pts);
		param_nbrealisations--;
		
		/*PointsetIllustrator< 2, double, Point<2, double> > illu;
		
		for(int i=0; i<pts.size()/16; i++)
		{
			Pointset<2, double, Point<2, double> > pts2;
			for(int j=0; j<16; j++)
				pts2.push_back(pts[i*16+j]);
			
			illu.setBoundingBox(1000);
			illu.setPointRadius(10);
			char a = '0'+i;
			illu.open( std::string("test")+a+".ps" );
			
			illu.drawGrid(16);
			illu.drawPointset(pts2);
			illu.close();
		}
		
		illu.setBoundingBox(1000);
		illu.setPointRadius(10);
		illu.setNumbered(1);
		illu.open( "all.ps" );
		
		illu.drawGrid(16);
		illu.drawPointset(pts);
		illu.close();*/
		
	}
	writer.close();
	
	/*uint pj[D] = { 0, 0 };
	uint dj[D] = { 1, 1 };
	std::vector<uint> idEI;
	sampler.getIndicesInElementaryInterval<D>(idEI, pj, dj);*/
	
	return 0;
}