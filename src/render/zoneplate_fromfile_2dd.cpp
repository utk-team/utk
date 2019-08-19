/*
 * Hélène Perrier helene.perrier@liris.cnrs.fr
 * and David Coeurjolly david.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2018 CNRS Université de Lyon
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the UTK project.
 */
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
	bool help = false;
	
	utk::ParamParser_getopt parser;
	parser.addShortOption('i', &fn_input, 1, utk::assignString, utk::displayString, (char*)"[string]\t The input pointset", (char*)"Input: ");
	parser.addShortOption('o', &fn_output, 1, utk::assignString, utk::displayString, (char*)"[string]\t The output zoneplate", (char*)"Ouput: ");
	parser.addShortOption('h', &help, 0, utk::assignBoolTrue, NULL, (char*)"\t Displays this help message");
	parser.addShortOption('z', &zoom, 1, utk::assignDouble, utk::displayDouble, (char*)"[double=1]\t Zoom factor", (char*)"Zoom: ");
	parser.addShortOption('p', &density, 1, utk::assignDouble, utk::displayDouble, (char*)"[uint=4]\t Nb samples per pixel", (char*)"Spp: ");
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
	std::cout << "Using " << pts.size()/(scale*scale) << " samples per pixel" << std::endl;
	//int rep = ceil(res/scale);
	
	for(uint i=0; i<pts.size(); i++)
		pts[i].pos() *= scale;
	
	Zoneplate zp(scale, 1/zoom, density);
	zp.make(pts);
	zp.writePng(fn_output);
	
	exit(EXIT_SUCCESS);
}
