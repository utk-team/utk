/*
 * Mayur Patel  mayur.patel.rogers@gmail.com
 *
 * Copyright (c) 2021
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
 * either expressed or implied, of the Halton project.
 */
#pragma once

#include <utk/utils/Pointset.hpp>
#include <cmath>

namespace utk
{
    class Diaphony
    {
    public:
        Diaphony() {}

        template<typename T>
        T compute(const Pointset<T>& pts)
        {
            auto n = pts.Npts();
            auto D = pts.Ndim();

            T sum_over_i = 0;
            for(unsigned i=0; i<n; i++)
            {
                T sum_over_j = 0;
                for(unsigned j=0; j<n; j++)
                {
                    T product_over_k = 1;
                    for(unsigned k=0; k<D; k++)
                    {
                        const T uik = pts[i][k];
                        const T ujk = pts[j][k];

                        const T delta = uik - ujk;
                        const T mod1_delta = delta - std::floor(delta);

                        const T B2 = mod1_delta * mod1_delta - mod1_delta + (1.0/6.0); // second Bernoulli polynomial
                        product_over_k *= 1.0 + 2.0 * M_PI * M_PI * B2;
                    }
                    sum_over_j += product_over_k - 1.0;
                }
                sum_over_i += sum_over_j;
            }

            const T bot = T(n*n) * ( std::pow ( 1.0 + M_PI * M_PI / 3.0, D ) - 1.0 );
            return std::sqrt(sum_over_i / bot);
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
    };
};