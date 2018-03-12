#include "../parameters/ParamParser_getopt.hpp"
#include "../io/fileIO.hpp"
#include "../io/histogramIO.hpp"
#include "../io/imageIO.hpp"
#include <chrono>
#include <fstream>

using namespace utk;

#define D 2

int main(int argc, char** argv)
{
	bool displayhelp=false;
	std::string param_input_begin;
	std::string param_output;
	
	utk::ParamParser_getopt parser;
	parser.addShortOption('i', &param_input_begin, 1, utk::assignString, utk::displayString, (char*)"\t <string> The begin pointset", (char*)"Input: ");
	parser.addShortOption('o', &param_output, 1, utk::assignString, utk::displayString, (char*)"\t <string> The output images", (char*)"Ouput: ");

	parser.parse(argc, argv);	
	
	if(displayhelp)
	{
		std::cout << parser.getHelp() << std::endl;
		return 0;
	}
	if(param_input_begin.empty())
	{
		ERROR("Param -i mandatory");
		std::cout << parser.getHelp() << std::endl;
		return 1;
	}
	if(param_output.empty())
	{
		ERROR("Param -o mandatory");
		std::cout << parser.getHelp() << std::endl;
		return 1;
	}
	
	parser.displayState();
	
	PointsetReader<2, uint, Point<2, uint>> pts_reader;
	pts_reader.open(param_input_begin);
	Pointset<2, uint, Point<2, uint> > pts_begin;
	pts_reader.readPointset(pts_begin);
	pts_reader.close();
	
	Pointset<2, double, Point<2, double> > pts_norm;
	pts_begin.normalizePositions(pts_norm);
	
	PointsetIllustrator<2, double, Point<2, double>> illu;
	
	PointsetWriter<2, double, Point<2, double>> pts_writer;
	pts_writer.open(param_output);
	pts_writer.writePointset(pts_norm);
	pts_writer.close();
	
	return 0;
}
