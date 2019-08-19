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
#include "../spectral/Fourier2D.hpp"
#include "../io/fileIO.hpp"
#include "../io/histogramIO.hpp"
#include <chrono>
#include <fstream>

using namespace utk;

#define D 2

int main(int argc, char** argv)
{
	ParamParser_getopt parser;
	Fourier2D fourier;
	bool help = false;
	parser.addShortOption('h', &help, 0, assignBoolTrue, displayBool, (char*)"\tDisplay this help message", (char*)"");
	
	std::string param_pointset = "";
	parser.addShortOption('i', &param_pointset, 1, assignString, displayString, (char*)"<string>\tThe input pointset", (char*)"Input");
	
	double domain = 4.0;
	parser.addLongOption((char*)"domain", &domain, 1, assignDouble, displayDouble, (char*)"<double=4>\tThe fourier domain ( D*sqrt(N) )", (char*)"Domain");
	
	std::string param_fourier_file="";
	std::string param_power_file="";
	std::string param_radial_file="";
	parser.addLongOption((char*)"Fdat", &param_fourier_file, 1, assignString, displayString, (char*)"<string>\tThe file to write the fourier spectrum", (char*)"Fourier Dat");
	parser.addLongOption((char*)"Pdat", &param_power_file, 1, assignString, displayString, (char*)"<string>\tThe file to write the power spectrum", (char*)"Power Spectrum Dat");
	parser.addLongOption((char*)"Rdat", &param_radial_file, 1, assignString, displayString, (char*)"<string>\tThe file to write the radial spectrum", (char*)"Radial Dat");
	uint param_radial_nbbins=200;
	parser.addLongOption((char*)"radial-nbbins", &param_radial_nbbins, 1, assignUInt, displayUInt, (char*)"<uint=200>\tThe number of bins in the radial hitogram.", (char*)"Radial Dat Nbbins");
	
	std::string param_power_img="";
	parser.addShortOption('P', &param_power_img, 1, assignString, displayString, (char*)"<string>\tThe image to draw the power spectrum", (char*)"Power Spectrum Image");
	
	bool param_limit_size = false;
	parser.addLongOption((char*)"limit-size", &param_limit_size, 0, assignBoolTrue, displayBool, (char*)"\tIf set, the discretization will be limited onto 4096^2. \n\t\t\t This allows to compute the Fourier spectrum of big pointsets but changes the domain", (char*)"Limit size");
	
	bool param_log_bins = false;
	parser.addLongOption((char*)"log-bins", &param_log_bins, 0, assignBoolTrue, displayBool, (char*)"\tIf set, the radial bins size will increase logarithmically", (char*)"Log Bins");
	
	parser.parse(argc, argv);
	
	if(help)
	{
		std::cout << parser.getHelp() << std::endl;
		return 0;
	}
	
	if(param_pointset=="")
	{
		ERROR("Missing option -i");
		std::cout << parser.getHelp() << std::endl;
		return 1;
	}
	
	parser.displayState();

	PointsetReader< D, double, Point<D, double> > stream;
	stream.open(param_pointset);
	
	fourier.setDomain(domain);
	fourier.setNbBinsRadial(param_radial_nbbins);
	
	Histogram1dd radial_data;
	Histogram2dd spectrum_data;
	Histogram2dd power_spectrum_data;
	Histogram2dd image_spectrum_data;
	
	int nb = 0;
	int ptssize = 0;
	Pointset< D, double, Point<D, double> > pts;
	while(stream.readPointset(pts))
	{
		ptssize = pts.size();
		if(nb==0 && pts.size() <= 4096)
		{
			std::stringstream ss;
			ss << "The pointset size is of only " << pts.size() << " samples" << std::endl;
			WARNING(ss.str());
		}
		
		fourier.compute< 2, double, Point<2, double> >(pts, param_limit_size);
		
		if(radial_data.size() == 0)
		{
			radial_data.resize(fourier.radial_data.size());
			for(uint i=0; i<fourier.radial_data.size(); i++)
				radial_data[i] = fourier.radial_data[i];
		}
		else
		{
			for(uint i=0; i<fourier.radial_data.size(); i++)
				radial_data[i].second += fourier.radial_data[i].second;
		}
		
		
		if(spectrum_data.size() == 0)
		{
			spectrum_data.resize(fourier.spectrum_data.size());
			for(uint i=0; i<fourier.spectrum_data.size(); i++)
				spectrum_data[i] = fourier.spectrum_data[i];
		}
		else
		{
			for(uint i=0; i<fourier.spectrum_data.size(); i++)
				spectrum_data[i].second += fourier.spectrum_data[i].second;
		}
		
		
		if(power_spectrum_data.size() == 0)
		{
			power_spectrum_data.resize(fourier.power_spectrum_data.size());
			for(uint i=0; i<fourier.power_spectrum_data.size(); i++)
				power_spectrum_data[i] = fourier.power_spectrum_data[i];
		}
		else
		{
			if(fourier.power_spectrum_data.size() != power_spectrum_data.size())
				WARNING("Ignoring fourier spectrum in averaging as it is of a different size");
			else {
				for(uint i=0; i<fourier.power_spectrum_data.size(); i++)
					power_spectrum_data[i].second += fourier.power_spectrum_data[i].second;
			}
		}
		
		if(image_spectrum_data.size() == 0)
		{
			image_spectrum_data.resize(fourier.power_spectrum_data.size());
			for(uint i=0; i<fourier.power_spectrum_data.size(); i++)
			{
				image_spectrum_data[i] = fourier.power_spectrum_data[i];
				if( fourier.power_spectrum_data[i].second > 2 )
						image_spectrum_data[i].second = 2;
					else if( fourier.power_spectrum_data[i].second < 0 )
						image_spectrum_data[i].second = 0;
					else 
						image_spectrum_data[i].second = fourier.power_spectrum_data[i].second;
			}
		}
		else
		{
			if(fourier.power_spectrum_data.size() != power_spectrum_data.size())
				WARNING("Ignoring fourier spectrum in averaging as it is of a different size");
			else {
				for(uint i=0; i<fourier.power_spectrum_data.size(); i++)
				{
					if( fourier.power_spectrum_data[i].second > 2 )
						image_spectrum_data[i].second += 2;
					else if( fourier.power_spectrum_data[i].second < 0 )
						image_spectrum_data[i].second += 0;
					else 
						image_spectrum_data[i].second += fourier.power_spectrum_data[i].second;
				}
			}
		}
		
		nb++;
		
		pts.clear();
	}
	
	if(nb<10 && ptssize <= 4096)
	{
		std::stringstream ss;
		ss << "For a pointset size of " << ptssize << ", you should average on at least 10 realisations" << std::endl;
		WARNING(ss.str());
	}
	
	
	if(nb > 1)
	{
		for(uint i=0; i<radial_data.size(); i++)
			radial_data[i].second /= (double)nb;
		
		for(uint i=0; i<power_spectrum_data.size(); i++)
		{
			spectrum_data[i].second /= (double)nb;
			power_spectrum_data[i].second /= (double)nb;
			image_spectrum_data[i].second /= (double)nb;
		}
	}
	
	if(param_log_bins)
	{
		for(uint i=0; i<radial_data.size(); i++)
		{
			radial_data[i].first[0] = log(radial_data[i].first[0]);
			radial_data[i].second = log(radial_data[i].second);
		}
	}
		
	if(!param_fourier_file.empty())
	{
		HistogramWriter<2, double, double> writer;
		writer.open(param_fourier_file);
		writer.writeHistogram(spectrum_data);
		writer.close();
	}
	if(!param_power_file.empty())
	{
		HistogramWriter<2, double, double> writer;
		writer.open(param_power_file);
		writer.writeHistogram(power_spectrum_data);
		writer.close();
	}
	if(!param_radial_file.empty())
	{
		HistogramWriter<1, double, double> writer;
		writer.open(param_radial_file);
		writer.writeHistogram(radial_data);
		writer.close();
	}
	
	
	if(!param_power_img.empty())
	{
		uint m_res = sqrt(image_spectrum_data.size());
		CImg<unsigned char> spectrum(m_res, m_res, 1);
		for(uint i=0; i<m_res; i++)
		{
		for(uint j=0; j<m_res; j++)
		{
			double val = image_spectrum_data[j*m_res+i].second;
			spectrum.atXY(i, j) =  (val/2.0) * 255;
		}
		}
		spectrum.save( param_power_img.c_str() );
	}
	
	return 0;
}