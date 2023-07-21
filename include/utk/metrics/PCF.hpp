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

#include <utk/utils/Pointset.hpp>
#include <utk/utils/utk_types.hpp>
#include <cmath>

namespace utk
{
    class PCF
    {
    public:
        PCF(
            bool to = true, 
            double rmi = 0.01, 
            double rma = 0.15, 
            uint32_t nb = 200, 
            double sm = 0.001
        ) :
            toroidal(to),
            rmin(rmi), rmax(rma), smoothing(sm), nbbins(nb)
        {}

        void setToroidal(bool to) { toroidal = to; }
        void setRmin(double r) { rmin = r; }
        void setRmax(double r) { rmax = r; }
        void setSmoothing(double s) { smoothing = s; }
        void setNbBins(uint32_t b) { nbbins = b; }

        template<typename T>
        std::vector<T> compute(const Pointset<T>& pts)
        {
            static constexpr T invPI = 0.3183098861837907;
            static constexpr T    PI = 3.1415926535897932;
            uint32_t N = pts.Npts();

            T ra = rmin;
            T rb = rmax;

            auto dist = toroidal ? &PCF::toricDistance<T> :
                                   &PCF::distance<T>;

            std::vector<T> pcf(nbbins, 0.0);

            #pragma omp parallel for
            for (OPENMP_UINT pcfid = 0; pcfid < nbbins; ++pcfid)
            {
                T r = ra + pcfid * (rb - ra) / (T)nbbins;
                T estimator = 0.0;

                for (uint32_t i = 0; i < N; i++)
                {
                    for (uint32_t j = i + 1; j < N; j++)
                    {
                        T val = gaussianKernel(smoothing, r - dist(pts.Ndim(), pts[i], pts[j]));
                        estimator += val;
                        estimator += val;
                    }
                }

                T cov = 1.0 - 2 * (2 * r * invPI) + r * r * invPI;
                T factor = 2 * PI * r * cov;
                factor = 1 / factor;

                estimator *= factor;
                estimator /= (N * (N - 1));
                pcf[pcfid] = estimator;
            }

            return pcf;
        }

        template<typename T>
        std::vector<T> compute(const std::vector<Pointset<T>>& ptss)
        {
            if (ptss.size() == 0) return std::vector<T>();

            const uint32_t N = ptss[0].Npts();
            const uint32_t D = ptss[0].Ndim();
            const T invNPts = 1. / (T)ptss.size();
            
            static constexpr T invPI = 0.3183098861837907;
            static constexpr T    PI = 3.1415926535897932;

            T ra = rmin;
            T rb = rmax;

            auto dist = toroidal ? &PCF::toricDistance<T> :
                                   &PCF::distance<T>;

            std::vector<T> pcf(nbbins, 0.0);

            #pragma omp parallel for
            for (OPENMP_UINT pcfid = 0; pcfid < nbbins; ++pcfid)
            {
                T r = ra + pcfid * (rb - ra) / (T)nbbins;
                pcf[pcfid] = 0.0;

                for (uint32_t k = 0; k < ptss.size(); k++)
                {
                    T estimator = 0.0;
                
                    for (uint32_t i = 0; i < N; i++)
                    {
                        for (uint32_t j = i + 1; j < N; j++)
                        {
                            T val = gaussianKernel(smoothing, r - dist(D, ptss[k][i], ptss[k][j]));
                            estimator += val;
                            estimator += val;
                        }
                    }

                    T cov = 1.0 - 2 * (2 * r * invPI) + r * r * invPI;
                    T factor = 2 * PI * r * cov;
                    factor = 1 / factor;

                    estimator *= factor;
                    estimator /= (N * (N - 1));
                    pcf[pcfid] += estimator;
                }
                pcf[pcfid] *= invNPts;
            }

            return pcf;
        }
    private:
        bool toroidal;

        double rmin;
        double rmax;
        double smoothing;
        uint32_t nbbins;

        template <typename T>
        static T toricDistance(uint32_t D, const T* a, const T* b)
        {
            T dist = 0;
            for(uint32_t d = 0; d < D; d++)
            {
                T tmp = std::abs(a[d] - b[d]);
                T diff = std::min(tmp, 1- tmp);
                dist += diff * diff;
            }
            return std::sqrt(dist);
        }

        template <typename T>
        static T distance(uint32_t D, const T* a, const T* b)
        {
            T dist = 0;
            for(uint32_t d = 0; d < D; d++)
            {
                T diff = a[d] - b[d];
                dist += diff * diff;
            }
            return std::sqrt(dist);
        }
        
        template<typename T>
        static T gaussianKernel(T sigma, T x)
        {
            static constexpr T NORM = 0.3989422804014327; // 1 / sqrt(2 * PI) rounded
            return NORM * std::exp( - 0.5 * x * x / (sigma * sigma)) / sigma;
        }
        
        //Box Kernel 1D
        // template<typename T>
        // static T boxKernel(T h, T x)
        // {
        //     if(x >= -h && x < h)
        //         return 1.0/2.0*h;
        //     return 0;
        // }
    };
};