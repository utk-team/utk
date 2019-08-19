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
 * either expressed or implied, of the Halton project.
 */
#include "../parameters/ParamParser_getopt.hpp"
#include "../discrepancy/L2Discrepancy.hpp"
#include "../io/fileIO.hpp"
#include <chrono>

#include "runDiscrepancy.hpp"

#define D 3

using namespace utk;

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	L2Discrepancy disc;
	
	initParserDiscrepancy(&parser);
	
	parser.parse(argc, argv);
	
	if(!dealParamParserDiscrepancy(&parser))
		return 0;

	PointsetReader< D, double, Point<D, double> > stream;
	stream.open(param_input);
	
	DiscrepancyStatistics stats;

	int size = 0;
	Pointset< D, double, Point<D, double> > pts;
	while(stream.readPointset(pts))
	{
		if(param_nbpts != 0)
			pts.resize(param_nbpts);
		
		if(stats.nbpts == 0)
			stats.nbpts = pts.size();
		else if(stats.nbpts != pts.size())
			WARNING("File contains pointsets of different sizes, averaging");
		
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		double discrepancy = 0;
		if(!disc.compute< D, double, Point<D, double> >(pts, discrepancy))
			return 1;
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		if(param_verbose)
			std::cout << std::fixed << std::setprecision(5) << "Discrepancy on " << pts.size() << " samples computed in " << time_span.count() << " secs" << std::endl;
				
		if(param_brute)
		{
			if(param_output.empty())
				std::cout << pts.size() << " " << discrepancy << std::endl;
			else
			{
				std::ofstream file(param_output, std::ios::out);
				file << pts.size() << " " << discrepancy << std::endl;
				file.close();
			}
		}
		else
			stats.addValue(discrepancy);
		
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
	
	return 0;
}
