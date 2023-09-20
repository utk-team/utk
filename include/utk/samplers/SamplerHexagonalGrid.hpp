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

#include <utk/utils/Log.hpp>
#include <utk/utils/Pointset.hpp>
#include <cmath>


namespace utk
{

class SamplerHexagonalGrid
{
public:

    SamplerHexagonalGrid() { }
    
	uint32_t GetDimension() const { return 2; }

    template<typename T>
    bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
    {
		const uint32_t n = (uint32_t) std::round(std::sqrt(N));
        
        if (n * n != N)
        {
            UTK_ERROR("Sampler HexagonalGrid requires squared number of point {} != {}", N, n * n);
            return false;
        }
        arg_pts.Resize(N, 2);
        const double s = std::sqrt(2.0 / (3.0 * N * sqrt(3.0)));
        
        const double grid_size_x = std::sqrt(3) * s;
        const double grid_size_y = (3.0 / 2.0) * s;

        for (uint32_t i = 0; i < N; i++)
        {
            const uint32_t coords = i;
            const uint32_t xi = (coords % n);
            const uint32_t yi = (coords / n);

            arg_pts[i][0] = (xi + (yi % 2) / 2.0) * grid_size_x;
            arg_pts[i][1] = yi * grid_size_y;
        }
        return true;
    }
};

}