#include <getopt.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>

#include "../../../externals/CImg-1.6.9/CImg.h"
#include "../../pointsets/Histogram.hpp"
#include "../../io/histogramIO.hpp"

using namespace cimg_library;

void splitBasenameExt(std::string f, std::string &basename, std::string &ext)
{
	basename = f.substr(0, f.find_last_of('.'));
	ext = f.substr(f.find_last_of('.'));
}

void carte2polar(double x, double y, double &r, double &theta)
{
	r = sqrt(x*x+y*y);
	if (x != 0)
		theta = atan(y/x);
	else
		theta = 0;
}

int main(int argc, char** argv)
{
	srand((long int)argv);
	
	std::string input_file;
	std::string output_radial;
	double nbbins = 200;
	bool normalized = false;
	
	bool help = false;
	int optch;
	extern int opterr;
	char format[] = "i:ho:b:n";
	opterr = 1;
	while ((optch = getopt(argc, argv, format)) != -1)
	switch (optch) {
		case 'i':
			input_file = optarg;
			break;
		case 'o':
			output_radial = optarg;
			break;
		case 'b':
			nbbins = atoi(optarg);
			break;
		case 'n':
			normalized = true;
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
	if (output_radial.empty())
	{
		std:: cerr << "Parameter -o is required" << std::endl;
		help = true;
	}
	
	if (help)
	{
		std::cout << "Help" << std::endl;
		std::cout << "-i <input_file> \t file to read the image from" << std::endl;
		std::cout << "-o <radial_output> \t the histogram containing the radial power" << std::endl;
		std::cout << "-b <bin number=50> \t the number of bins for the radial power histogram" << std::endl;
		std::cout << "-n \t if set, the histogram will be normalized (apart form the DC)" << std::endl;
		std::cout << "-h \t\t\t displays this help message" << std::endl;
		return 0;
	}
	
	printf("Computing image %s . . .\n", input_file.c_str());
	
	std::string basename, ext;
	splitBasenameExt(input_file, basename, ext);
	
	uint m_domain = 1;
	CImg<double> original_image;
	original_image.load(input_file.c_str());

	int m_res = original_image.width();
	double max_r = m_res/2.0; //0.5 the side of the picture
	double bin_width = max_r/(nbbins+1); //otherwise, the last bin is outisde the spectrum
	
	utk::Histogram1dd radial_data;
	
	//sum of all the values computed inside each bin
	std::vector<double> histo_values(nbbins, 0);
	std::vector<double> histo_count(nbbins, 0);

	for(uint i=0; i<m_res; i++)
	for(uint j=0; j<m_res; j++)
	{
		double r, theta;
		carte2polar(i-m_res/2.0, j-m_res/2.0, r, theta);
		
		for(uint bin_id=0; bin_id<nbbins; bin_id++)
		{
			if (r >= bin_id*bin_width && r < (bin_id+1)*bin_width)
			{
				double c = original_image.atXY(i, j); //dualSpacePow[j*m_res+i];
				histo_values[bin_id] += c;
				histo_count[bin_id]++;
				break;
			}
		}
	}

	radial_data.resize(nbbins-1);
	//double areabin = 0;
	for(uint i=1; i<nbbins; i++)
	{
		double r = i*bin_width;
		utk::HistogramNDValue<1, double, double> val;
		val.first[0]  = (r/max_r * (double)m_domain);
		val.second = histo_values[i]/histo_count[i];
		radial_data[i-1] = val;
	}
	
	utk::HistogramWriter<1, double, double> writer;
	writer.open(output_radial);
	writer.writeHistogram(radial_data);
	writer.close();
	
	return 0;
}
