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
#include <cmath>
#include "Spectrum.hpp"

namespace utk
{
    class RadialSpectrum
    {
    public:
        RadialSpectrum(uint32_t nb = 0, double s = 0.5, uint32_t w = 0, bool cd = true) : 
            scale(s), paramNBins(nb), spectrum(w, cd) 
        { }

        void setResolution(uint32_t nw) { spectrum.setResolution(nw); }
        void setCancelDC(bool cdc)      { spectrum.setCancelDC(cdc); }
        void setScale(double s)         { scale = s; }
        void setBins(uint32_t b)        { paramNBins = b; }

        uint32_t getNBBins(uint32_t hintN = 0) const
        {
            if (paramNBins == 0)
            {
                auto width = spectrum.getWidth(hintN);
                // Magic formula, just found it good, but could be changed
                return static_cast<uint32_t>(std::sqrt(width * (width - 1))) / 2;
            }
            return paramNBins;
        }

        template<typename T>
        std::vector<T> fromSpectrum(const std::vector<T>& spectra, uint32_t N, uint32_t D)
        {
            const uint32_t width  = spectrum.getWidth(N);
            const uint32_t Origin = width >> 1;
            const uint32_t totalIts = std::pow(width, D);
            const uint32_t nbins = getNBBins(N);

            std::vector<T> rp(nbins, 0.0);
            std::vector<uint32_t> hits(nbins, 0);

            for (uint32_t i = 0; i < totalIts; i++)
            {
                T radius = 0.0;
                uint32_t tmp = i;
                for (uint32_t d = 0; d < D; d++)
                {
                    long long int coord = (static_cast<long long>(tmp % width) - Origin);
                    tmp /= width;
                    radius += coord * coord;
                }

                uint32_t binId = scale * std::sqrt(radius);
                if (binId < nbins)
                {
                    rp[binId] += spectra[i];
                    hits[binId] ++;
                }
            }

            // Normalize
            for (uint32_t i = 0; i < nbins; i++)
            {
                if (hits[i]) rp[i] /= hits[i];
            }

            return rp;
        }

        template<typename T>
        std::vector<T> compute(const Pointset<T>& pts)
        {
            return fromSpectrum(spectrum.compute(pts), pts.Npts(), pts.Ndim());
        }

        template<typename T>
        std::vector<T> compute(const std::vector<Pointset<T>>& ptss)
        {
            if (ptss.size() == 0) return std::vector<T>();
            
            const uint32_t N = ptss[0].Npts();
            const uint32_t D = ptss[0].Ndim();

            return fromSpectrum(spectrum.compute(ptss), N, D);
        }
    private:
        double scale;
        uint32_t paramNBins;
        Spectrum spectrum;
    };
};