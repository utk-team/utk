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
#include "PCF.hpp"
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
	double smoothing = -1;
	uint nbbins = 200;
	bool normalized_distance=true;
	
	//PARSE PARAM
	parser.addLongOption((char*)"silent", &param_verbose, 0, utk::assignBoolFalse, utk::displayBool, (char*)"\t\tSilent mode", (char*)"Verbose");
	parser.addShortOption('h', &param_help, 0, utk::assignBoolTrue, NULL, "<string>\t\tDisplays this help message");
	
	parser.addShortOption('i', &input_file, 1, assignString, displayString, (char*)"<string> the input pointset", (char*)"Input");
	parser.addShortOption('o', &output_file, 1, assignString, displayString, (char*)"<string> the output PCF", (char*)"Output");
	
	parser.addLongOption((char*)"rmin", &rmin, 1, assignDouble, displayDouble, (char*)"<double=0.01> the minimum radius", (char*)"Rmin");
	parser.addLongOption((char*)"rmax", &rmax, 1, assignDouble, displayDouble, (char*)"<double=0.15> the maximal radius", (char*)"Rmax");
	parser.addLongOption((char*)"sigma", &smoothing, 1, assignDouble, displayDouble, (char*)"<double=0.1> the sigma of the gaussian used for smoothing", (char*)"Sigma");
	parser.addLongOption((char*)"nbbins", &nbbins, 1, assignUInt, displayUInt, (char*)"<uint=200> the number of histogram bins", (char*)"Nb bins");
	parser.addLongOption((char*)"unnorm-dist", &normalized_distance, 0, assignBoolFalse, displayBool, (char*)"does not normalize the distances in the PCF", (char*)"Norm Dist");
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
	
	PCF pcf_engine;
	HistogramWriter<1, double, double> histo_writer;
	PointsetReader< D, T, P > pts_reader;
	pts_reader.open(input_file.c_str());
	Pointset< D, T, P > pts;
	int nb_real = 0;
	while(pts_reader.readPointset(pts))
	{
		double d = 2.0 * sqrt( 1.0 / (2.0*sqrt(3.0)*pts.size()) );
		if(smoothing < 0)
			smoothing = 0.1;
		smoothing *= d;
		if(rmin < 0)
			rmin = 1.5*smoothing;
		//else
		//	rmin = rmin*smoothing;
		if(rmax < 0)
			rmax = 2.5*d;
		//else
		//	rmax = rmax*d;
		
		Histogram1dd pcf_data;
		if(!pcf_engine.compute<D, T, P>(pts, pcf_data, rmin, rmax, nbbins, smoothing, normalized_distance))
			return 1;
		
		histo_writer.open(output_file);
		histo_writer.writeHistogram(pcf_data);
		histo_writer.close();
		
		/*for(int i=1; i<pcf_data.size(); i++)
			pcf_data[i].second += pcf_data[i-1].second;
		
		histo_writer.open("PCF_cdf.dat");
		histo_writer.writeHistogram(pcf_data);
		histo_writer.close();*/
		
		nb_real++;
	}
	pts_reader.close();
	
	return 0;
}
