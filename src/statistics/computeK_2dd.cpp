#include "../statistics/KLFunctions.hpp"
#include "../parameters/ParamParser_getopt.hpp"
#include "../io/fileIO.hpp"
#include "../io/histogramIO.hpp"
#include <chrono>

using namespace utk;

typedef double T;
#define D 2
typedef Point<D, T> P;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	
	bool param_help = false;
	bool param_verbose = true;
	
	std::string input_file;
	std::string output_file;
	
	double rmin = -1;
	double rmax = -1;
	uint nbbins = 50;
	bool normalized_distance=true;
	
	//PARSE PARAM
	parser.addLongOption((char*)"silent", &param_verbose, 0, utk::assignBoolFalse, utk::displayBool, (char*)"\t\tSilent mode", (char*)"Verbose");
	parser.addShortOption('h', &param_help, 0, utk::assignBoolTrue, NULL, "<string>\t\tDisplays this help message");
	
	parser.addShortOption('i', &input_file, 1, assignString, displayString, (char*)"<string> the input pointset", (char*)"Input");
	parser.addShortOption('o', &output_file, 1, assignString, displayString, (char*)"<string> the output PCF", (char*)"Output");
	
	parser.addLongOption((char*)"rmin", &rmin, 1, assignDouble, displayDouble, (char*)"<double=0.01> the minimum radius", (char*)"Rmin");
	parser.addLongOption((char*)"rmax", &rmax, 1, assignDouble, displayDouble, (char*)"<double=0.15> the maximal radius", (char*)"Rmax");
	parser.addLongOption((char*)"nbbins", &nbbins, 1, assignUInt, displayUInt, (char*)"<uint=200> the number of histogram bins", (char*)"Nb bins");
	parser.addLongOption((char*)"unnorm-dist", &normalized_distance, 0, assignBoolFalse, displayBool, (char*)"does not normalized the distances in the PCF", (char*)"Norm Dist");
	//PARSING
	parser.parse(argc, argv);
	
	if(input_file.empty())
	{
		ERROR("Parameter -i is mandatory");
		std::cout << parser.getHelp() << std::endl;
		return 0;
	}
	if(output_file.empty())
	{
		ERROR("Parameter -o is mandatory");
		std::cout << parser.getHelp() << std::endl;
		return 0;
	}
	if(param_help)
	{
		std::cout << parser.getHelp() << std::endl;
		return 0;
	}
	
	if(param_verbose)
		parser.displayState();
	
	KLFunctions k_engine;
	HistogramWriter<1, double, double> histo_writer;
	PointsetReader< D, T, P > pts_reader;
	pts_reader.open(input_file.c_str());
	Pointset< D, T, P > pts;
	int nb_real = 0;
	while(pts_reader.readPointset(pts))
	{
		double d = 2.0 * sqrt( 1.0 / (2.0*sqrt(3.0)*pts.size()) );
		if(rmin < 0)
			rmin = 1.5*0.1*d;
		else
			rmin = rmin*0.1*d;
		if(rmax < 0)
			rmax = 2.5*d;
		else
			rmax = rmax*d;
		
		Histogram1dd k_data;
		if(!k_engine.computeK<D, T, P>(pts, k_data, rmin, rmax, nbbins, normalized_distance))
			return 1;
		
		histo_writer.open(output_file);
		histo_writer.writeHistogram(k_data);
		histo_writer.close();
		
		nb_real++;
	}
	pts_reader.close();
	
	return 0;
}
