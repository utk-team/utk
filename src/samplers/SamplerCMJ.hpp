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
#ifndef _UTK_SAMPLER_CMJ_
#define _UTK_SAMPLER_CMJ_

/**
Kensler, A. (2013). Correlated multi-jittered sampling. Pixar Tech Report
***/

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>

namespace utk
{

class SamplerCMJ
{
public:

    SamplerCMJ() { setRandomSeedTime(); }

	void setRandomSeed( long unsigned int arg_seed ) { m_mersenneTwister.seed(arg_seed); }
	void setRandomSeedTime() { m_mersenneTwister.seed(std::chrono::system_clock::now().time_since_epoch().count()); }

	template<unsigned int D, typename T, typename P>
    bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
    {
		if (!arg_pts.empty())
		{
			WARNING("SamplerCMJ::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		if (D != 2)
		{
			WARNING("SamplerCMJ::generateSamples is only a 2D sampler ...");
			return false;
		}

		if (!isFloatingType<T>())
		{
			ERROR("SamplerCMJ::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
			return false;
		}
		
        for(uint d=0; d<D; d++)
		{
			arg_pts.domain.pMin.pos()[d] = 0;
			arg_pts.domain.pMax.pos()[d] = 1;
		}
		arg_pts.toricity = 1;

        int n = sqrt(arg_points);
        if( n*n != arg_points )
            std::cerr << arg_points << " is not a perfect square, rounding up to " << n*n << " instead" << std::endl;

        arg_points=n*n;

        utk::Point<D,T>* p = new utk::Point<D,T>[arg_points];

        //Generate a canonical arrangement
        for(unsigned int i=0; i<n; i++)
        for(unsigned int j=0; j<n; j++)
        {
            p[i*n+j].pos()[0] = ( (double)i + ((double)j/(double)n) ) / (double)n;
            p[i*n+j].pos()[1] = ( (double)j + ((double)i/(double)n) ) / (double)n;
        }
        
        //Shuffle it
        //on x
        for(int j=0; j<n; j++)
        {
            float k = j + getRandom01() * (n-j);
            for(int i=0; i<n; i++)
            {
                std::swap(p[j*n+i].pos()[0], p[(int)k*n+i].pos()[0]);
            }
        }
        //on y
        for(int i=0; i<n; i++)
        {
            float k = i + getRandom01() * (n-i);
            for(int j=0; j<n; j++)
            {
                std::swap(p[j*n+i].pos()[1], p[j*n+(int)k].pos()[1]);
            }
        }

        for(unsigned int a=0; a<arg_points; a++)
            arg_pts.push_back(p[a]);

        delete [] p;
		
		return true;
    }
protected:
	std::mt19937 m_mersenneTwister;
    double getRandom01()
    {
        return (double)m_mersenneTwister()/(double)m_mersenneTwister.max();
    }
};

} //end namespace utk


#endif
