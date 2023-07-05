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

#include <utk/utils/utk_types.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/utils/log.hpp>

#include <cmath>

namespace utk
{
    template<typename T>
    std::pair<T, T> sincos(T angle)
    {
        // TODO : on some platform, a sincos function
        // is supported !
        return {std::sin(angle), std::cos(angle)};
    }

    class Spectrum
    {
    public:
        Spectrum(uint32_t w = 0, bool cd = true) : paramWidth(w), cancelDC(cd) {}

        void setResolution(uint32_t nw) { paramWidth = nw; }
        void setCancelDC(bool cdc) { cancelDC = cdc; }
        uint32_t getWidth(uint32_t hint_N = 0) const
        {
            uint32_t w = paramWidth;
            if (paramWidth == 0)
                w = static_cast<uint32_t>(2 * 5 * std::sqrt(hint_N));
            else if ((w & 1) == 0)
                UTK_WARN("Spectrum: only using odd resolution ({} => {})", w, w - !(w & 1));

            return w - !(w & 1);
        }

        template<typename T>
        std::vector<T> compute(const Pointset<T>& pts)
        {
            static constexpr T TWOPI_NEG = -6.2831853071795864769;

            const uint32_t width = getWidth(pts.Npts());
            const uint32_t Origin = width >> 1;
            const T invNPts = 1. / (pts.Npts());
            
            std::vector<std::vector<T>> ws;     
            std::vector<T> spectrum(std::pow(width, pts.Ndim()), 0.0);

            // Spectrum is computed by computing scalar products
            // Between a grid and every point of the set
            #pragma omp parallel 
            {
                // Pre-Allocate memory
                #ifdef UTK_USE_OPENMP
                    #pragma omp single
                    {
                        ws.resize(omp_get_num_threads(), std::vector<T>(pts.Ndim(), 0.0));
                    }
                #else
                    ws.resize(1, std::vector<T>(pts.Ndim(), 0.0));
                #endif

                #pragma omp for
                for (OPENMP_UINT i = 0; i < spectrum.size(); i++)
                {
                    #ifdef UTK_USE_OPENMP
                        uint32_t tId = omp_get_thread_num();
                    #else
                        uint32_t tId = 0;
                    #endif

                    uint32_t tmp = i;
                    // Coordinate of the grid point (in the frequency domain)
                    for (uint32_t d = 0; d < pts.Ndim(); d++)
                    {
                        ws[tId][d] = TWOPI_NEG * (static_cast<long long>(tmp % width) - Origin);
                        tmp /= width;
                    }

                    T ex = 0.0;
                    T ey = 0.0;
                    for (uint32_t j = 0; j < pts.Npts(); j++)
                    {
                        // Scalar product between 'j' and the grid point
                        T angle = 0.0;
                        for (uint32_t d = 0; d < pts.Ndim(); d++)
                            angle += ws[tId][d] * pts[j][d];

                        auto [s, c] = sincos<T>(angle);
                        ex += c;
                        ey += s;
                    }
                    spectrum[i] = (ex * ex + ey * ey) * invNPts;
                }
            }
            
            if (cancelDC)
            {
                uint32_t origin = 0;
                for (uint32_t d = 0; d < pts.Ndim(); d++)
                    origin = origin * width + Origin;
                spectrum[origin] = 0;
            }
            return spectrum;
        }
        
        // Specialized version here
        // Loop over pointsets is pushed in the code to save on memory !
        template<typename T>
        std::vector<T> compute(const std::vector<Pointset<T>>& ptss)
        {
            if (ptss.size() == 0) return std::vector<T>();

            static constexpr T TWOPI_NEG = -6.2831853071795864769;

            const uint32_t N = ptss[0].Npts();
            const uint32_t D = ptss[0].Ndim();
            const T invNPts = 1. / ((T)ptss.size() * N);

            const uint32_t width = getWidth(N);
            const uint32_t Origin = width >> 1;
            
            std::vector<std::vector<T>> ws;     
            std::vector<T> spectrum(std::pow(width, D), 0.0);

            // Spectrum is computed by computing scalar products
            // Between a grid and every point of the set
            #pragma omp parallel 
            {
                // Pre-Allocate memory
                #ifdef UTK_USE_OPENMP
                    #pragma omp single
                    {
                        ws.resize(omp_get_num_threads(), std::vector<T>(D, 0.0));
                    }
                #else
                    ws.resize(1, std::vector<T>(D, 0.0));
                #endif

                #pragma omp for
                for (OPENMP_UINT i = 0; i < spectrum.size(); i++)
                {
                    #ifdef UTK_USE_OPENMP
                        uint32_t tId = omp_get_thread_num();
                    #else
                        uint32_t tId = 0;
                    #endif

                    uint32_t tmp = i;
                    // Coordinate of the grid point (in the frequency domain)
                    for (uint32_t d = 0; d < D; d++)
                    {
                        ws[tId][d] = TWOPI_NEG * (static_cast<long long>(tmp % width) - Origin);
                        tmp /= width;
                    }

                    T P = 0.0;
                    for (uint32_t k = 0; k < ptss.size(); k++)
                    {
                        T ex = 0.0;
                        T ey = 0.0;
                        for (uint32_t j = 0; j < N; j++)
                        {
                            // Scalar product between 'j' and the grid point
                            T angle = 0.0;
                            for (uint32_t d = 0; d < D; d++)
                                angle += ws[tId][d] * ptss[k][j][d];

                            auto [s, c] = sincos<T>(angle);
                            ex += c;
                            ey += s;
                        }
                        P += ex * ex + ey * ey;
                    }
                    
                    spectrum[i] = P * invNPts;
                }
            }
            
            if (cancelDC)
            {
                uint32_t origin = 0;
                for (uint32_t d = 0; d < D; d++)
                    origin = origin * width + Origin;
                spectrum[origin] = 0;
            }
            return spectrum;
        }
    private:
        uint32_t paramWidth;
        bool cancelDC;
    };
};