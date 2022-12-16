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
#ifndef _UTK_INTEGRATION_IMAGE_2D_
#define _UTK_INTEGRATION_IMAGE_2D_

#include <cmath>
#include <array>

#include "../io/messageStream.hpp"
#include "../pointsets/Pointset.hpp"
#include "../../externals/CImg-3.2.0_pre110222/CImg.h"

using namespace cimg_library;

namespace utk
{
  
class IntegrationImage2D
{
public:
	IntegrationImage2D() { m_imagepath = ""; }

	bool setImage(std::string arg_imagepath) 
	{ 
		if(arg_imagepath=="")
		{
			ERROR("IntegrationImage2D::compute No image given");
			return false;
		}
		
		m_imagepath=arg_imagepath;
		m_image_rgb = CImg<double>(m_imagepath.c_str());
		m_imgheight = m_image_rgb.height();
		m_imgwidth = m_image_rgb.width();
		m_imgchannels = m_image_rgb.spectrum();
		convertToBW();
		
		return true;
	}

	template<unsigned int D, typename T, typename P>
	bool compute(const Pointset<D, T, P>& arg_pointset, double& integration_value, double& analytic_value)
	{	
		if(D!=2)
		{
			ERROR("IntegrationImage2D::compute Only 2D integration allowed");
			return false;
		}

		std::array<double, D> nd_domain_extent;
		for(unsigned int d=0; d<D; d++)
		{
			nd_domain_extent[d] = (arg_pointset.domain.pMax.pos() - arg_pointset.domain.pMin.pos())[d];

			if(nd_domain_extent[d] == 0)
			{
				WARNING("IntegrationImage2D::compute domain extent is 0 on at least one dimension, scaling might fail");
			}
			if(nd_domain_extent[d] < 0)
			{
				ERROR("IntegrationImage2D::compute domain extent is negative on at least one dimension");
				return false;
			}
		}

		analytic_value = analyticIntegrand();

		integration_value = 0;
		for(uint i=0; i<arg_pointset.size(); i++)
		{
			Point<D, double> pt;
			for(unsigned int d=0; d<D; d++)
				pt.pos()[d] = (arg_pointset[i].pos() - arg_pointset.domain.pMin.pos())[d] / nd_domain_extent[d];
			
			double l = sampleIntegrand(pt);
			integration_value+=l;
		}
		integration_value /= (double)arg_pointset.size();

		return true;
	}
	
	double analyticIntegrand()
	{
		double integrand=0;
		for(uint x=0; x<m_imgwidth; x++)
		for(uint y=0; y<m_imgheight; y++)
		{
			integrand += m_image_bw.atXY(x, y) / (double)(m_imgheight*m_imgwidth);
		}
		return integrand / (double)(m_imgheight*m_imgwidth);
	}
	
	double sampleIntegrand(Point<2, double>& pt)
	{
		return m_image_bw.atXY(pt.pos()[0]*m_imgwidth, pt.pos()[1]*m_imgheight) / (double)(m_imgwidth*m_imgheight);
	}
	
private:

	void convertToBW()
	{
		//TODO weight accordingly the human eye sensitivity to color ?
		//Converting to B&W by averaging over each channel
		m_image_bw = CImg<double>(m_imgwidth, m_imgheight, 1, 1);
		for(uint i=0; i<m_imgwidth; i++)
		for(uint j=0; j<m_imgheight; j++)
		{
			double bw_value=0;
			for(uint c=0; c<m_imgchannels; c++)
				bw_value += m_image_rgb.atXYZC(i, j, 0, c);
			m_image_bw.atXY(i, j) = (bw_value/(double)m_image_rgb.spectrum());
		}
	}
	
	std::string m_imagepath;
	CImg<double> m_image_rgb;
	CImg<double> m_image_bw;
	uint m_imgwidth = 0;
	uint m_imgheight = 0;
	uint m_imgchannels = 0;
  
};

}//end namespace

#endif
