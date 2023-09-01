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
#include <cmath>

namespace utk
{

class SamplerDartThrowing
{
protected:
public:

	SamplerDartThrowing(
		uint32_t d,
		bool relaxed_ = true,
		bool toroidal_ = true,
		uint32_t numTrials_ = 1000,
		double relaxedFactor_ = 0.9,
		double spherePacking = -1.0
	) : 
		D(d), toroidal(toroidal_), relaxed(relaxed_),
		numTrials(numTrials_), relaxedFactor(relaxedFactor_ * relaxedFactor_)	
	{ 
		setDistance(spherePacking);
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
	void setDistance(double distance) 
	{
		// I don't like maccros
		static constexpr double PI = 3.14159265358979323846;
	
		if (distance < 0)
		{
			// Use center density (ie. Number of sphere per unit cube in space)
			// (1.5 * to match UTK's formula)	
			initialDistanceSquared = GetPackingDistance(D);
			initialDistanceSquared = 1.5 * std::pow(initialDistanceSquared * std::tgamma(D / 2. + 1) / std::pow(PI, D / 2.), 1. / D);
		}
		else
		{
			initialDistanceSquared = distance;
		}
		// Keep it squared 
		initialDistanceSquared *= initialDistanceSquared;
	}

	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
		std::uniform_real_distribution<T> dist(0, 1);
		
		// TODO : is using arg_pts[i] instead faster ? 
		std::vector<T> pt(D);
		arg_pts.Resize(N, D);

		auto dist_function = (toroidal) ? &SamplerDartThrowing::computeToroidalDistSquared<T> :
		                                  &SamplerDartThrowing::computeDistSquared<T>;  

		// Scale by N ^ (1/D) (squared because we only store squared distances...)
		T currentDist = static_cast<T>(initialDistanceSquared * std::pow(1. / (T)N, 2. / (T)D));

        for (uint32_t i = 0; i < N; /* change i only when sample is accepted */)
		{
			bool accept = false;
			for (uint32_t it = 0; (it < numTrials) && !accept; it++)
			{
				for (uint32_t d = 0; d < D; d++)
					pt[d] = dist(gen);

				accept = true;
				for (uint32_t j = 0; (j < i) && accept; j++)
				{
					const T dist = dist_function(pt.data(), arg_pts[j], D);
					accept = (dist > currentDist);
				}
			}

			if (accept) 
			{
				std::memcpy(arg_pts[i], pt.data(), sizeof(T) * D);
				i++;
			}
			else if (relaxed) 
			{
				currentDist *= relaxedFactor;
			}
		}
		return true;
	};

// utk::SamplerDartThrowing::GetDistFunction() [with T = double]::<lambda(const double*, const double*)>
// utk::SamplerDartThrowing::GetDistFunction() [with T = double]::<lambda(const double*, const double*)>
private:
	template<typename T>
	static T computeDistSquared(const T* a, const T* b, uint32_t n)
	{
		T dist = 0.0;
		for (uint32_t i = 0; i < n; i++)
		{
			const T diff = (a[i] - b[i]);
			dist += diff * diff; 
		}
		return dist;
	}
	template<typename T>
	static T computeToroidalDistSquared(const T* a, const T* b, uint32_t n)
	{
		T dist = 0.0;
		for (uint32_t i = 0; i < n; i++)
		{
			const T diff = std::abs(a[i] - b[i]);
			const T toric_diff = std::min(diff, 1. - diff);
			dist += toric_diff * toric_diff;
		}
		return dist;
		
	}

	static double GetPackingDistance(uint32_t D)
	{
		// I simply don't like maccros for constants ^^'
		static constexpr double PI = 3.14159265358979323846;

		double gamman_max = 0;
		switch(D)
		{
			case 1: gamman_max = 1; break;
			case 2: gamman_max = (1.0/6.0)   * PI *           std::sqrt(3); break;
			case 3: gamman_max = (1.0/6.0)   * PI *           std::sqrt(2); break;
			case 4: gamman_max = (1.0/16.0)  * PI * PI                    ; break;
			case 5: gamman_max = (1.0/30.0)  * PI * PI *      std::sqrt(2); break;
			case 6: gamman_max = (1.0/144.0) * PI * PI * PI * std::sqrt(3); break;
			case 7: gamman_max = (1.0/105.0) * PI * PI * PI               ; break;
			case 8: gamman_max = (1.0/384.0) * PI * PI * PI * PI;           break;
			default:
			{
				gamman_max = (1.0/384.0) * PI * PI * PI * PI;
				UTK_WARN("Sampler DartThrowing does not have builtin value for D > 8. Using value for D = 8, for D = {} ({})", D, gamman_max);
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
	double initialDistanceSquared;

    utk::PCG32 gen;
};

}
