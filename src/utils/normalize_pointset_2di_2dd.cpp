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
