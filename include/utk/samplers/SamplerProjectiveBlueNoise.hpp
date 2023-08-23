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
#include <utk/utils/Log.hpp>
#include <cstring> // memcpy
#include <random>
#include <algorithm>
#include <vector>
#include <cmath>

class CombinationGenerator
{
public:
    CombinationGenerator(uint32_t n, uint32_t k, std::vector<bool>* buff) : 
        buffer(buff),
        N(n), K(k)
    {
        end = false;
        buff->resize(N);
        
        std::fill(buff->begin(), buff->begin() + K, true);
        std::fill(buff->begin() + K,   buff->end(), false);
    }

    bool GetNext(std::vector<uint32_t>& permut)
    {
        if (end) return false;
        
        permut.resize(0);
        for (uint32_t i = 0; i < N; i++)
            if (buffer->at(i)) permut.push_back(i);
        
        end = !std::prev_permutation(buffer->begin(), buffer->end());
        return true;
    }
private:
    bool end;
    std::vector<bool>* buffer;
    const uint32_t N;
    const uint32_t K;
};


namespace utk
{

class SamplerProjectiveBlueNoise
{
protected:
public:

	SamplerProjectiveBlueNoise(
		uint32_t d,
		bool relaxed_ = true,
		bool toroidal_ = true,
		uint32_t numTrials_ = 1000,
		double relaxedFactor_ = 0.9
	) : 
		D(d), toroidal(toroidal_), relaxed(relaxed_),
		numTrials(numTrials_), relaxedFactor(relaxedFactor_ * relaxedFactor_)	
	{ 
		setRandomSeed(); 
	}
	
	void setDimension(uint32_t d) { D = d; }
    uint32_t GetDimension() const { return D; }
	
	void setRandomSeed( uint64_t arg_seed ) { gen.seed(arg_seed); }
	void setRandomSeed() { gen.seed(std::random_device{}()); }

	void setRelaxed(bool re) { relaxed = re; }
	void setToroidal(bool trd) { toroidal = trd; }
	void setMaxIters(uint32_t its) { numTrials = its; }
	void setRelaxedFactor(double factor) { relaxedFactor = factor * factor; /* we use square dist */}

	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
		std::uniform_real_distribution<T> dist(0, 1);
		
		// TODO : is using arg_pts[i] instead faster ? 
		std::vector<T> pt(D);
		arg_pts.Resize(N, D);

		auto dist_function = (toroidal) ? &SamplerProjectiveBlueNoise::computeToroidalDistSquared<T> :
		                                  &SamplerProjectiveBlueNoise::computeDistSquared<T>;  
		
		// Buffers to avoid reallocation ! 
		std::vector<bool> combinationBuffer(D);
		std::vector<uint32_t> combination(D);

		// Compute per-projection distances
		std::vector<T> projectiveDistances(D);
		for (uint32_t d = 0; d < D; d++)
			projectiveDistances[d] = GetDistance<T>(N, d + 1);

		T currentRelaxFactor = static_cast<T>(1.0);
		for (uint32_t i = 0; i < N; /* change i only when sample is accepted */)
		{
			bool accept = false;
			for (uint32_t it = 0; (it < numTrials) && !accept; it++)
			{
				for (uint32_t d = 0; d < D; d++)
					pt[d] = dist(gen);

				accept = true;
				// For each size of projection
				for (uint32_t k = 0; (k < D) && accept; k++)
				{
					const T currentDist = projectiveDistances[k] * currentRelaxFactor; 
					CombinationGenerator generator(D, k + 1, &combinationBuffer);

					// For each projection of size k
					while (generator.GetNext(combination) && accept)
					{
						// For each previous point
						for (uint32_t j = 0; (j < i) && accept; j++)
						{
							const T dist = dist_function(pt.data(), arg_pts[j], combination);
							accept = (dist > currentDist);
						}
					}
				}
			}

			if (accept) 
			{
				std::memcpy(arg_pts[i], pt.data(), sizeof(T) * D);
				i++;
			}
			else if (relaxed) 
			{
				currentRelaxFactor *= relaxedFactor;
			}
		}
		return true;
	};


private:
	template<typename T>
	static T computeDistSquared(const T* a, const T* b, const std::vector<uint32_t>& coords)
	{
		T dist = 0.0;
		for (uint32_t i = 0; i < coords.size(); i++)
		{
			const T diff = (a[coords[i]] - b[coords[i]]);
			dist += diff * diff; 
		}
		return dist;
	}
	
	template<typename T>
	static T computeToroidalDistSquared(const T* a, const T* b, const std::vector<uint32_t>& coords)
	{
		T dist = 0.0;
		for (uint32_t i = 0; i < coords.size(); i++)
		{
			const T diff = std::abs(a[coords[i]] - b[coords[i]]);
			const T toric_diff = std::min(diff, 1. - diff);
			dist += toric_diff * toric_diff;
		}
		return dist;
		
	}

	template<typename T>
	static T GetDistance(uint32_t N, uint32_t k)
	{
		T gamman_max = 0;
		switch(k)
		{
			// Note: Everything is squared !
			// 1 - 4 : taken from [Reinert 2015]
			// 5 - 6 : from UTK (which, given general formula of [Reinert 2015] seems correct)
			case 1: gamman_max = 1.0 / (4.0 * N * N)                                 ; break;
			case 2: gamman_max = 1.0 / (        2.0 * N * std::sqrt(3.0))            ; break;
			case 3: gamman_max = 1.0 / std::pow(4.0 * N * std::sqrt(2.0), 1.0 /  6.0); break;
			case 4: gamman_max = 1.0 / std::pow(8.0 * N                 , 1.0 /  8.0); break;
			case 5: gamman_max = 1.0 / std::pow(8.0 * N * std::sqrt(2.0), 1.0 / 10.0); break;
			case 6: gamman_max = 1.0 / std::pow(8.0 * N * std::sqrt(3.0), 1.0 / 12.0); break;
			default:
			{
				gamman_max = 1.0 / std::pow(8.0 * N * std::sqrt(3.0), 1.0 / 12.0);
				UTK_WARN("Sampler Projective blue noise does not have builtin packing values for D > 6. Using value for D = 6, for D = {} ({})", k, gamman_max);
			}
			
		}

		return gamman_max;
	}

protected:
	uint32_t D;

	bool toroidal;
	bool relaxed;

	uint32_t numTrials;
	double relaxedFactor;

    utk::PCG32 gen;
};

}
