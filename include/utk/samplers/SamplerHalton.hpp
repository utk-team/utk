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

#include <utk/utils/Pointset.hpp>
#include <utk/utils/log.hpp>
#include <utk/utils/RadicalInversion.hpp>
#include <cmath>


namespace utk
{

class SamplerHalton
{
protected:
public:
	static constexpr uint32_t PRIMES[100] = {
	 	  2,   3,   5,   7,  11,  13,  17,  19,  23,  29, 
		 31,  37,  41,  43,  47,  53,  59,  61,  67,  71, 
	  	 73,  79,  83,  89,  97, 101, 103, 107, 109, 113, 
		127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 
		179, 181, 191, 193, 197, 199, 211, 223, 227, 229,
		233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 
		283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 
		353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 
		419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 
		467, 479, 487, 491, 499, 503, 509, 521, 523, 541
	};

	SamplerHalton(uint32_t d) : D(d) 
    {
		setBasisList(PRIMES, sizeof(PRIMES) / sizeof(uint32_t));
	}
    
	void setDimension(uint32_t d) { D = d; }
    uint32_t GetDimension() const { return D; }

	void setBasisList(const uint32_t* arg_base_list, uint32_t arg_idBaseMax )
	{
		m_base_list = arg_base_list;
		m_idBaseMax = arg_idBaseMax;
	}

	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
		if(D >= m_idBaseMax) 
		{
			UTK_ERROR("Sampler Halton: dimension {} exeed basis list size {}", D, m_idBaseMax);
			return false;
		}
		
		arg_pts.Resize(N, D);
		if constexpr (std::is_integral_v<T>)
		{
			for(uint32_t i=0; i < N; i++)
			{
				for(uint32_t d = 0; d < D; d++)
				{
					arg_pts[i][d] = RadicalInverseInt(i, m_base_list[d]);
				}
			}
		}
		else // Assume double or float here
		{
			for(uint32_t i = 0; i < N; i++)
			{
				for(uint32_t d = 0; d < D; d++)
				{
					arg_pts[i][d] = RadicalInverseDouble<T>(i, m_base_list[d]);
				}
			}
		}

		return true;
	}

private:
    uint32_t D;
	uint32_t m_idBaseMax;
	const uint32_t* m_base_list;
};

} //end namespace utk
