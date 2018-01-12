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
	std::string param_input;
	std::string param_output;
	
	double resolution = 1024;
	double radius = 0.001;
	int fontsize=10;
	double offsetsize=0;
	double subset=1.0;
	
	bool numbering=false;
	bool displayhelp=false;
	bool drawborder=false;
	
	utk::ParamParser_getopt parser;
	parser.addShortOption('i', &param_input, 1, utk::assignString, utk::displayString, (char*)"\t <string> The input pointset", (char*)"Input: ");
	parser.addShortOption('o', &param_output, 1, utk::assignString, utk::displayString, (char*)"\t <string> The output image", (char*)"Ouput: ");
	parser.addShortOption('h', &displayhelp, 0, utk::assignBoolTrue, utk::displayBool, (char*)"\t Displays this help message");
	parser.addLongOption((char*)"subset", &subset, 1, utk::assignDouble, utk::displayDouble, (char*)"\t [0 < double < 1=1.0] Displays samples in [0, subset]^2");
	parser.addLongOption((char*)"radius", &radius, 1, utk::assignDouble, utk::displayDouble, (char*)"\t [0< double <1 =0.001] The point radius", (char*)"Radius: ");
	parser.addLongOption((char*)"res", &resolution, 1, utk::assignDouble, utk::displayDouble, (char*)"\t [int=1024] The image resolution", (char*)"Resolution: ");
	parser.addLongOption((char*)"numbering", &numbering, 0, utk::assignBoolTrue, utk::displayBool, (char*)"\t Numbered samples", (char*)"Numbering: ");
	parser.addLongOption((char*)"border", &drawborder, 0, utk::assignBoolTrue, utk::displayBool, (char*)"\t Numbered samples", (char*)"Border: ");
	parser.addLongOption((char*)"fontsize", &fontsize, 1, utk::assignInt, utk::displayInt, (char*)"\t [int=10] Font size", (char*)"Font size: ");
	parser.addLongOption((char*)"offset", &offsetsize, 1, utk::assignDouble, utk::displayDouble, (char*)"\t [0 <= int=0 < 0.5] Border size", (char*)"Offset size: ");
	
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
	
	parser.displayState();
	
	PointsetReader<2, double, Point<2, double>> pts_reader;
	pts_reader.open(param_input);
	
	Pointset<2, double, Point<2, double>> pts;
	pts_reader.readPointset(pts);
	pts_reader.close();
	
	std::cout << "Scaling pointset from [0," << subset << ") to [0, 1) ..." << std::endl;
	Pointset<2, double, Point<2, double>> subset_pts;
	for(int i=0; i<pts.size(); i++)
	{
		if(pts[i].pos()[0] < subset && pts[i].pos()[1] < subset)
		{
			Point<2, double> p;
			p.pos()[0] = pts[i].pos()[0] * (1.0/subset);
			p.pos()[1] = pts[i].pos()[1] * (1.0/subset);
			subset_pts.push_back(p);
		}
	}
	std::cout << "Subset contains " << subset_pts.size() << " pts" << std::endl;
	
	PointsetIllustrator<2, double, Point<2, double>> illu;
	
	illu.setBoundingBox(resolution);
	illu.setPointRadius(radius*resolution);
	illu.setFontSize(fontsize);
	illu.setBorderSize((offsetsize*resolution) +1);
	illu.setNumbered(numbering);
	illu.open(param_output);
	if(drawborder)
		illu.drawRectangle(0, 0, 1, 1, 0.0025);
	illu.drawPointset(subset_pts);
	illu.close();
	
	return 0;
}