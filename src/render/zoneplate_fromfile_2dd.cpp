/**********************************************
 code from STK https://github.com/stk-team/stk
 * Adrien PILLEBOUE © 2012
 *
 * TODO:
 * - Reintroduce the influence of samples' weights 
 * - rework zoom and resolution parameters to better fit what we need
 *********************************************/

#include <cstdlib>
#include <iostream>
#include <string>
#include <cmath>
#include "Zoneplate2D.hpp"

int main(int argc, char** argv)
{
	srand48(time(NULL));
	
	/* ARG PARSER *****************************************************/
	std::string fn_input;
	std::string fn_output;
	std::string fn_weight;
	double density=4;
	double zoom=1;
	int res=512;
	bool help = false;
	
	utk::ParamParser_getopt parser;
	parser.addShortOption('i', &fn_input, 1, utk::assignString, utk::displayString, (char*)"\t The input pointset", (char*)"Input: ");
	parser.addShortOption('o', &fn_output, 1, utk::assignString, utk::displayString, (char*)"\t The output zoneplate", (char*)"Ouput: ");
	parser.addShortOption('r', &res, 1, utk::assignInt, utk::displayInt, (char*)"\t The image resolution", (char*)"Resolution: ");
	parser.addShortOption('h', &help, 0, utk::assignBoolTrue, NULL, (char*)"\t Displays this help message");
	parser.addShortOption('z', &zoom, 1, utk::assignDouble, utk::displayDouble, (char*)"\t Zoom factor", (char*)"Zoom: ");
	parser.addShortOption('p', &density, 1, utk::assignDouble, utk::displayDouble, (char*)"\t Nb samples per pixel", (char*)"Spp: ");
	parser.parse(argc, argv);
	
	if(fn_input.empty())
	{
		ERROR("Parameter -i mandatory");
		std::cout << parser.getHelp() << std::endl;
		return 1;
	}
	if(help)
	{
		std::cout << parser.getHelp() << std::endl;
		return 0;
	}

	density = sqrt(density);
	
	/* INIT ***********************************************************/
	utk::Pointset<2, double, utk::Point<2, double> > pts;
	utk::PointsetReader<2, double, utk::Point<2, double> > pts_reader;
	pts_reader.open(fn_input);
	pts_reader.readPointset(pts);
	pts_reader.close();
	
	double scale = sqrt(pts.size())/density;
	std::cout << "Scaling " << scale << std::endl;
	//int rep = ceil(res/scale);
	
	for(uint i=0; i<pts.size(); i++)
	{
		pts[i].pos() *= scale;
	}
	
	/* TEST ***********************************************************/
	Zoneplate zp(res, 1/zoom, density);
	zp.make(pts);
	zp.writePng(fn_output);
	
	exit(EXIT_SUCCESS);
}
