#include "../parameters/ParamParser_getopt.hpp"
#include "../integration/IntegrationNSphere.hpp"
#include "../io/fileIO.hpp"
#include <chrono>

#include "runIntegration.hpp"

#define D 3

using namespace utk;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	IntegrationNSphere disc;

	double param_radius = 0.25;
	bool param_output_func=false;
	parser.addLongOption((char*)"radius", &param_radius, 1, assignDouble, displayDouble, (char*)"<double>\tSets the radius of the n_sphere. Must be in ]0, 0.5[", (char*)"Radius");
	parser.addLongOption((char*)"outputfunc", &param_output_func, 0, assignBoolTrue, displayBool, (char*)"Outputs the integrated function in func.dat (plottable with gnuplot)", (char*)"Output Func");
	initParserIntegration(&parser);
	
	parser.parse(argc, argv);
	
	if(!dealParamParserIntegration(&parser))
		return 0;
	
	disc.setRadius(param_radius);

	PointsetReader< D, double, Point<D, double> > stream;
	stream.open(param_input);
	
	IntegrationStatistics stats;

	int size = 0;
	Pointset< D, double, Point<D, double> > pts;
	while(stream.readPointset(pts))
	{
		if(param_npts != 0)
			pts.resize(param_npts);
		
		if(stats.nbpts == 0)
			stats.nbpts = pts.size();
		else if(stats.nbpts != pts.size())
			WARNING("File contains pointsets of different sizes, averaging");
		
		double analytical = 0;
		double integration = 0;
		if(!disc.compute< D, double, Point<D, double> >(pts, integration, analytical))
			return 1;
		
		stats.setAnalytical(analytical);
			
		if(param_brute)
		{
			if(param_output.empty())
				std::cout << pts.size() << " " << integration << std::endl;
			else
			{
				std::ofstream file(param_output, std::ios::out);
				file << pts.size() << " " << integration << std::endl;
				file.close();
			}
		}
		else
			stats.addValue(integration);
		
		size+=pts.size();
		pts.clear();
	}
	
	stats.nbpts = size/stats.iter;
	
	if(!param_brute)
	{
		stats.computeStatistics();
		
		if(param_output.empty())
			std::cout << stats << std::endl;
		else
		{
			std::ofstream file(param_output, std::ios::out);
			file << stats << std::endl;
			file.close();
		}
	}
	
	if(param_output_func)
	{
		std::ofstream file("func.dat");
		
		for(int i=0; i<20; i++)
		for(int j=0; j<20; j++)
		for(int k=0; k<20; k++)
		{
			Point<3, double> pt; pt.pos()[0] = (20.0/2.0)-(double)i/20.0; pt.pos()[1] = (20.0/2.0)-(double)j/20.0; pt.pos()[2] = (20.0/2.0)-(double)k/20.0;
			file << i << " " << j << " " << k << " " << disc.sampleIntegrand(pt) << std::endl;
		}
		
		file.close();
	}
	
	return 0;
}
