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

#include <utk/utils/log.hpp>
#include <utk/utils/Pointset.hpp>
#include <cmath>

namespace utk
{

class SamplerKorobov
{
protected:
public:

	SamplerKorobov(uint32_t d, uint32_t gen = 3) : D(d), gen_a(gen) { }
	
	void setDimension(uint32_t d) { D = d; }
    uint32_t GetDimension() const { return D; }

	void setGeneratorA(uint32_t arg_a) { if (arg_a != 0) gen_a = arg_a; }

	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
		// @TODO : prevent user from generating points if gcd(N, a) != 1 ? 
		arg_pts.Resize(N, D);
		std::vector<T> W(D);		
		const T step = 1.0 / (T) N;
		
		if (D >= 1) W[0] = 1 * step;
		if (D >= 2) W[1] = gen_a * step;
		if (D >  2) 
		{
			for (uint32_t i = 2; i < D; i++)
				W[i] = (T)((int) std::pow(gen_a, i) % N) * step;
		}
		
		if (std::gcd(N, gen_a) != 1)
			UTK_WARN("Sampler Korobov: {} is not relatively prime with {}", N, gen_a);

		for(uint32_t i = 0; i < N; i++)
		{
			for (uint32_t d = 0; d < D; d++)
			{
				arg_pts[i][d] = std::fmod(i * W[d], 1.0);
			}
		}

		return true;
	};

protected:
	uint32_t D;
	uint32_t gen_a;
};

} //end namespace utk
