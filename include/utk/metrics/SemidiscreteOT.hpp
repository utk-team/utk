/*
 * Coded by Nicolas Bonneel nicolas.bonneel@liris.cnrs.fr 
 *  and Bastien Doignies bastien.doignies@liris.cnrs.fr 
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


#include <limits>
#include <utk/utils/Pointset.hpp>
#include <utk/utils/Omp.hpp>
#include <utk/utils/Log.hpp>
#include <cmath>

#include "transport.h"

namespace utk
{
    class SemidiscreteOT
    {
    public:
        SemidiscreteOT()
        {}

        void setMaxNewtonIter(uint32_t it) { maxNewtonIters = it; }
        void setWorstAreaRelativeThreshold(double ar) { worstAreaRelativeThresholdPercent = ar; }

        template<typename T>
        T compute(const Pointset<T>& pts)
        {
            if (pts.Ndim() != 2)
            {
                UTK_ERROR("Only 2D pointsets are supported. Found {}-d pointsets", pts.Ndim());
                return static_cast<T>(-1.0);
            }
            
            transport::OptimalTransport2D ot;

            ot.V.vertices.resize(pts.Npts());
            for (unsigned int i = 0; i < pts.Npts(); i++)
                ot.V.vertices[i] = transport::Vector(pts[i][0], pts[i][1]);
            
            return static_cast<T>(ot.optimize(maxNewtonIters, worstAreaRelativeThresholdPercent));
        }

        template<typename T>
        std::vector<T> compute(const std::vector<Pointset<T>>& ptss)
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
        uint32_t maxNewtonIters = 100;                  // Default value of original code !
        double worstAreaRelativeThresholdPercent = 0.5; // Default value of original code !
    };
};