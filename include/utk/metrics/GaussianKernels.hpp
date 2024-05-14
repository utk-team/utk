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

#ifdef UTK_USE_OPENMP
    #include <omp.h>
#endif

#include <limits>
#include <utk/utils/Pointset.hpp>
#include <utk/utils/Omp.hpp>
#include <cmath>

namespace utk
{
    class GaussianKernels
    {
    public:
        GaussianKernels(bool to = false, double _sigma = 1.0) :
            toroidal(to), sigma(_sigma)
        {}

        void setToroidal(bool to) { toroidal = to; }
        void setSigma(double _sigma) { sigma = _sigma; }
        
        template<typename T>
        T compute(const Pointset<T>& pts) const
        {
            const auto dist = toroidal ? &GaussianKernels::toricDistanceSquared<T> :
                                         &GaussianKernels::distanceSquared<T>;


            const double PI = 3.141592653589793238462;
            const unsigned int N  = pts.Npts();
            const unsigned int D  = pts.Ndim();
            const double scaledSigma = sigma * std::pow(N, 1.0 / D);
            const double invSigma = - 1.0 / (2.0 * scaledSigma * scaledSigma); 
            const double norm     = PI * scaledSigma * scaledSigma / (2.0 * N);
            const double invN     = 1.0 / N;
            
            double value = 0.;

            #pragma omp parallel for reduction(+: value)
            for (OPENMP_UINT k = 0; k < N; k++)
            {
                for (OPENMP_UINT l = 0; l < N; l++)
                {
                    const double weight = std::exp(invSigma * dist(pts.Ndim(), pts[k], pts[l]));
                    value += weight * norm * invN;
                }   
            }
            
            return value;
        }

        template<typename T>
        std::vector<T> compute(const std::vector<Pointset<T>>& ptss) const
        {
            if (ptss.size() == 0) return std::vector<T>();
            
            std::vector<T> results(ptss.size());
            for (uint32_t i = 0; i < results.size(); i++)
            {
                results[i] = compute(ptss[i]);
            }

            return results;
        }

    private:
        bool toroidal;
        double sigma;

        template <typename T>
        static T toricDistanceSquared(uint32_t D, const T* a, const T* b)
        {
            T dist = 0;
            for(uint32_t d = 0; d < D; d++)
            {
                T tmp = std::abs(a[d] - b[d]);
                T diff = std::min(tmp, 1 - tmp);
                dist += diff * diff;
            }
            return dist;
        }

        template <typename T>
        static T distanceSquared(uint32_t D, const T* a, const T* b)
        {
            T dist = 0;
            for(uint32_t d = 0; d < D; d++)
            {
                T diff = a[d] - b[d];
                dist += diff * diff;
            }
            return dist;
        }
    };
};