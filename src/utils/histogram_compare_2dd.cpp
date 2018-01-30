#include "../../io/histogramIO.hpp"
#include "../../parameters/ParamParser_getopt.hpp"

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