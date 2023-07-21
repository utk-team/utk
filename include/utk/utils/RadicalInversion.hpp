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

#include <cmath>

#ifndef __CHAR_BIT__
    #define __CHAR_BIT__ 8
#endif

namespace utk
{    
    template<typename IntegerType = uint32_t>
    inline IntegerType RadicalInverseBase2(IntegerType n)
    {
        // uint32_t
        if constexpr (sizeof(IntegerType) <= 4)
        {
            n = (n << 16u) | (n >> 16u);
            n = ((n & 0x55555555u) << 1u) | ((n & 0xAAAAAAAAu) >> 1u);
            n = ((n & 0x33333333u) << 2u) | ((n & 0xCCCCCCCCu) >> 2u);
            n = ((n & 0x0F0F0F0Fu) << 4u) | ((n & 0xF0F0F0F0u) >> 4u);
            n = ((n & 0x00FF00FFu) << 8u) | ((n & 0xFF00FF00u) >> 8u);
            return n;
        }
        else // Assume n is 8 bytes integers
        {
            uint64_t nr = (uint64_t) n;
            uint64_t n0 = RadicalInverseBase2<uint32_t>((uint32_t) nr);
            uint64_t n1 = RadicalInverseBase2<uint32_t>((uint32_t) (nr >> 32));
            return (n0 << 32) | n1;
        }
    }

    template<typename T = double, typename IntegerType = uint32_t>
    inline T RadicalInverseBase2Double(IntegerType n)
    {
        // uint32_t or int
        if constexpr (sizeof(IntegerType) <= 4)
        {
            n = RadicalInverseBase2(n);
            return static_cast<T>(n * static_cast<T>(0x1p-32));
        }
        else // Assume n is 8 bytes integers
        {
            n = RadicalInverseBase2(n);
            return static_cast<T>(n * static_cast<T>(0x1p-64));
        }
    }

    // Templating on base allow compiler to generate faster codes for divisions
    // Ex: x / 3 <=> (static_cast<uint64_t>(x) * 2863311531) >> 33
    // Ref: 
    //      https://doc.lagout.org/security/Hackers%20Delight.pdf: pages 190 and followings
    template<uint32_t Base, typename IntegerType = uint32_t>
    inline IntegerType RadicalInverseIntInternal(IntegerType N)
    {
        IntegerType result = 0;
        while (N)
        {
            const IntegerType next = N / Base;
            const IntegerType digit = N - next * Base;

            result = result * Base + digit;
            N = next;
        }

        return result;
    }

    // Non-templated version
    template<typename IntegerType = uint32_t>
    inline IntegerType RadicalInverseIntInternal(IntegerType N, uint32_t Base)
    {
        IntegerType result = 0;
        while (N)
        {
            const IntegerType next = N / Base;
            const IntegerType digit = N - next * Base;

            result = result * Base + digit;
            N = next;
        }
        return result;
    }

    template<uint32_t Base, typename T = double, typename IntegerType = uint32_t>
    inline T RadicalInverseDoubleInternal(IntegerType N)
    {
        const T invBase = (T) 1.0 / (T) Base;
        T invBaseN = (T)1.0;

        IntegerType result = 0;
        while (N)
        {
            const IntegerType next = N / Base;
            const IntegerType digit = N - next * Base;

            result = result * Base + digit;
            invBaseN *= invBase;
            N = next;
        }

        return result * invBaseN;
    }

    // Non-templated version
    template<typename T = double, typename IntegerType = uint32_t>
    inline T RadicalInverseDoubleInternal(uint32_t N, uint32_t Base)
    {
        const T invBase = (T) 1.0 / (T) Base;
        T invBaseN = (T)1.0;

        uint32_t result = 0;
        while (N)
        {
            const uint32_t next = N / Base;
            const uint32_t digit = N - next * Base;

            result = result * Base + digit;
            invBaseN *= invBase;
            N = next;
        }

        return result * invBaseN;
    }

    template<typename IntegerType = uint32_t>
    inline IntegerType RadicalInverseInt(IntegerType N, uint32_t Base)
    {
        switch (Base)
        {
        case  2: return RadicalInverseIntInternal< 2, IntegerType>(N);
        case  3: return RadicalInverseIntInternal< 3, IntegerType>(N);
        case  5: return RadicalInverseIntInternal< 5, IntegerType>(N);
        case  7: return RadicalInverseIntInternal< 7, IntegerType>(N);
        case 11: return RadicalInverseIntInternal<11, IntegerType>(N);
        case 13: return RadicalInverseIntInternal<13, IntegerType>(N);
        case 17: return RadicalInverseIntInternal<17, IntegerType>(N);
        case 19: return RadicalInverseIntInternal<19, IntegerType>(N);
        case 23: return RadicalInverseIntInternal<23, IntegerType>(N);
        case 29: return RadicalInverseIntInternal<29, IntegerType>(N);
        default:
            return RadicalInverseIntInternal(N, Base);
        }
    }

    template<typename T = double, typename IntegerType = uint32_t>
    inline T RadicalInverseDouble(IntegerType N, uint32_t Base)
    {
        switch (Base)
        {
        case  2: return RadicalInverseDoubleInternal< 2, T, IntegerType>(N);
        case  3: return RadicalInverseDoubleInternal< 3, T, IntegerType>(N);
        case  5: return RadicalInverseDoubleInternal< 5, T, IntegerType>(N);
        case  7: return RadicalInverseDoubleInternal< 7, T, IntegerType>(N);
        case 11: return RadicalInverseDoubleInternal<11, T, IntegerType>(N);
        case 13: return RadicalInverseDoubleInternal<13, T, IntegerType>(N);
        case 17: return RadicalInverseDoubleInternal<17, T, IntegerType>(N);
        case 19: return RadicalInverseDoubleInternal<19, T, IntegerType>(N);
        case 23: return RadicalInverseDoubleInternal<23, T, IntegerType>(N);
        case 29: return RadicalInverseDoubleInternal<29, T, IntegerType>(N);
        default:
            return RadicalInverseDoubleInternal<T>(N, Base);
        }
    }

    template<typename T = double, typename IntegerType = uint32_t>
    inline T convertFullRadicalInverseBase2(IntegerType x)
    {
        // Just forward integers
        if constexpr (std::is_integral_v<T>)
        {
            return static_cast<T>(x);
        }
        else
        {
            if constexpr (sizeof(IntegerType) == 4)
            {
                return x * 0x1p-32;
            }
            else if constexpr (sizeof(IntegerType) == 8)
            {
                return x * 0x1p-64;
            }
            else 
            {
                // std::pow is not constexpr until C++23... So computation
                // can not be done at compile time... That's why is is separated away
                static constexpr T factor = std::pow(1.0 / 2.0, 8 * sizeof(IntegerType));
                return x * factor;
            }
            // Compute base 2 thing
            // T r = 0.0;
            // T b = static_cast<T>(0.5);
            // while (x)
            // {
            //     r += (x & 1) * b;
            // 
            //     b *= static_cast<T>(0.5);
            //     x >>= 1;
            // }
            // 
            // return r;
        }
    }
};