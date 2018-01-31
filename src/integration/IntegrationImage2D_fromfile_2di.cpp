#include "../parameters/ParamParser_getopt.hpp"
#include "../integration/IntegrationImage2D.hpp"
#include "../io/fileIO.hpp"
#include <chrono>
#include <fstream>

#include "runIntegration.hpp"

#define D 2

using namespace utk;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	IntegrationImage2D integrator;

	std::string param_image = "";
	parser.addLongOption((char*)"image", &param_image, 1, assignString, displayString, (char*)"<string>\tThe image to integrate", (char*)"Image");
	initParserIntegration(&parser);
	
	parser.parse(argc, argv);
	
	if(!dealParamParserIntegration(&parser))
		return 0;
	if(param_image=="")
	{
		std::cout << "Parameter --image is mandatory" << std::endl;
		return 0;
	}
	
	if(!integrator.setImage(param_image))
		return 1;

	PointsetReader< D, uint, Point<D, uint> > stream;
	stream.open(param_input);
	
	IntegrationStatistics stats;

	int size = 0;
	Pointset< D, uint, Point<D, uint> > pts;
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
		if(!integrator.compute< D, uint, Point<D, uint> >(pts, integration, analytical))
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
	
	return 0;
}
