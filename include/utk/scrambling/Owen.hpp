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

#include <utk/utils/Pointset.hpp>
#include <utk/utils/FastPRNG.hpp>
#include <utk/utils/RadicalInversion.hpp>
#include <random>
#include <bitset>

namespace utk
{
    template <typename IntegerType = uint32_t>
    class ScramblingOwen
    {
    public:
        static constexpr unsigned char MAX_BITS = (sizeof(IntegerType) * __CHAR_BIT__);
    
        ScramblingOwen(unsigned char dp = MAX_BITS) : 
            depth(dp)
        { }

        void setOwenDepth(unsigned char dp)
        {
            if (dp < MAX_BITS)
                depth = dp;
            else
                depth = MAX_BITS;
        }

        void setRandomSeed(uint64_t arg_seed) 
        { 
            mt.seed(arg_seed);
        }

        void setRandomSeed() 
        { 
            setRandomSeed(std::random_device{}());
        }

        template<typename T>
        void Scramble(Pointset<T>& in)
        {
            std::vector<uint64_t> seeds(in.Ndim());
            for (uint32_t d = 0; d < in.Ndim(); d++)
                seeds[d] = mt();
            
            for (IntegerType i = 0; i < in.Npts(); i++)
            {
                for (uint32_t d = 0; d < in.Ndim(); d++)
                {
                    in[i][d] = owen(in[i][d], d, depth, seeds.data());
                }
            }
        }

        template<typename T, typename D>
        void Scramble(const Pointset<T>& in, Pointset<D>& out)
        {
            out.Resize(in.Npts(), in.Ndim());

            std::vector<uint64_t> seeds(in.Ndim());
            for (uint32_t d = 0; d < in.Ndim(); d++)
                seeds[d] = mt();
            
            for (IntegerType i = 0; i < in.Npts(); i++)
            {
                for (uint32_t d = 0; d < in.Ndim(); d++)
                {
                    out[i][d] = convertFullRadicalInverseBase2<D>(owen(in[i][d], d, depth, seeds.data()));
                }
            }
        }
        
    private:
        static IntegerType ConvertBitset(const std::bitset<MAX_BITS>& in)
        {
            if constexpr (sizeof(IntegerType) == 4)
                return in.to_ulong();
            else
                return in.to_ullong();
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

            RNG rng;
            for (unsigned char idigit = 0; idigit < depth; idigit++)
            {
                IntegerType indPermut = (1 << idigit) - 1 + ConvertBitset(digitsBase2 >> (MAX_BITS - idigit));
                uint64_t seed = seeds[dim] + indPermut;
                
                rng.seed(seed);
                int thisDigitPermut = rng.sample() & 1;
                newDigits[(MAX_BITS - 1) - idigit] = (thisDigitPermut ^ digitsBase2[(MAX_BITS - 1) - idigit]);
            }

            return ConvertBitset(newDigits);
        }
    private:
        unsigned char depth;
        std::mt19937 mt;
    };
};