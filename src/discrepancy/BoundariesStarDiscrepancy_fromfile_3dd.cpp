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
#include "BoundariesStarDiscrepancy.hpp"
#include "../io/fileIO.hpp"
#include <chrono>

#include "runDiscrepancy.hpp"

#define D 3

using namespace utk;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	BoundariesStarDiscrepancy disc;
	
	double param_epsilon=-1;
	bool param_epsilonauto=0;
	parser.addLongOption((char*)"epsilon", &param_epsilon, 1, assignDouble, displayDouble, (char*)"<double>\tSets the epsilon to use for the boundary computation. Must be in [0, 1]", (char*)"Epsilon Value");
	initParserDiscrepancy(&parser);
	parser.addLongOption((char*)"epsilon-auto", &param_epsilonauto, 0, assignBoolTrue, displayBool, (char*)"\tAutomatically sets the epsilon as log(N)^(d-1)/N", (char*)"Auto Epsilon");
	
	parser.parse(argc, argv);
	
	if(!dealParamParserDiscrepancy(&parser))
		return 0;
	if(param_epsilon != -1 && param_epsilonauto)
	{
		ERROR("Parameters --epsilon and --epsilon-auto cannot be used jointly");
		return 1;
	}
	if(param_epsilon == -1 && !param_epsilonauto)
	{
		ERROR("Parameter --epsilon required");
		return 1;
	}
	if(!param_epsilonauto && (param_epsilon <= 0 || param_epsilon >=1))
	{
		ERROR("Parameter --epsilon must be in [0, 1]");
		return 1;
	}

	PointsetReader< D, double, Point<D, double> > stream;
	stream.open(param_input);
	
	DiscrepancyStatistics statsBoundMin;
	DiscrepancyStatistics statsBoundMax;

	int size = 0;
	Pointset< D, double, Point<D, double> > pts;
	while(stream.readPointset(pts))
	{
		if(param_nbpts != 0)
			pts.resize(param_nbpts);
		
		if(statsBoundMin.nbpts == 0)
		{
			statsBoundMax.nbpts = pts.size();
			statsBoundMin.nbpts = pts.size();
		}
		else if(statsBoundMin.nbpts != pts.size())
			WARNING("File contains pointsets of different sizes, averaging");
		
		if(param_epsilonauto)
			param_epsilon = pow(log2(pts.size()), D-1)/(double)pts.size();
		
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		std::array<double, 2> discrepancy;
		if(!disc.compute< D, double, Point<D, double> >(pts, param_epsilon, discrepancy))
			return 1;
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		if(param_verbose)
			std::cout << std::fixed << std::setprecision(5) << "Discrepancy on " << pts.size() << " samples computed in " << time_span.count() << " secs" << std::endl;
				
		if(param_brute)
		{
			if(param_output.empty())
				std::cout << pts.size() << " " << param_epsilon << " " << discrepancy[0] << " " << discrepancy[1] << std::endl;
			else
			{
				std::ofstream file(param_output, std::ios::out);
				file << pts.size() << " " << param_epsilon << " " << discrepancy[0] << " " << discrepancy[1] << std::endl;
				file.close();
			}
		}
		else
		{
			statsBoundMin.addValue(discrepancy[0]);
			statsBoundMax.addValue(discrepancy[1]);
		}
		
		size+=pts.size();
		pts.clear();
	}
	
	statsBoundMax.nbpts = size/statsBoundMax.iter;
	statsBoundMin.nbpts = size/statsBoundMin.iter;
	
	if(!param_brute)
	{
		statsBoundMin.computeStatistics();
		statsBoundMax.computeStatistics();
		
		if(param_output.empty())
		{
			std::cout << "#Nbpts Epsilon MinBoundMin MaxBoundMax Iter" << std::endl;
			std::cout << statsBoundMin.nbpts << " " << param_epsilon << " " << statsBoundMin.min_discrepancy << " " << statsBoundMax.max_discrepancy << " " << statsBoundMax.iter << std::endl;
		}
		else
		{
			std::ofstream file(param_output, std::ios::out);
			file << "#Nbpts Epsilon MinBoundMin MaxBoundMax Iter" << std::endl;
			file << statsBoundMin.nbpts << " " << param_epsilon << " " << statsBoundMin.min_discrepancy << " " << statsBoundMax.max_discrepancy << " " << statsBoundMax.iter << std::endl;
			file.close();
		}
	}
	
	return 0;
}
