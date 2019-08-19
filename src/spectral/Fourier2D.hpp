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
#ifndef UTK_FOURIER2D_HPP
#define UTK_FOURIER2D_HPP

#include <cstdlib>
#include <iostream>
#include <string>
#include <complex>
#include <fstream>
#include <vector>

#include <fftw3.h>

#include "../io/messageStream.hpp"
#include "../pointsets/Pointset.hpp"
#include "../pointsets/Histogram.hpp"

#include "../../externals/CImg-1.6.9/CImg.h"
using namespace cimg_library;

namespace utk
{

class Fourier2D
{
public:
	
	Histogram1dd radial_data;
	Histogram2dd spectrum_data;
	Histogram2dd power_spectrum_data;
	
	Fourier2D(){
		m_domain = 4;
		m_radial_nbbins=200;
	}
	
	void setDomain(double arg_domain) { m_domain = arg_domain; }
	void setNbBinsRadial(uint arg_nbbins) { m_radial_nbbins = arg_nbbins; }
	
	template < uint D, typename T, typename P>
	bool compute(const Pointset<D, T, P>& arg_pts, bool limit_size=0)
	{		
		uint nPts= arg_pts.size();
		
		uint size = (int)round(4.0 * std::sqrt(nPts)) / 4.0;
		if(size > 256 && limit_size)
			size = 256;
		
		uint m_res = m_domain * 2 * size;
		uint m_hires = m_domain * m_res;
		m_hires = 1 << (int)ceil(log2(m_hires));
		int start = (m_hires - m_res) / 2;

		std::cout << "NPts:" << nPts << std::endl;
		std::cout << "FFT Resolution:" << m_hires << std::endl;
		std::cout << "Final Resolution:" << m_res << std::endl;
		
		fftw_complex* fftPrimal = (fftw_complex*) fftw_malloc(m_hires*m_hires*sizeof(fftw_complex));
		fftw_complex* fftDual = (fftw_complex*) fftw_malloc(m_hires*m_hires*sizeof(fftw_complex));
		fftw_plan fftp = fftw_plan_dft_2d(m_hires, m_hires, fftPrimal, fftDual, -1.0, FFTW_MEASURE);
	
		//CImg<double> primalSpace(m_res, m_res, 1);	
		//CImg< std::complex<double> > dualSpace(m_res, m_res, 1);
		//CImg<double> dualSpaceAmp(m_res, m_res, 1);
		//m_dualSpacePow = CImg<double>(m_res, m_res, 1);
		
		//Pointset<D, double, Point<D, double> > arg_pts_normalized = arg_pts.normalizePositions();
		for(uint p=0; p<arg_pts.size(); p++)
		{
			int x = (int)(m_hires*arg_pts[p].pos()[0])%m_hires;
			int y = (int)(m_hires*arg_pts[p].pos()[1])%m_hires;
			fftPrimal[y*m_hires+x][0] = 1;//arg_pts[p].val();
		}
		
		//Compute FFT
		fftw_execute(fftp);
		
		std::complex<double>* dualSpace = (std::complex<double>*) malloc(sizeof(std::complex<double>)*m_res*m_res);
		double* dualSpaceAmp = (double*) malloc(sizeof(double)*m_res*m_res);
		double* dualSpacePow = (double*) malloc(sizeof(double)*m_res*m_res);
		
		for(uint y = 0; y < m_res; y++)
		for(uint x = 0; x < m_res; x++)
		{
			int sx = (x+m_hires/2 + start)%m_hires;
			int sy = (y+m_hires/2 + start)%m_hires;
			int k = (sy) * m_hires + sx;
			
			std::complex<double> val (fftDual[k][0], fftDual[k][1]);
			dualSpace[x+m_res*y] = val;
			dualSpaceAmp[x+m_res*y] = std::abs(val);
			dualSpacePow[x+m_res*y] = pow(std::abs(val), 2)/nPts;
		}
		
		spectrum_data.resize(m_res*m_res);
		power_spectrum_data.resize(m_res*m_res);
		for(uint y = 0; y < m_res; y++)
		for(uint x = 0; x < m_res; x++)
		{
			HistogramValue<2, double, double> val;
			val.first[0] = 2*( (double)x/(double)m_res * m_domain ) - m_domain;
			val.first[1] = 2*( (double)y/(double)m_res * m_domain ) - m_domain;
			
			val.second = dualSpaceAmp[x+m_res*y];
			spectrum_data[x+m_res*y] = val;
			
			val.second = dualSpacePow[x+m_res*y];
			power_spectrum_data[x+m_res*y] = val;
		}

		uint nbbins = m_radial_nbbins;
		double max_r = m_res/2.0; //0.5 the side of the picture
		double bin_width = max_r/(nbbins+1); //otherwise, the last bin is outisde the spectrum
		
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
					double c = dualSpacePow[j*m_res+i];
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
			HistogramValue<1, double, double> val;
			val.first[0]  = (r/max_r * (double)m_domain);
			val.second = histo_values[i]/histo_count[i];
			radial_data[i-1] = val;
		}
		
		free(dualSpace);
		free(dualSpaceAmp);
		free(dualSpacePow);
		
		fftw_free(fftPrimal);
		fftw_free(fftDual);
		
		return true;
	}
	
private:	
	
	double m_domain;
	uint m_radial_nbbins;
	
	void carte2polar(double x, double y, double &r, double &theta)
	{
		r = sqrt(x*x+y*y);
		if (x != 0)
			theta = atan(y/x);
		else
			theta = 0;
	}
	
};

}//end namespace utk

#endif