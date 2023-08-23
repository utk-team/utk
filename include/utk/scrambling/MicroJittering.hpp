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
#include <utk/utils/FastPRNG.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/utils/FastPRNG.hpp>
#include <random>

namespace utk
{
    class MicroJittering
    {
    public:
        MicroJittering(double ip = -1.0) : intensityParam(ip)
        { }

        void setRandomSeed(uint64_t arg_seed) 
        { 
            mt.seed(arg_seed);
        }

        void setRandomSeed() 
        { 
            setRandomSeed(std::random_device{}());
        }

        template<typename T>
        T getIntensity(uint32_t hint_N, uint32_t hint_D) const
        {
            if (intensityParam < 0.0)
                return std::pow(std::max(hint_N, (uint32_t)1), -(T)hint_D);
            return intensityParam;
        }

        template<typename T>
        void Scramble(Pointset<T>& in)
        {
            const T intensiy = getIntensity<T>(in.Npts(), in.Ndim());
            std::uniform_real_distribution<T> dist(-intensiy, intensiy);

            for (uint32_t i = 0; i < in.Npts(); i++)
            {
                for (uint32_t d = 0; d < in.Ndim(); d++)
                {
                    in[i][d] = std::clamp(in[i][d] + dist(mt), (T)0.0, (T)1.0);
                }
            }
        }

        template<typename T, typename D>
        void Scramble(const Pointset<T>& in, Pointset<D>& out)
        {
            out.Resize(in.Npts(), in.Ndim());

            const T intensiy = getIntensity<T>(in.Npts(), in.Ndim());
            std::uniform_real_distribution<T> dist(-intensiy, intensiy);
            
            for (uint32_t i = 0; i < in.Npts(); i++)
            {
                for (uint32_t d = 0; d < in.Ndim(); d++)
                {
                    out[i][d] = std::clamp(in[i][d] + dist(mt), (T)0.0, (T)1.0);
                }
            }

        }
    private:
        double intensityParam;
        utk::PCG32 mt;
    };
}