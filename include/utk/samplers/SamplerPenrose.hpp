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

#include "Penrose/quasisampler_prototype.h"
#include <utk/utils/Log.hpp>
#include <utk/utils/Pointset.hpp>
#include <random>

namespace utk
{
	
class MyQuasisampler : public penrose::Quasisampler
{
	unsigned const_val;

	public:
		MyQuasisampler(unsigned val) : penrose::Quasisampler(100,100), const_val(val) {}

		unsigned getImportanceAt( penrose::Point2D pt ) 
		{ 
			return const_val; // This function returns a constant value.
		}
};

class SamplerPenrose
{
protected:
public:

	SamplerPenrose(/*uint32_t mag = 100*/) : magFactor(10) { }
	
    uint32_t GetDimension() const { return 2; }
	// void setMagFactor(uint32_t mag) { magFactor = mag; }

	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
		MyQuasisampler test(magFactor * N);
		std::vector<penrose::Point2D> points = test.getSamplingPoints();

		if (N != points.size())
            UTK_WARN("Sampler Penrose, returning {} samples when asked for {}", points.size(), N);
       
        arg_pts.Resize(points.size(), 2);
		for (uint32_t i = 0; i < points.size(); i++)
		{
			arg_pts[i][0] = (points[i].x / 100.0);
			arg_pts[i][1] = (points[i].y / 100.0);
		}
        
		return true;
	};

protected:
	uint32_t magFactor;
};

}
