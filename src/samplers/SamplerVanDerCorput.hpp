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
#ifndef _UTK_SAMPLER_VAN_DER_CORPUT_
#define _UTK_SAMPLER_VAN_DER_CORPUT_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>
#include <sstream>

namespace utk
{

	class SamplerVanDerCorput
	{
	protected:

	public:

		SamplerVanDerCorput() { }
		
		template<unsigned int D, typename T, typename P>
		P generateIthPoint(unsigned int i)
		{
			P pt;
			for(int d=0; d<D; d++)
			{
				pt.pos()[d] = radicalInverse_VdC(i);
			}
			return pt;
		}

		template<unsigned int D, typename T, typename P>
		bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
		{
			if (!arg_pts.empty())
			{
				WARNING("SamplerVanDerCorput::generateSamples the pointset to fill is not empty, clearing it ...");
				arg_pts.clear();
			}
		
			arg_pts.resize(arg_points);
			arg_pts.toricity = 1;
			
			if(isIntegerType<T>())
			{
				int smax = ceil(log2(arg_points));
				smax = pow(2, smax);
				
				for(int d=0; d<D; d++){
					arg_pts.domain.pMin.pos()[d] = 0;
					arg_pts.domain.pMax.pos()[d] = smax;
				}
				
				for(int i=0; i<arg_points; i++)
				{
					Point<D, double> pt = generateIthPoint< D, double, Point<D, double> >(i);
					for(int d=0; d<D; d++)
						arg_pts[i].pos()[d] = pt.pos()[d]*smax;
				}
			}
			else if (isFloatingType<T>())
			{
				for(int d=0; d<D; d++){
					arg_pts.domain.pMin.pos()[d] = 0;
					arg_pts.domain.pMax.pos()[d] = 1;
				}
				
				for(int i=0; i<arg_points; i++)
				{
					arg_pts[i] = generateIthPoint<D, T, P>(i);
				}
			}
			else
			{
				ERROR("SamplerVanDerCorput::generateSamples The samples coordinates must be of numeric value");
				return false;
			}

			return true;
		};

	protected:
		
		//Code from http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
	static float radicalInverse_VdC(uint bits) {
		bits = (bits << 16u) | (bits >> 16u);
		bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
		bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
		bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
		bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
		return float(bits) * 2.3283064365386963e-10; // / 0x100000000
	}

	};

} //end namespace utk


#endif
