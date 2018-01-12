#include "../../parameters/ParamParser_getopt.hpp"
#include "../../integration/IntegrationGaussian2D.hpp"
#include "../../io/fileIO.hpp"
#include <chrono>
#include <fstream>

#include "runIntegration.hpp"

#define D 2

using namespace utk;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	IntegrationGaussian2D gauss;

	double param_stddev_x = 0.15;
	double param_mean_x = 0.5;
	double param_stddev_y = 0.15;
	double param_mean_y = 0.5;

	bool param_output_func=false;
	parser.addLongOption((char*)"stddevx", &param_stddev_x, 1, assignDouble, displayDouble, (char*)"<double=0.15>\tSets the x value for the standard deviation of the gaussian", (char*)"Stddev X");
	parser.addLongOption((char*)"meanx", &param_mean_x, 1, assignDouble, displayDouble, (char*)"<double=0.5>\tSets the x value for the mean of the gaussian", (char*)"Mean X");
	parser.addLongOption((char*)"stddevy", &param_stddev_y, 1, assignDouble, displayDouble, (char*)"<double=0.15>\tSets the y value for the standard deviation of the gaussian", (char*)"Stddev Y");
	parser.addLongOption((char*)"meany", &param_mean_y, 1, assignDouble, displayDouble, (char*)"<double=0.5>\tSets the y value for the mean of the gaussian", (char*)"Mean Y");
	
	parser.addLongOption((char*)"outputfunc", &param_output_func, 0, assignBoolTrue, displayBool, (char*)"Outputs the integrated function in func.dat (plottable with gnuplot)", (char*)"Output Func");
	
	
	initParserIntegration(&parser);
	
	parser.parse(argc, argv);
	
	if(!dealParamParserIntegration(&parser))
		return 0;
	
	gauss.setStddevX(param_stddev_x);
	gauss.setMeanX(param_mean_x);
	gauss.setStddevY(param_stddev_y);
	gauss.setMeanY(param_mean_y);

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
	}
	
	return 0;
}