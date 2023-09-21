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
#include <utk/utils/Log.hpp>
#include <utk/utils/FastPRNG.hpp>
#include <utk/utils/RadicalInversion.hpp>
#include <fstream>
#include <type_traits>
#include <bitset>
#include <array>
#include <random>


namespace utk
{
    template <typename IntegerType = uint32_t>
    class SamplerSobol
    {
    public:
        static constexpr unsigned char MAX_BITS = (sizeof(IntegerType) * __CHAR_BIT__);
        
        static std::string GetDefaultTablePath()
        {
            return std::string(UTK_DATA_PATH) + "/Sobol/new-joe-kuo-6.21201";
        }

        SamplerSobol(
            uint32_t d,
            unsigned char owenDepth = 32,
            const std::string& fp = "") : D(d) 
        { 
            setDirectionFile(fp);
            setOwenDepth(owenDepth);
            setRandomSeed(); 
        }
    
        void setDimension(uint32_t d) { D = d; }
        uint32_t GetDimension() const { return D; }

        void setOwenDepth(unsigned char depth) { owenDepth = (depth > MAX_BITS) ? MAX_BITS : depth; }
        void setDirectionFile(const std::string& file) 
        { 
            if (file.empty())
                directionFile = GetDefaultTablePath();
            else
                directionFile = file;
        }

        void setRandomSeed(uint64_t arg_seed) { gen.seed(arg_seed); }
        void setRandomSeed() { gen.seed(std::random_device{}()); }

        template <typename T>
        bool generateSamples(Pointset<T> &arg_pts, IntegerType N)
        {
            sobol_mk.resize(D);
            if (!load_mk(directionFile))
            {
                UTK_ERROR("SamplerSobol : no such file or directory {}", directionFile);
                return false;
            }
                
            std::vector<uint64_t> seeds(D);
            for (uint32_t d = 0; d < D; d++)
                seeds[d] = gen();

            arg_pts.Resize(N, D);
            for (IntegerType i = 0; i < N; i++)
            { 
                arg_pts[i][0] = convertFullRadicalInverseBase2<T>(owen(RadicalInverseBase2(i), 0, owenDepth, seeds.data()));
                for (uint32_t d = 1; d < D; d++)
                    arg_pts[i][d] = convertFullRadicalInverseBase2<T>(owen(sobol_binary(i, sobol_mk[d].data()), d, owenDepth, seeds.data()));
            }

            return true;
        };

    protected:
        static IntegerType sobol_binary(IntegerType i, const IntegerType* mk) 
        {
            // `x` is the result binary representation of a float
            IntegerType x = 0;
            // For the k-th bit of the index, if this bit is one, accumulate
            // the k-th direction number modulo 2 (using a xor).
            for (unsigned char k = 0; k < MAX_BITS; ++k) 
            {
                x <<= 1;
                if (i & 1) 
                {
                    x ^= *(mk++);
                } 
                else 
                {
                    mk++;
                }
                i >>= 1;
            }
            return x;
        }
      
        static void generate_mk(IntegerType aj, IntegerType sj, IntegerType* mk) 
        {
            /* For each direction number, we apply the recurrence formula:
            *    m_k = aj_1 2 m_{k-1} xor ... xor aj_{sj} 2^{sj} m_{k-sj}
            *                                 xor mk_{k-sj}
            */
            const unsigned char size = MAX_BITS;
            for (unsigned char k = sj; k < size; ++k) 
            {
                mk[k] = 0;
                for (IntegerType i = 1; i < sj; ++i) {
                    // `akj` stores aj_k, note that the polynomial rep is reversed
                    // `pw2` stores 2^k
                    const IntegerType akj = (aj >> (sj - 1 - i)) & 1;
                    const IntegerType pw2 = (1 << i);
                    mk[k] ^= akj * pw2 * mk[k - i];
                }
                mk[k] ^= (1 << sj) * mk[k - sj];
                mk[k] ^= mk[k - sj];
            }
        }

        static IntegerType owen(
            IntegerType i,
            uint32_t dim, 
            unsigned char depth,
            const uint64_t* seeds
        )
        {
            std::bitset<MAX_BITS> digitsBase2 = i;
            std::bitset<MAX_BITS> newDigits = i;

            PCG32 rng;
            for (unsigned char idigit = 0; idigit < depth; idigit++)
            {
                IntegerType indPermut = (1 << idigit) - 1 + (digitsBase2 >> (MAX_BITS - idigit)).to_ullong();
                uint64_t seed = seeds[dim] + indPermut;
                
                rng.seed(seed);
                int thisDigitPermut = rng() & 1;
                newDigits[(MAX_BITS - 1) - idigit] = (thisDigitPermut ^ digitsBase2[(MAX_BITS - 1) - idigit]);
            }

            return newDigits.to_ullong();
        }
    protected:

        bool load_mk(const std::string& filename)
        {            
            std::ifstream file(filename);
            if (!file.is_open())
                return false;
            
            // JKheader_flag : skip header, otherwise continue loading file in Joe & Kuo format
            file.ignore(256, '\n');
    
            // Load all the mk, aj and sj
            uint32_t index = 1;
            while (file.good() && index < D)
            {
                // Parse the polynomial
                IntegerType d, sj, aj;
                file >> d >> sj >> aj;

                // Parse the direction numbers
                for (IntegerType i = 0; i < sj; ++i)
                    file >> sobol_mk[index][i];

                // Fill the remaining direction numbers
                generate_mk(aj, sj, sobol_mk[index].data());
                index++;
            }
            return (index == D);
        } 


    protected:
        uint32_t D;
        utk::PCG32 gen;

        unsigned char owenDepth;
        std::string directionFile;

        // mk = Direction numbers * 2^k
        std::vector<std::array<IntegerType, MAX_BITS>> sobol_mk;
    };

}
