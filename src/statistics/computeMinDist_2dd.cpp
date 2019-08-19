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
#include "../statistics/MinimalDistance.hpp"
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

	bool toric=true;
	
	//PARSE PARAM
	parser.addLongOption((char*)"silent", &param_verbose, 0, utk::assignBoolFalse, utk::displayBool, (char*)"\t\tSilent mode", (char*)"Verbose");
	parser.addShortOption('h', &param_help, 0, utk::assignBoolTrue, NULL, "<string>\t\tDisplays this help message");
	
	parser.addShortOption('i', &input_file, 1, assignString, displayString, (char*)"<string> the input pointset", (char*)"Input");
	
	parser.addLongOption((char*)"non-toric", &toric, 0, assignBoolFalse, displayBool, (char*)"does not compute toric distances", (char*)"Toric");
	//PARSING
	parser.parse(argc, argv);
	
	if(input_file.empty())
	{
		ERROR("Parameter -i is mandatory");
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
	
	MinimalDistance md_engine;
	PointsetReader< D, T, P > pts_reader;
	pts_reader.open(input_file.c_str());
	Pointset< D, T, P > pts;

	int nb_iter = 0;
	double mindist_avg=0;
	double mindist_var=0;
	double max_mindist=0;
	double min_mindist=1;
	
	while(pts_reader.readPointset(pts))
	{
		double mindist;
		if(!md_engine.compute<D, T, P>(pts, toric, mindist))
			return 1;
		nb_iter++;

		double delta = mindist - mindist_avg;
		mindist_avg += delta/nb_iter;
		mindist_var += delta*(mindist - mindist_avg);
		
		if(nb_iter == 1) {
			min_mindist = mindist;
			max_mindist = mindist;
		} else {
			if(mindist > max_mindist)
				max_mindist = mindist;
			if(mindist < min_mindist)
				min_mindist = mindist;
		}
	}
	if(nb_iter <= 1)
		mindist_var=0;
	else
		mindist_var /= nb_iter-1;
	
	std::cout << "#Samples\t#Avg MinDist\t#Var MinDist\t#Max MinDist\t#Min MinDist" << std::endl;
	std::cout << pts.size() << "\t" << mindist_avg << "\t" << mindist_var << "\t" << max_mindist << "\t" << min_mindist << std::endl;
	pts_reader.close();
	
	return 0;
}
