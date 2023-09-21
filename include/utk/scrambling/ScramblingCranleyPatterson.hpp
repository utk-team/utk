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
#include <cmath>

namespace utk
{
    class ScramblingCranleyPatterson
    {
    public:
        ScramblingCranleyPatterson(double md = 1.0, double ds = 1.0) :
            domainSize(ds), maxDispacement(md)
        { }

        void setMaxDispacement(double mv = 1.0)
        {
            maxDispacement = mv;
        }

        void setDomainSize(double ds = 1.0)
        {
            domainSize = ds;
        }

        void setRandomSeed(uint64_t arg_seed)
        {
            mt.seed(arg_seed);
        }

        void setRandomSeed()
        {
            setRandomSeed(std::random_device{}());
        }

        template<typename T>
        bool Scramble(Pointset<T>& in)
        {
            std::uniform_real_distribution<T> dist(-maxDispacement, maxDispacement);
            std::vector<T> shift(in.Ndim());

            for (uint32_t d = 0; d < in.Ndim(); d++)
                shift[d] = dist(mt);

            for (uint32_t i = 0; i < in.Npts(); i++)
            {
                for (uint32_t d = 0; d < in.Ndim(); d++)
                {
                    in[i][d] = std::fmod(in[i][d] + shift[d], domainSize);
                }
            }
            return true;
        }

        template<typename T>
        bool Scramble(const Pointset<T>& in, Pointset<T>& out)
        {
            std::uniform_real_distribution<T> dist(-maxDispacement, maxDispacement);
            std::vector<T> shift(in.Ndim());

            out.Resize(in.Npts(), in.Ndim());

            for (uint32_t d = 0; d < in.Ndim(); d++)
                shift[d] = dist(mt);

            for (uint32_t i = 0; i < in.Npts(); i++)
            {
                for (uint32_t d = 0; d < in.Ndim(); d++)
                {
                    out[i][d] = std::fmod(in[i][d] + shift[d], domainSize);
                }
            }
            return true;
        }

        template<typename T>
        bool Scramble(std::vector<Pointset<T>>& in)
        {
            bool result = true;
            for (auto& i : in)
                result = result && Scramble(i);
            return result;
        }


        template<typename T, typename D>
        bool Scramble(const std::vector<Pointset<T>>& in, std::vector<Pointset<D>>& out)
        {
            bool result = true;
            out.resize(in.size());

            for (std::size_t i = 0; i < in.size(); i++)
            {
                out[i].Resize(in[i].Npts(), in[i].Ndim());
                result = result && Scramble(in[i], out[i]);
            }
            return result;
        }

    private:
        double domainSize;
        double maxDispacement;
        utk::PCG32 mt;
    };
}
