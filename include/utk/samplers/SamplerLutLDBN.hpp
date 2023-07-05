/*
 * Coded by Hélène Perrier helene.perrier@liris.cnrs.fr
 * and Bastien Doignies bastien.doignies@liris.cnrs.fr 
 * and David Coeurjolly David.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2023 CNRS Université de Lyon
 * All rights reserved.
 * 
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
#pragma once

#include "LutLDBN/lutLDBN.h"
#include <utk/utils/log.hpp>
#include <utk/utils/Pointset.hpp>
#include <random>
#include <cmath>

namespace utk
{

class SamplerLutLDBN
{
protected:
public:
	static std::vector<std::string> GetTargets()
	{
		return { "BNOT", "STEP" };
	}

	SamplerLutLDBN() : target("BNOT") { initSamplers(); }

    uint32_t GetDimension() const { return 2; }

	void setTarget(const std::string& tgt) 
	{
		target = tgt;
	}

	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
		const uint32_t n  = (uint32_t) std::round(std::sqrt(N));
		if (n * n != N)
		{
            UTK_ERROR("Sampler LDBN requires squared number of point {} != {}", N, n * n);
			return false;
		}
			
		std::vector<Point> samples;
		if (target == "STEP")
		{
			ldbnSTEP(N, samples);
		}
		else if (target == "BNOT") 
		{
			ldbnBNOT(N, samples);
		}
		else
		{
			UTK_WARN("Sampler LutLDBN unknown target {}, falling back to BNOT", target);
			ldbnBNOT(N, samples);
		}
		
		arg_pts.Resize(N, 2);
		uint32_t pmax = std::ceil(std::log2(N));
		uint32_t smax = std::pow(2, pmax);

		if (std::is_integral_v<T>)
		{
			for (uint32_t i = 0; i < N; i++)
			{
				arg_pts[i][0] = static_cast<T>(samples[i][0] * smax);
				arg_pts[i][1] = static_cast<T>(samples[i][1] * smax);
			}
		}
		else
		{
			for (uint32_t i = 0; i < N; i++)
			{
				arg_pts[i][0] = static_cast<T>(samples[i][0]);
				arg_pts[i][1] = static_cast<T>(samples[i][1]);
			}	
		}
				
		return true;
	};

protected:
	std::string target;
};

}
