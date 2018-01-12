#ifndef _UTK_INTEGRATION_IMAGE_2D_
#define _UTK_INTEGRATION_IMAGE_2D_

#include <cmath>
#include <array>

#include "../io/messageStream.hpp"
#include "../pointsets/Pointset.hpp"
#include "../../externals/CImg-1.6.9/CImg.h"

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
