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
#ifndef _UTK_SAMPLER_NROOKS_
#define _UTK_SAMPLER_NROOKS_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>

namespace utk
{

	class SamplerNRooks
	{
	public:

		SamplerNRooks() { setRandomSeedTime(); }

		void setRandomSeed(long unsigned int arg_seed) { m_mersenneTwister.seed(arg_seed); }
		void setRandomSeedTime() { m_mersenneTwister.seed(std::chrono::system_clock::now().time_since_epoch().count()); }

		template<unsigned int D, typename T, typename P>
		bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned long long int arg_points)
		{
			if(!arg_pts.empty())
			{
				WARNING("SamplerNRooks::generateSamples the pointset to fill is not empty, clearing it ...");
				arg_pts.clear();
			}

			if(!isFloatingType<T>())
			{
				ERROR("SamplerNRooks::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
				return false;
			}

			//load domain & toricity
			for(uint d = 0; d<D; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = 1;
			}
			arg_pts.toricity = 1;

			arg_pts.resize(arg_points);

			m_distribution = std::uniform_int_distribution<int>(0, arg_points - 1);
			double unit = (1.0 / arg_points);

			for(uint i = 0; i < arg_points; i++)
			{
				double pos = i * unit;
				for(uint d = 0; d < D; d++)
				{
					arg_pts[i].pos()[d] = pos;
				}
			}

			/// N-Rooks Swapping
			for(uint i = 0; i < arg_points-2; i++)
			{
				for(uint d = 0; d < D; d++)
				{
					int j = genRandomAB(i, arg_points-1);
					std::swap(arg_pts[i].pos()[d], arg_pts[j].pos()[d]);
				}
			}

			return true;
		};

	protected:
		std::mt19937 m_mersenneTwister;
		std::uniform_int_distribution<int> m_distribution;

		int genRandomAB(int A, int B)
		{
			return m_distribution(m_mersenneTwister);
		}

		double getRandom01()
		{
			return (double)m_mersenneTwister() / (double)m_mersenneTwister.max();
		}
	};

} //end namespace utk


#endif
