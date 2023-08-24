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
#include <utk/utils/Omp.hpp>
#include <cmath>

namespace utk
{
    class StarDiscrepancy
    {
    public:
        StarDiscrepancy() {}

        template<typename T>
        T compute(const Pointset<T>& pts)
        {
            const uint32_t count = std::pow(pts.Npts() + 1, pts.Ndim());
            const double invN = 1. / pts.Npts();

            std::vector<std::vector<uint32_t>> sorted;
            for (uint32_t d = 0; d < pts.Ndim(); d++)
            {
                std::vector<uint32_t> v(pts.Npts());
                
                for (uint32_t k = 0; k < pts.Npts(); k++) v[k] = k;
                
                std::sort(v.begin(), v.end(), [&](uint32_t a, uint32_t b)
                {
                    return pts[a][d] < pts[b][d];
                });
            }

            // N^d loop
            std::vector<uint32_t>  coords(pts.Ndim(), 0);
            std::vector<T>         points(pts.Ndim(), 0);

            for (uint32_t i = 0; i < count - 1; i++)
            {
                T max = 0.;
                uint32_t nBoundary = 0;

                uint32_t idx = i;

                // Store min value of coordinates
                uint32_t minCoordIdx = pts.Ndim() + 1;
                uint32_t minCoordVal = pts.Npts() + 1;
            
                // Compute coordinates of grid point    
                for (uint32_t k = 0; k < pts.Ndim(); k++)
                {
                    coords[k] = (idx % (pts.Npts() + 1));
                    idx /= pts.Npts();

                    // Count number of 'real' point (excluding 1)
                    // that belongs to the boundary of the box. 
                    if (coords[k] != pts.Npts())
                        nBoundary ++;

                    // Upper right box coordinate. 
                    // Add point (1, 1, ...) for box to possibly touch the hypercube boundary
                    points[k] = (coords[k] == pts.Npts()) ? 1.0 : pts[sorted[coords[k]]][k];

                    if (coords[k] < minCoordVal)
                    {
                        minCoordVal = coords[k];
                        minCoordIdx = k;
                    }
                }

                // The d-uplet coords is admissible if it spans a box
                // that can be a 'maximum box' for the discrepancy.             
                std::vector<T> box(pts.Ndim(), 1.0);
    
                bool admissible = true;
                double boxSize  = 1.0;

                for (uint32_t d = 0; d < pts.Ndim(); d++)
                {
                    for (uint32_t k = 0; k < d; k++)
                        admissible = admissible && (points[k] <= box[k]);

                    box[d]   = points[k];
                    boxSize *= points[k];
                }

                if (!admissible) continue;

                // Check points. Using minCoordVal performs a quick filter
                // and helps reducing the number of checks by a lot ! 
                uint32_t ptCount = 0;              
                for (uint32_t k = 0; k < minCoordVal; k++)
                {
                    uint32_t ptIdx = sorted[minCoordIdx][k];
                    bool is_inside = true;
                    for (uint32_t d = 0; d < pts.Ndim(); d++)
                        is_inside = is_inside && (pts[ptIdx][d] < pts[sorted[d][coords[d]]][d]);

                    ptCount += is_inside; 
                }

                for (uint32_t k = 0; k < nBoundary; k++)
                {
                    uint32_t realPtCount = ptCount + k;
                    
                    double error = std::abs((realPtCount * invN) - boxSize);
                    max = (max < error) ? error : max;
                }
            }
        }

        template<typename T>
        std::vector<T> compute(const std::vector<Pointset<T>>& ptss)
        {
            std::vector<T> rslts;
            rslts.reserve(ptss.size());
            for (const auto& pts : ptss)
                rslts.push_back(compute(pts));
            return rslts;
        }
    private:
        void CountInBox()
        {

        }
    };
};