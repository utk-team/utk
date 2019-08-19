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
#include "../io/histogramIO.hpp"
#include "../parameters/ParamParser_getopt.hpp"

#include <iostream>

int main(int argc, char** argv)
{
	std::string file1;
	std::string file2;
	
	srand(time(NULL));
	
	utk::ParamParser_getopt parser;
	parser.addLongOption((char*)"h1", &file1, 1, utk::assignString, utk::displayString, (char*)"\t <string> Histogram 1", (char*)"Histo 1: ");
	parser.addLongOption((char*)"h2", &file2, 1, utk::assignString, utk::displayString, (char*)"\t <string> Histogram 2", (char*)"Histo 2: ");
	parser.parse(argc, argv);
	
	if (file1.empty())
	{
		ERROR("Parameter --h1 mandatory");
		std::cout << parser.getHelp() << std::endl;
		return 1;
	}
	if (file2.empty())
	{
		ERROR("Parameter --h2 mandatory");
		std::cout << parser.getHelp() << std::endl;
		return 1;
	}
	
	utk::HistogramReader<1, double, double> historeader;
	utk::Histogram1dd histo1;
	utk::Histogram1dd histo2;
	
	historeader.open(file1);
	historeader.readHistogram(histo1);
	historeader.close();
	historeader.open(file2);
	historeader.readHistogram(histo2);
	historeader.close();
	
	double l2, linf;
	histo1.Linf(histo2, linf);
	histo1.L2(histo2, l2);
	
	
	//std::cout << linf << std::endl;
	std::cout << "Distance between Histograms:" << std::endl;
	std::cout << "\t Linf:" << linf << std::endl;
	std::cout << "\t L2:" << l2 << std::endl;
	
	return 0;
}
