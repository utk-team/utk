#include "../../parameters/ParamParser_getopt.hpp"
#include "../../io/fileIO.hpp"
#include "../../io/histogramIO.hpp"
#include "../../io/imageIO.hpp"
#include <chrono>
#include <fstream>

using namespace utk;

#define D 2

int main(int argc, char** argv)
{
	bool displayhelp=false;
	std::string param_input;
	std::string param_output;
	double radius=0.0025;
	bool tiled=false;
	bool numbering=false;
	bool drawborder=false;
	double offsetsize=0;
	int fontsize=10;
	
	utk::ParamParser_getopt parser;
	parser.addShortOption('i', &param_input, 1, utk::assignString, utk::displayString, (char*)"\t <string> The input pointset", (char*)"Input: ");
	parser.addShortOption('o', &param_output, 1, utk::assignString, utk::displayString, (char*)"\t <string> The output image", (char*)"Ouput: ");
	parser.addShortOption('h', &displayhelp, 0, utk::assignBoolTrue, utk::displayBool, (char*)"\t Displays this help message");
	parser.addLongOption((char*)"radius", &radius, 1, utk::assignDouble, utk::displayDouble, (char*)"\t [0< double <1 =0.0025] The point radius", (char*)"Radius: ");
	parser.addLongOption((char*)"tiled", &tiled, 0, utk::assignBoolTrue, utk::displayBool, (char*)"\t display 3x3 tiling of the point set", (char*)"Tiling: ");
	parser.addLongOption((char*)"numbering", &numbering, 0, utk::assignBoolTrue, utk::displayBool, (char*)"\t Numbered samples", (char*)"Numbering: ");
	parser.addLongOption((char*)"fontsize", &fontsize, 1, utk::assignInt, utk::displayInt, (char*)"\t [int=10] Font size", (char*)"Font size: ");
	parser.addLongOption((char*)"offset", &offsetsize, 1, utk::assignDouble, utk::displayDouble, (char*)"\t [0 <= int < 0.5 =0] Border size", (char*)"Offset size: ");
	parser.addLongOption((char*)"border", &drawborder, 0, utk::assignBoolTrue, utk::displayBool, (char*)"\t Numbered samples", (char*)"Border: ");
	
	/*parser.addLongOption((char*)"res", &resolution, 1, utk::assignDouble, utk::displayDouble, (char*)"\t [int=1024] The image resolution", (char*)"Resolution: ");
	parser.addLongOption((char*)"offset", &offsetsize, 1, utk::assignDouble, utk::displayDouble, (char*)"\t [0 <= int=0 < 0.5] Border size", (char*)"Offset size: ");*/
	
	parser.parse(argc, argv);	
	
	if(displayhelp)
	{
		std::cout << parser.getHelp() << std::endl;
		return 0;
	}
	if(param_input.empty())
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
	if(tiled && offsetsize != 0)
	{
		WARNING("tiled and offset can't be used together, ignoring offset");
		offsetsize = 0;
	}
	if(tiled && drawborder)
	{
		WARNING("tiled and border can't be used together, ignoring border");
		drawborder = false;
	}
	
	parser.displayState();
	
	PointsetReader<2, double, Point<2, double>> pts_reader;
	pts_reader.open(param_input);
	
	int iter = 0;
	Pointset<2, double, Point<2, double> > pts;
	while(pts_reader.readPointset(pts))
	{
		Pointset<2, double, Point<2, double> > pts2;
		pts.normalizePositions(pts2);
		
		PointsetIllustrator<2, double, Point<2, double>> illu;
		
		illu.setBorderSize(offsetsize);
		illu.setNumbered(numbering);
		illu.setFontSize(fontsize);
		illu.setTiled(tiled);
		illu.setPointRadius(radius);
		
		std::string basename = getBasename(param_output);
		std::string ext = getExt(param_output, "");
		std::string num = "";
		
		if(iter != 0)
		{
			std::stringstream ss;
			ss << iter;
			num = ss.str();
		}
		
		std::string filename = std::string(basename + num + ext);
		std::cout << basename << " + " << num << " + " << ext << std::endl;
		std::cout << filename << std::endl;

		illu.open(filename);
		illu.setColor(1, 0, 0);
		if(drawborder)
			illu.drawRectangle(offsetsize, offsetsize, 1-2*offsetsize, 1-2*offsetsize, 0.0025);
		illu.drawPointset(pts2);
		
		illu.close();
		iter++;
	}
	
	pts_reader.close();
	
	
	return 0;
}