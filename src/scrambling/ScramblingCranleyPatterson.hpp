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
#ifndef UTK_SCRAMBLER_CP_HPP_
#define UTK_SCRAMBLER_CP_HPP_

#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>


namespace utk
{

class ScramblingCranleyPatterson
{
public:
	ScramblingCranleyPatterson() { setRandomSeedTime(); }
	void setRandomSeed( long unsigned int arg_seed ) { m_mersenneTwister.seed(arg_seed); }
	void setRandomSeedTime() { m_mersenneTwister.seed(std::chrono::system_clock::now().time_since_epoch().count()); }
	
	template<unsigned int D, typename T, typename P>
	bool scramble(const Pointset< D, T, P >& pts_input, Pointset< D, T, P >& pts_output)
	{
		if(pts_input.toricity!=1)
			WARNING("ScramblingCranleyPatterson:compute applying on a non-toric point set");
		
		Vector<D, double> randvector;
		for(uint d=0; d<D; d++)
			randvector[d] = getRandom01()*(pts_input.domain.pMax.pos()[d]-pts_input.domain.pMin.pos()[d]);
		
		pts_output.resize(pts_input.size());
		for(uint d=0; d<D; d++)
		{
			pts_output.domain.pMin.pos()[d] = pts_input.domain.pMin.pos()[d];
			pts_output.domain.pMax.pos()[d] = pts_input.domain.pMax.pos()[d];
		}
		
		for(uint i=0; i<pts_input.size(); i++)
		{
			for(uint d=0; d<D; d++)
			{
				pts_output[i].pos()[d] = pts_input[i].pos()[d] + randvector[d];
				if(pts_output[i].pos()[d] > pts_output.domain.pMax.pos()[d])
					pts_output[i].pos()[d] -= (pts_output.domain.pMax.pos()[d]-pts_output.domain.pMin.pos()[d]);
			}
		}
		
		return true;
	}
	
	template<unsigned int D, typename T, typename P>
	bool scramble(const Pointset< D, T, P >& pts_input, Pointset< D, T, P >& pts_output, utk::Vector<D, T> randvector)
	{
		if(pts_input.toricity!=1)
			WARNING("ScramblingCranleyPatterson:compute applying on a non-toric point set");

		pts_output.resize(pts_input.size());
		for(uint d=0; d<D; d++)
		{
			pts_output.domain.pMin.pos()[d] = pts_input.domain.pMin.pos()[d];
			pts_output.domain.pMax.pos()[d] = pts_input.domain.pMax.pos()[d];
		}
		
		for(uint i=0; i<pts_input.size(); i++)
		{
			for(uint d=0; d<D; d++)
			{
				pts_output[i].pos()[d] = pts_input[i].pos()[d] + randvector[d];
				if(pts_output[i].pos()[d] > pts_output.domain.pMax.pos()[d])
					pts_output[i].pos()[d] -= (pts_output.domain.pMax.pos()[d]-pts_output.domain.pMin.pos()[d]);
			}
		}
		
		return true;
	}
	
	
protected:
	std::mt19937 m_mersenneTwister;
    double getRandom01()
    {
        return (double)m_mersenneTwister()/(double)m_mersenneTwister.max();
    }

};
	
}

#endif