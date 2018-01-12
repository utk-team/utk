#include <getopt.h>
#include <fstream>
#include "../../../externals/CImg-1.6.9/CImg.h"
#include <iostream>

using namespace cimg_library;

int main(int argc, char** argv)
{
	std::string input_file;
	std::string output_file;
	int nblevels=1000;
	bool cdf=false;
	
	srand(time(NULL));
	
	bool help = false;
	bool random_shift = false;
	int optch;
	extern int opterr;
	char format[] = "i:o:hl:c";
	opterr = 1;
	while ((optch = getopt(argc, argv, format)) != -1)
	switch (optch) {
		case 'i':
			input_file = optarg;
			break;
		case 'o':
			output_file = optarg;
			break;
		case 'l':
			nblevels=atoi(optarg);
			break;
		case 'c':
			cdf=true;
			break;
		case 'h':
			help = true;
			break;
	}
	
	if (input_file.empty())
	{
		std:: cerr << "Parameter -i is required" << std::endl;
		help = true;
	}
	if (help)
	{
		std::cout << "Help" << std::endl;
		std::cout << "-i <input_file> \t image to compute" << std::endl;
		std::cout << "-o [output_file] \t if set, the resulting histogram will be written into the given file" << std::endl;
		std::cout << "-l [nblevels] \t the number of bins in the histogram (default:1000)" << std::endl;
		std::cout << "-c \t returns the CDF of the histogram instead" << std::endl;
		std::cout << "-h \t\t\t displays this help message" << std::endl;
		return 0;
	}
	
	CImg<float> hdrimage;
	hdrimage.load(input_file.c_str());
	CImg<float> histo =  hdrimage.histogram(nblevels);
	
	float min_val = hdrimage.min();
	float max_val = hdrimage.max();
	std::cout << min_val << " -- " << max_val << std::endl;
	
	if (output_file.empty())
	{
		if (cdf)
		{
			float cdf_val=0;
			for(int i=0; i<histo.width(); i++)
			{
				cdf_val += histo.atX(i);
				histo.atX(i) = cdf_val;
			}
			for(int i=0; i<histo.width(); i++)
			{
				std::cout << i << "\t" << histo.atX(i)/histo.atX(histo.width()-1) << std::endl;
			}
		}
		else
		{
			for(int i=0; i<histo.width(); i++)
			{
				std::cout << i << "\t" << histo.atX(i) << std::endl;
			}
		}
	}
	else
	{
		std::ofstream file;
		file.open(output_file.c_str(), std::ofstream::out | std::ofstream::app);
		
		if (cdf)
		{
			float cdf_val=0;
			for(int i=0; i<histo.width(); i++)
			{
				cdf_val += histo.atX(i);
				histo.atX(i) = cdf_val;
			}
			float pix_val=min_val;
			for(int i=0; i<histo.width(); i++)
			{
				file << i << "\t" << histo.atX(i)/histo.atX(histo.width()-1) << std::endl;
				pix_val+=(max_val-min_val)/(float)histo.width();
			}
		}
		else
		{
			for(int i=0; i<histo.width(); i++)
			{
				file << i << "\t" << histo.atX(i) << std::endl;
			}
		}
		
		file.close();
	}
	return 0;
}