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

#include <utk/utils/FastPRNG.hpp>
#include <utk/utils/Pointset.hpp>
#include <random>

#include <Math/VecX.h>
#include <Tools/iopointset.h>
#include <Tools/mapping.h>
#include <Transport/slicedOptimalTransportNBall.h>

namespace utk
{

class SamplerSOT
{
protected:
public:

	SamplerSOT(
        uint32_t d, 
        uint32_t iterations = 4096, 
        uint32_t batch = 64
    ) : BatchSize(batch), NbIterations(iterations), D(d) 
    { setRandomSeed(); }

    void setIterations(uint32_t its) { NbIterations = its; }
    void setBatchSize(uint32_t bs) { BatchSize = bs; }
    
	void setDimension(uint32_t d) { D = d; }
    uint32_t GetDimension() const { return D; }

	void setRandomSeed( uint64_t arg_seed ) { gen.seed(arg_seed); }
	void setRandomSeed() { gen.seed(std::random_device{}()); }

	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
        arg_pts.Resize(N, D);	
		return GeneratePointsDispatch(arg_pts, N);
	};

private:
    template<typename T>
    bool GeneratePointsDispatch(Pointset<T>& arg_pts, uint32_t N)
    {
        switch (D)
        {
            case  1: return GeneratePoints<T, VecX< 1>>(arg_pts, N);
            case  2: return GeneratePoints<T, VecX< 2>>(arg_pts, N);
            case  3: return GeneratePoints<T, VecX< 3>>(arg_pts, N);
            case  4: return GeneratePoints<T, VecX< 4>>(arg_pts, N);
            case  5: return GeneratePoints<T, VecX< 5>>(arg_pts, N);
            case  6: return GeneratePoints<T, VecX< 6>>(arg_pts, N);
            case  7: return GeneratePoints<T, VecX< 7>>(arg_pts, N);
            case  8: return GeneratePoints<T, VecX< 8>>(arg_pts, N);
            case  9: return GeneratePoints<T, VecX< 9>>(arg_pts, N);
            case 10: return GeneratePoints<T, VecX<10>>(arg_pts, N);
            case 11: return GeneratePoints<T, VecX<11>>(arg_pts, N);
            case 12: return GeneratePoints<T, VecX<12>>(arg_pts, N);
            case 13: return GeneratePoints<T, VecX<13>>(arg_pts, N);
            case 14: return GeneratePoints<T, VecX<14>>(arg_pts, N);
            case 15: return GeneratePoints<T, VecX<15>>(arg_pts, N);
            case 16: return GeneratePoints<T, VecX<16>>(arg_pts, N);
            case 18: return GeneratePoints<T, VecX<18>>(arg_pts, N);
            case 19: return GeneratePoints<T, VecX<19>>(arg_pts, N);
            case 20: return GeneratePoints<T, VecX<20>>(arg_pts, N);
            default: return GeneratePoints<T, VecXDynamic>(arg_pts, N);
        };
    }
    
    template<typename T, typename SOTVec>
    bool GeneratePoints(Pointset<T>& arg_pts, uint32_t N)
    {
        std::vector<SOTVec> points(N, SOTVec(D));

        //Init from whitenoise in ball
        for (size_t i = 0; i < points.size(); ++i) 
        {
            points[i] = randomVectorInBall<SOTVec>(D, gen);
        }

        std::vector<SOTVec> result;
        slicedOptimalTransportNBall(points, result, NbIterations, BatchSize, gen());
        for (unsigned int i = 0; i < result.size(); i++) 
        {
            SOTVec mapped = 0.5 * NBall2NCube(result[i]) + 0.5;
            for (uint32_t d = 0; d < D; d++)
                arg_pts[i][d] = mapped[d];
        }

        return true;
    }


protected:
    uint32_t BatchSize;
    uint32_t NbIterations;

	uint32_t D;
    utk::PCG32 gen;
};

}