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
	std::string output_fft;
	std::string output_radial;
	double nbbins = 50;
	
	bool help = false;
	int optch;
	extern int opterr;
	char format[] = "i:ho:r:b:";
	opterr = 1;
	while ((optch = getopt(argc, argv, format)) != -1)
	switch (optch) {
		case 'i':
			input_file = optarg;
			break;
		case 'o':
			output_fft = optarg;
			break;
		case 'r':
			output_radial = optarg;
			break;
		case 'b':
			nbbins = atoi(optarg);
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
	if (output_fft.empty() && output_radial.empty())
	{
		std:: cerr << "At least one of parameters -o or -r is required" << std::endl;
		help = true;
	}
	
	if (help)
	{
		std::cout << "Help" << std::endl;
		std::cout << "-i <input_file> \t file to read the pointsets from" << std::endl;
		std::cout << "-o \t outputs an fft image named \"<input_file>_fft.png\"" << std::endl;
		std::cout << "-r <radial_output> \t the histogram containing the radial power" << std::endl;
		std::cout << "-b <bin number=50> \t the number of bins for the radial power histogram" << std::endl;
		std::cout << "-h \t\t\t displays this help message" << std::endl;
		return 0;
	}
	
	printf("Computing image %s . . .\n", input_file.c_str());
	
	std::string basename, ext;
	splitBasenameExt(input_file, basename, ext);
	
	CImg<double> original_image;
	original_image.load(input_file.c_str());
	
	//padding to obtain a power of two image
	//naive algorithm => FUCKING SLOW
	
	//retreiving new size (the first power of two superior to the image size)
	double w2 = 1;
	while (w2 < original_image.width()) w2*=2;
	if (original_image.height() != original_image.width())
	while (w2 < original_image.height()) w2*=2;
	
	int diffw = (w2 - original_image.width())/2;
	int diffh = (w2 - original_image.height())/2;

	CImg<double> image(w2, w2, 1, 1);
	for(int x=0; x<w2; x++)
	for(int y=0; y<w2; y++)
	{
		if (x < diffw)
			image.atXY(x, y) = 0;
		else if (y < diffh)
			image.atXY(x, y) = 0;
		else if (x-diffw < original_image.width() && y-diffh < original_image.height())
		{
			int c = 0;
			if (original_image.spectrum() == 3)
				c = 1;
			image.atXY(x, y) = original_image.atXY(x-diffw, y-diffh, 0, c);
		}
		else
			image.atXY(x, y) = 0;
	}
	
	image /= 255.0;
	CImgList<double> F = image.get_FFT();
	cimglist_apply(F,shift)(image.width()/2,image.height()/2,0,0,2); //reorganizes FFT so that the zero fqcy is at the coordinates [image.width/2, image.height/2]
	CImg<double> mag_unnom = (F[0].get_pow(2) + F[1].get_pow(2)); //computes the magnitude of the power spectrum
	
	if (!output_radial.empty())
	{
		int m_res = mag_unnom.width();
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
					double c = mag_unnom.atXY(i, j); //dualSpacePow[j*m_res+i];
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
			val.first[0]  = (r/max_r * (double)1);
			val.second = histo_values[i]/histo_count[i];
			val.second /= 10e6;
			radial_data[i-1] = val;
		}
		
		utk::HistogramWriter<1, double, double> writer;
		writer.open(output_radial);
		writer.writeHistogram(radial_data);
		writer.close();
	
		/*std::ofstream file;
		file.open(output_radial.c_str(), std::ofstream::out | std::ofstream::app);
		
		double max_r = original_image.width()/2.0; //0.5 the side of the picture
		double bin_width = max_r/(nbbins+1); //otherwise, the last bin is outisde the spectrum
		
		//sum of all the values computed inside each bin
		double* histo_values = (double*)malloc(sizeof(double)*nbbins);
		
		//without analytical answer, we need to compute an iterative mean at each step to obtaina  variance.
		//this mean is NOT used as the final spectral mean since we would rather use the 
		//real area of the bin to average the values instead of approximating it
		//with the number of values that fell in the bin.
		double* histo_mean = (double*)malloc(sizeof(double)*nbbins);
		double* histo_variances = (double*)malloc(sizeof(double)*nbbins);
		double* histo_iter = (double*)malloc(sizeof(double)*nbbins);
		for(int i =0; i<nbbins; i++){
			histo_values[i] = 0;
			histo_mean[i] = 0;
			histo_variances[i] = 0;
			histo_iter[i] = 0;
		}
		
		double max = mag_unnom.atXY(0, 0);
		//double max = original_image.atXY(0, 0);
		for(int i=0; i<image.width(); i++)
		for(int j=0; j<image.height(); j++)
		{
			double r, theta;
			carte2polar(i-image.width()/2.0, j-image.width()/2.0, r, theta);
			
			for(int bin_id=0; bin_id<nbbins; bin_id++)
			{
				if (r >= bin_id*bin_width && r < (bin_id+1)*bin_width)
				{
					double c = mag_unnom.atXY(i, j);
					//double c = original_image.atXY(i, j);
					if (c > max)
						max = c;
					histo_values[bin_id] += c;
					
					//variance
					histo_iter[bin_id]++;
					double delta = c - histo_mean[bin_id];
					histo_mean[bin_id] += delta/histo_iter[bin_id];
					histo_variances[bin_id] += delta*(c - histo_values[bin_id]);
					
					//mag.at()
					image.atXY(i, j, 0, 0) = 25*bin_id; //(used to debug the sum by to displaying the bins)
					break;
				}
			}
		}
		//image.save("radial.png");
		
		double* histo = (double*)malloc(sizeof(double)*(nbbins));
		double max_histo = 0;
		
		double v = rand()%10;
		v/=10.0;
		double areabin = 0;
		for(int i=1; i<nbbins; i++)
		{
			double r = (i+1)*bin_width;
			double r2 = r*r;
			
			double r_2 = i*bin_width;
			double r2_2 = r_2*r_2;
			areabin = M_PI*r2 - M_PI*r2_2;

			histo[i] = (histo_values[i]/areabin);
			if (max_histo < histo[i])
				max_histo = histo[i];
		}
		for(int i=1; i<nbbins; i++)
		{
			if (normalized)
				file << i*bin_width << "\t" << histo[i]/max_histo << std::endl;
			else
 				file << i*bin_width << "\t" << histo[i] << std::endl;
			
			//std::cout << i*bin_width << "\t" << histo_variances[i] << std::endl;
		}
		
		free(histo_values);
		free(histo_variances);
		free(histo_iter);
		free(histo_mean);*/
	}
	
	if (!output_fft.empty())
	{
		CImg<unsigned char> mag = (mag_unnom.sqrt()+1).log().normalize(0,255);
		mag.save(output_fft.c_str());
	}
	
	printf("Done\n");
}
