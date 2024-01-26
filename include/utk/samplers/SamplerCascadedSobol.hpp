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

#include <utk/samplers/SamplerSobol.hpp>
#include <random>
#include <tuple> // std::ignore
#include <cmath>
#include <map>

namespace utk
{

// Inherit to save code. But functions are not virtual, so it can't be used 
// with pointer to SamplerSobol !
template<typename IntegerType>
class SamplerCascadedSobol : public SamplerSobol<IntegerType>
{
protected:
public:
    SamplerCascadedSobol(
            uint32_t d,
            unsigned char owenDepth = 32
    ) : SamplerSobol<IntegerType>(d, owenDepth, std::string(UTK_DATA_PATH) + "/Sobol/cascaded_sobol_init_tab.dat")
    { }

    template <typename T>
    bool generateSamples(Pointset<T> &arg_pts, IntegerType N)
    {
        this->sobol_mk.resize(this->D);
        if (!this->load_mk(this->directionFile))
        {
            UTK_ERROR("SamplerCascadedSobol : no such file or directory {}", this->directionFile);
            return false;
        }
        
        IntegerType nbits = (IntegerType) ((double) log((double) N) / (double) log((double) 2));

        std::vector<uint64_t> seeds(this->D);
        for (uint32_t d = 0; d < this->D; d++)
            seeds[d] = this->gen();

        arg_pts.Resize(N, this->D);
        for (IntegerType i = 0; i < N; i++)
        { 
            IntegerType index = RadicalInverseBase2(i);
            arg_pts[i][0] = convertFullRadicalInverseBase2<T>(this->owen(index, 0, this->owenDepth, seeds.data()));
            
            for (uint32_t d = 1; d < this->D; d++)
            {
                index = (index >> (SamplerSobol<IntegerType>::MAX_BITS - nbits));
                index = this->sobol_binary(index, this->sobol_mk[d].data());
                arg_pts[i][d] = convertFullRadicalInverseBase2<T>(this->owen(index, d, this->owenDepth, seeds.data()));
            }
        }

        return true;
    };
};

}
