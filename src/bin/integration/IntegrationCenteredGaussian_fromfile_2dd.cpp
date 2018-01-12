#include "../../parameters/ParamParser_getopt.hpp"
#include "../../integration/IntegrationCenteredGaussian2D.hpp"
#include "../../io/fileIO.hpp"
#include <chrono>
#include <fstream>

#include "runIntegration.hpp"

//#include "../../../externals/CImg-1.6.9/CImg.h"
//using namespace cimg_library;

#define D 2

using namespace utk;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	IntegrationCenteredGaussian2D gauss;

	bool param_output_func=false;
	parser.addLongOption((char*)"outputfunc", &param_output_func, 0, assignBoolTrue, displayBool, (char*)"Outputs the integrated function in func.dat (plottable with gnuplot)", (char*)"Output Func");
	
	
	initParserIntegration(&parser);
	
	parser.parse(argc, argv);
	
	if(!dealParamParserIntegration(&parser))
		return 0;

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
		if(!gauss.compute< D, double, Point<D, double> >(pts, integration, analytical))
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
		
		int res=40;
		for(int i=0; i<res; i++)
		for(int j=0; j<res; j++)
		{
			Point<2, double> pt; pt.pos()[0] = (double)i/res; pt.pos()[1] = (double)j/res;
			file << i << " " << j << " " << gauss.sampleIntegrand(pt) << std::endl;
		}
		
		file.close();
		
		/*CImg<double> m_image_bw;
		m_image_bw = CImg<double>(512, 512, 1, 1);
		double ming = 10000;
		double maxg = 0;
		for(int i=0; i<512; i++)
		for(int j=0; j<512; j++)
		{
			Point<2, double> pt; pt.pos()[0] = (double)i/512.0; pt.pos()[1] = (double)j/512.0;
			double g = gauss.sampleIntegrand(pt);
			if(g > maxg) maxg = g;
			if(g < ming) ming = g;
		    m_image_bw.atXY(i, j) = g;
		}
		for(int i=0; i<512; i++)
		for(int j=0; j<512; j++)
		{
		    m_image_bw.atXY(i, j) = ((m_image_bw.atXY(i, j) - ming)/(maxg-ming))*255.0;
		}
		m_image_bw.save("centered_gauss.png");*/
	}
	
	return 0;
}