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

#include <fstream>
#include <utk/utils/FastPRNG.hpp>
#include <utk/utils/Pointset.hpp>
#include <random>

namespace utk
{

template<typename IntegerType = uint32_t>
class SamplerGenerativeMatrices
{
protected:
public:

	SamplerGenerativeMatrices(std::string fn, uint8_t m, uint8_t p, uint32_t d, uint8_t depth = 0) 
        : filename(std::move(fn)), M(m), P(p), D(d), owenDepth(depth) 
    { setRandomSeed(); }

    void setFilename(std::string fn)  { filename = std::move(fn); }
    const std::string& getFilename() const { return filename; }

    void setBasis(uint8_t p) { P = p; }
    uint8_t getBasis() const { return P;}

    void setLogMaxN(uint8_t m) { M = m; }
    uint8_t getLogMaxN() const { return M; }

	void setDimension(uint32_t d) { D = d; }
    uint32_t GetDimension() const { return D; }

    void setOwenDepth(uint8_t depth) { owenDepth = depth; }
        
	void setRandomSeed( uint64_t arg_seed ) { gen.seed(arg_seed); }
	void setRandomSeed() { gen.seed(std::random_device{}()); }

	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
        if (!loadMatrices())
        {
            UTK_ERROR("SamplerSobol : no such file or directory {}", filename);
            return false;
        }

        std::vector<uint64_t> seeds(D);
        for (uint32_t d = 0; d < D; d++)
            seeds[d] = gen();

        const uint8_t EffectiveDepth = (owenDepth == 0) ? 0 : M;

        arg_pts.Resize(N, D);    

        std::vector<IntegerType> digits(M, 0);
		for(IntegerType i = 0; i < static_cast<IntegerType>(N); i++)
        {
            IntegerType inverse = getDigits(P, i, M, digits.data());
            
            for (uint32_t d = 0; d < D; d++)
            {
                IntegerType x = MatrixProduct(d, inverse, digits.data());
                arg_pts[i][d] = convert<T>(OwenSrambling(x, EffectiveDepth, seeds[d]));
            }
        }
        
		return true;
	};
protected:
    bool loadMatrices()
    {
        std::ifstream matrixFile(filename);

        if (!matrixFile.is_open())
        { return false; }

        for (unsigned int d = 0; d < D; d++)
        {
            std::vector<IntegerType> matrix(M * M, 0);
            for (unsigned int i = 0; i < M * M; i++)
            {
                if (!matrixFile) return false;
                matrixFile >> matrix[i];
            }

            matrices.push_back(std::move(matrix));
        }

        return true;
    }

    static IntegerType getDigits(uint8_t base, IntegerType n, IntegerType M, IntegerType* digits)
    {
        IntegerType current = 1;
        for (uint32_t i = 0; i < M; i++)
        {
            digits[i] = (n / current) % base;
            current *= base; 
        }
        return current;
    }

    IntegerType MatrixProduct(
        uint32_t dim, 
        IntegerType inverse, 
        const IntegerType* digits
    ) const
    {
        IntegerType result = 0;
        for (uint8_t row = 0; row < M; row++)
        {
            inverse /= P;
            uint32_t total = 0;
            for (uint8_t i = 0; i < M; i++)
                total += digits[i] * matrices[dim][i + row * M];

            result += (total % P) * inverse;
        }

        return result;
    }

    IntegerType OwenSrambling(
        IntegerType i, 
        IntegerType depth, 
        IntegerType seed
    ) const
    {
        if (depth == 0) return i;

        utk::PCG32 gen(seed);
        std::uniform_int_distribution<IntegerType> unif(0, P - 1);

        IntegerType res = 0;
        IntegerType current = static_cast<IntegerType>(round(pow(P, M - 1)));

        for (uint8_t k = 0; k < depth; k++)
        {
            IntegerType permut = unif(gen);
            IntegerType digit = (i / current) % P;

            res += ((digit + permut) % P) * current;

            gen.seed(gen() + digit);
            i -= digit * current;
            current /= P;
        }
    
        return res;
    }

    template<typename T>
    T convert(IntegerType i) const
    {
        if constexpr (std::is_integral_v<T>)
        {
            return static_cast<T>(i);
        }
        else
        {
            return static_cast<T>(i) / static_cast<T>(pow(static_cast<T>(P), M)); 
        }
    }
    
protected:
    std::string filename;


    uint8_t M;
    uint8_t P;
	uint32_t D;
    
    uint8_t owenDepth;

    std::vector<std::vector<IntegerType>> matrices;    
    
    utk::PCG32 gen;
};

}
