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

#include <utk/utils/FastPRNG.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/utils/Log.hpp>
#include <random>
#include <cmath>

namespace utk
{

	class SamplerStratified
	{
	protected:
	public:

		SamplerStratified(uint32_t d) : D(d) { setRandomSeed(); }

        void setDimension(uint32_t d) { D = d; }
        uint32_t GetDimension() const { return D; }

        void setRandomSeed( uint64_t arg_seed ) { gen.seed(arg_seed); }
        void setRandomSeed() { gen.seed(std::random_device{}()); }

		template<typename T>
        bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
        {
            const uint32_t n  = (uint32_t) std::round(pow((double) N, 1. / (double) D));
            const double gridSize = 1. / (double)n;

            if (N != static_cast<uint32_t>(std::round(std::pow(n, D))))
            {            
                UTK_ERROR("Sampler Stratified requires to be a power of dimension {} != {} ** {}", N, n, D);
                return false;
            }
            
            std::uniform_real_distribution<T> dist(0, 1);
            arg_pts.Resize(N, D);

            for (uint32_t i = 0; i < N; i++)
            {
                uint32_t coords = i;
                for (uint32_t d = 0; d < D; d++)
                {
                    const uint32_t cellStart = static_cast<uint32_t>(coords % n);
                    arg_pts[i][d] = (dist(gen) + cellStart) * gridSize;           

                    coords /= n;         
                }
            }

            return true;
        }

    protected:
        uint32_t D;
        utk::PCG32 gen;
	};

} 