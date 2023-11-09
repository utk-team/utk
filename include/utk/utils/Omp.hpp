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

#include <algorithm>
#include <cstdint>
#include <limits>

#define IF_OPENMP(x) x

#ifdef _WIN32
    // As windows definitely won't update OpenMP...
    using OPENMP_UINT = long int;         // Does not support unsigned int indexing...
    using OPENMP_DOUBLE_DEFAULT = double; // Does not support long double reduction...

    // Does not support max neither...
    template<typename T>
    T omp_parallel_max(const T* data, uint32_t size)
    {
        T max_val = std::numeric_limits<T>::min();

        #pragma omp parallel
        {
            T max_private = max_val;

            #pragma omp for nowait
            for (OPENMP_UINT i = 0; i < size; i++)
            {
                max_private = std::max(max_private, data[i]);
            }

            #pragma omp critical 
            {
                max_val = std::max(max_val, max_private);
            }
        }

        return max_val;
    }
#else
    using OPENMP_UINT = uint32_t;
    using OPENMP_DOUBLE_DEFAULT = long double;

    template<typename T>
    T omp_parallel_max(const T* data, uint32_t size)
    {
        T maxval = std::numeric_limits<T>::min();

        #pragma omp parallel for reduction(max: maxval)
        for (OPENMP_UINT i = 0; i < size; i++)
            maxval = std::max(maxval, data[i]);
        
        return maxval;
    }
#endif
