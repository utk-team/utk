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
    /*
     * Helper class to generate partition of N with (at most) k integers
     */
    struct PartitionGenerator
    {
        PartitionGenerator(uint32_t n, uint8_t k) : N(n), k(k) 
        {
            vec = std::vector<uint32_t>(k, 0.0);
            vec[0] = N;

            state = vec;
        }

        bool GetNext(std::vector<std::uint32_t>& out) 
        {
            // Small trick so that both bounds are included in the result
            out = state;
            if (state.size() == 0)   return false;
            if (state.back() == N) { state.clear(); return true; } 

            // Compute next partition
            std::uint32_t sum = N;
            for (std::uint8_t i = 0; i < k - 1; i++)
            {
                sum -= state[i];
                // If state[i] reached it's maximum value
                if (state[i] == N - sum && state[i] != 0)
                {      
                    // Things to do :           
                    // 1. Increment the right one 
                    // 2. Zero out everything behind
                    // 3. Set the first to it's maximum value. 
                    std::uint32_t tmp = state[i + 1] + 1;
                    for (std::uint8_t j = 0; j < i + 1; j++)
                        state[j] = 0;

                    state[0]     = N - sum - 1;
                    state[i + 1] = tmp; 
                }
            }

            return true;
        }
        
        // Parition N into k numbers 
        std::uint32_t N;
        std::uint8_t  k;
        std::vector<uint32_t> state;
        std::vector<uint32_t> vec;
    };

    class TValue
    {
    public:
        TValue(std::uint8_t b = 2) : basis(b) {}

        void SetBasis(std::uint8_t b) { basis = b; }

        template<typename T>
        std::uint32_t compute(const Pointset<T>& pts)
        {
            std::uint16_t m = static_cast<std::uint16_t>(std::round(std::log(pts.Npts()) / std::log(basis)));

            std::uint32_t tvalue = m; // Inpossible value !!!

            // Let's assume all points are inside the box !
            // Reverse order because if pointset passes smaller boxes then it 
            // will pass bigger ones. 
            for (std::uint16_t t = m; t > 0; t--)
            {   
                PartitionGenerator generator(t, pts.Ndim());

                std::vector<std::uint32_t> parts(pts.Ndim(), 0);
                std::vector<std::uint32_t> coord(pts.Ndim(), 0);
                std::vector<T> invParts(pts.Ndim(), 0);
                
                // Good if partition have the correct number of points
                const std::uint32_t expected = std::pow(basis, m - t);
                bool good = true;
                // Loop through all intervall sizes
                while(generator.GetNext(parts) && good)
                {
                    std::uint32_t box_count = 1.;
                    for (std::uint32_t i = 0; i < pts.Ndim(); i++)
                    {
                        std::uint32_t bpower = std::pow(basis, parts[i]);
                        box_count *= bpower;
                        
                        if constexpr (std::is_integral_v<T>) // Integer dyadic interval base^(m - q_i)
                            invParts[i] = static_cast<T>(std::round(std::pow(basis, m - parts[i])));
                        else                                 // Double dyadic interval : base^(-q_i)
                            invParts[i] = 1. / static_cast<T>(bpower);
                    }   


                    for (std::uint32_t k = 0; k < box_count; k++)
                    {
                        // Get b-adic intervall coordinates
                        GetCoords(k, parts, coord);
                        
                        // Count the number of point inside the box 
                        std::uint32_t count = 0;
                        #pragma omp parallel for reduction(+: count)
                        for (OPENMP_UINT i = 0; i < pts.Npts(); i++)
                        {
                            bool in_box = true;
                            for (std::uint32_t d = 0; d < pts.Ndim(); d++)
                                in_box = in_box && CheckInside(pts[i][d], coord[d], invParts[d]);
                            count += in_box;
                        }

                        if (count != expected)
                        {
                            good = false;
                            break; // No need to check other partitions or boxes
                        }
                    }
                }

                // If all boxes are good at level = t then we are done
                if (good)
                {
                    tvalue = m - t; 
                    break;
                }
            }

            return tvalue;
        }

        template<typename T>
        std::vector<std::uint32_t> compute(const std::vector<Pointset<T>>& ptss)
        {
            std::vector<std::uint32_t> rslts;
            rslts.reserve(ptss.size());
            for (const auto& pts : ptss)
                rslts.push_back(compute(pts));
            return rslts;
        }

    private:
        template<typename T>
        static inline bool CheckInside(T value, std::uint32_t elem, const T& right)
        {
            // Includes left element, but not right !
            return (value >= (elem * right)) && (value < ((elem + 1) * right));
        }

        static inline void GetCoords(std::uint32_t idx, const std::vector<std::uint32_t>& grid, std::vector<std::uint32_t>& out)
        {
            for (std::uint32_t k = 0; k < grid.size(); k++)
            {
                if (grid[k] == 0) 
                {
                    out[k] = 0;
                }
                else
                {
                    out[k] = idx % grid[k];
                    idx /= grid[k];
                }
            }
        }

        std::uint8_t basis;
    };
};
