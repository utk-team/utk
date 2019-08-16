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
#ifndef _UTK_SAMPLER_KOROBOV_
#define _UTK_SAMPLER_KOROBOV_

#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>

#include "../utils.hpp"

namespace utk
{

class SamplerKorobov
{
protected:
public:

	SamplerKorobov() { gen_a = 3; }
	
	void setGeneratorA(int arg_a) { gen_a = arg_a; }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerKorobov::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}

		if (!isFloatingType<T>())
		{
			ERROR("SamplerKorobov::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
			return false;
		}
		
		if( gen_a != 0 && (arg_points)/(gen_a) == (double)(arg_points)/(double)(gen_a))
		{
			WARNING("SamplerKorobov::generateSamples the number of points and the generator are not prime wr to each other ...");
		}

		//load domain & toricity
		for(uint d=0; d<D; d++)
		{
			arg_pts.domain.pMin.pos()[d] = 0;
			arg_pts.domain.pMax.pos()[d] = 1;
		}
		arg_pts.toricity = 1;
		arg_pts.resize(arg_points);
		
		utk::Vector<D, double> w;
		for(int j=0; j<D; j++)
				w[j] = (double)((int)pow(gen_a, j)%arg_points);
		w[0] = 1;
		w[1] = gen_a;
		
		//std::cout << w << std::endl;
		
		for(uint i = 0; i < arg_points; i++)
		{
			//for(uint d = 0; d < D; d++)
			arg_pts[i].pos() =  (double)(i)/(double)(arg_points) * w;
			for(uint d = 0; d < D; d++)
				arg_pts[i].pos()[d] = fmod(arg_pts[i].pos()[d], 1.0);
		}
		
		/*std::vector<int> counts;
		counts.resize(D+1, 0); /// D+1 to avoid overflow
		for(uint i = 0; i < arg_points; i++)
		{
			for(uint j = 0; j < D; j++)
				std::cout << counts[j] << " ";
			std::cout << std::endl;
			
			for(uint d = 0; d < D; d++) arg_pts[i].pos()[d] = 0;
			for(uint d = 0; d < D; d++)
				arg_pts[i].pos() += counts[d] * wi[d];

			counts[0] += 1;
			for(uint j = 1; j < D; j++)
			{
				if(arg_pts[i].pos()[j] >= 1)
				{
					counts[j-1] = 0;
					counts[j] += 1;
				}
			}
		}*/

		return true;
	};

protected:
	uint gen_a;
};

} //end namespace utk


#endif
