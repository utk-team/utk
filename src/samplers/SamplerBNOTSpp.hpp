/*
 * Coded by Hélène Perrier helene.perrier@liris.cnrs.fr
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
 * either expressed or implied, of the Halton project.
 */
#ifndef _UTK_SAMPLER_BNOT_SPP_
#define _UTK_SAMPLER_BNOT_SPP_

#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>

#include "../utils.hpp"
#include "../io/fileIO.hpp"
#include <sstream>

namespace utk
{

class SamplerBNOTSpp
{
protected:
public:

	SamplerBNOTSpp() { m_spp = 16; }

	void setSpp(int arg_spp) { m_spp = arg_spp; }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerBNOTSpp::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}

		if (!isFloatingType<T>())
		{
			ERROR("SamplerBNOTSpp::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
			return false;
		}
		
		int resolution = arg_points/m_spp;
		int size = sqrt(resolution);
		std::cout << arg_points << " samples, " << m_spp << " samples per pixel => " << size << "^2 pixel" << std::endl;
		
		if(resolution*m_spp != arg_points)
		{
			std::stringstream msg;
			msg << "SamplerBNOTSpp::generateSamples Can't subdivide " << arg_points << " into an integer number of blocks of " << m_spp << " samples" << std::endl;
			ERROR(msg.str());
			return false;
		}
		if(size*size != resolution)
		{
			std::stringstream msg;
			msg << "SamplerBNOTSpp::generateSamples Can't subdivide " << arg_points << " into an squared number of blocks of " << m_spp << " samples" << std::endl;
			ERROR(msg.str());
			return false;
		}
		
		//load domain & toricity
		for(uint d=0; d<D; d++)
		{
			arg_pts.domain.pMin.pos()[d] = 0;
			arg_pts.domain.pMax.pos()[d] = 1;
		}
		arg_pts.toricity = 1;

		arg_pts.resize(arg_points);
		
		for(int i=0; i<size*size; i++)
		{
			int x = i%size;
			int y = i/size;
			
			if(x == 0)
				std::cout << y << "/" << size << std::endl;
			
			std::stringstream cmd;
			cmd << "rm bnotspptmp.dat; ~/Desktop/Recherche/project_sampling/BlueLDS/src/samplers/bnot/build/pbnot -n " << m_spp << " -o bnotspptmp.dat >> /dev/null" << std::endl;
			system(cmd.str().c_str());
			
			Pointset< 2, double, Point<2, double> > pts2;
			PointsetReader<2, double, Point<2, double> > pts_read;
			pts_read.open("bnotspptmp.dat");
			pts_read.readPointset(pts2);
			pts_read.close();
			
			for(int n=0; n<m_spp; n++)
			{
				arg_pts[i*m_spp+n].pos()[0] = (pts2[n].pos()[0] * (1.0/(double)size)) + (double)x/(double)size;
				arg_pts[i*m_spp+n].pos()[1] = (pts2[n].pos()[1] * (1.0/(double)size)) + (double)y/(double)size;
				
				//arg_pts[i*m_spp+n].pos()[0] = pts2[n].pos()[0] +x;
				//arg_pts[i*m_spp+n].pos()[1] = pts2[n].pos()[1] +y;
			}	
		}

		return true;
	};

protected:
    int m_spp;
};

} //end namespace utk


#endif
