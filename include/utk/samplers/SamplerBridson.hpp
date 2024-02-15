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
#include <map>

namespace utk
{

class SamplerBridson
{
protected:
public:
	SamplerBridson(
		uint32_t d,
		bool toroidal_ = true,
		uint32_t candidates = 30,
		double distance = -1.0
	) : 
		D(d), toroidal(toroidal_),
		numCandidates(candidates)	
	{ 
		setDistance(distance);
		setRandomSeed(); 
	}
	
	void setDimension(uint32_t d) { D = d; }
    uint32_t GetDimension() const { return D; }

	void setRandomSeed( uint64_t arg_seed ) { gen.seed(arg_seed); }
	void setRandomSeed() { gen.seed(std::random_device{}()); }

	void setToroidal(bool trd) { toroidal = trd; }
	void setCandidates(uint32_t k) { numCandidates = k; }
	void setDistance(double distance) 
	{
		// I don't like maccros
		static constexpr double PI = 3.14159265358979323846;
	
		if (distance < 0)
        {
            // Use center density (ie. Number of sphere per unit cube in space)
            // (1.5 * to match UTK's formula)
	        initialDistance = GetPackingDistance(D);
            initialDistance = 1.5 * std::pow(initialDistance * std::tgamma(D / 2. + 1) / std::pow(PI, D / 2.), 1. / D);
        }
		else
        {
            initialDistance = distance;
        }
    }

    template<typename T>
	bool generateSamples(Pointset<T>& args_pts, uint32_t N)
    {
        using Index = std::vector<uint32_t>;
        if (N == 0) { args_pts.Resize(0, D); return true; }
        
        const std::uint32_t neighbors = (1 << (D + 1));

        const T r = static_cast<T>(initialDistance * std::pow(1. / (double)N, 1. / (double)D));
            const T cellSize = r / std::sqrt(D); // As suggested by Bridson
        const uint32_t cellCount = static_cast<uint32_t>(1. / cellSize) + 1;

        if (cellSize > 1.)
        {
            UTK_ERROR("Cell size is greater than 1 ({}) !", cellSize);
            return false;
        }

        auto dist_function = (toroidal) ? &SamplerBridson::computeToroidalDistSquared<T> :
		                                  &SamplerBridson::computeDistSquared<T>;  

        // Use a std map instead of a fully allocated grid. 
        // This saves memory (especially when d > 3), but cost
        // search time of log(N)... 
        std::map<Index, uint32_t> grid;     
        std::vector<typename decltype(grid)::iterator> activeList;  

        // Might samples less points
        args_pts.Resize(1, D, N); 

        // Generate first point
        Index idx(D, 0);
        for (uint32_t d = 0; d < D; d++)
        {
            args_pts[0][d] = std::uniform_real_distribution<T>(0, 1)(gen);
            idx[d] = static_cast<uint32_t>(args_pts[0][d] / cellSize);
        }

        grid[idx] = 0;
        activeList.push_back(grid.begin());

        std::uniform_real_distribution<T> radius(0, r);
        std::normal_distribution<T> normal(0, 1);

        std::vector<double> tmpPoints(D, 0.0);
        Index checkIndex(D, 0);
        Index      index(D, 0);

        while (activeList.size() != 0 && args_pts.Npts() < N)
        {
            bool reject = true;
            uint32_t currentIndex = activeList.back()->second;

            // Multiple tries
            for (std::uint32_t k = 0; (k < numCandidates) && reject; k++)
            {
                bool in_box = true;    
                reject      = false;
                
                // Generate random number in the anulus (x, r, 2r)
                {
                    T norm = 0.0;
                    for (std::uint32_t d = 0; d < D; d++)
                    { 
                        tmpPoints[d] = normal(gen); 
                        norm += tmpPoints[d] * tmpPoints[d]; 
                    }

                    T rad = std::pow(radius(gen) + r, 1. / D);
                    norm  = std::sqrt(norm);
                    for (std::uint32_t d = 0; d < D; d++)
                    {   
                        tmpPoints[d] = (tmpPoints[d] / norm) * rad + args_pts[currentIndex][d];
                        in_box = in_box && (tmpPoints[d] >= 0) && (tmpPoints[d] <= 1);
                        index[d] = static_cast<uint32_t>(tmpPoints[d] / cellSize);
                    }
                }
                
                // Reject point if not in domain !
                if (!in_box) { reject = true; continue; }
                // Check all boxes arround the point
                if (grid.find(index) != grid.end()) { reject = true; continue; }

                for (std::uint32_t i = 0; i < (neighbors + 1) && !reject; i++)
                {
                    uint32_t gidx = i;
                    for (std::uint32_t d = 0; d < D; d++)
                    {
                        int64_t new_index = index[d] + (gidx % 3 - 1); gidx /= 3;

                        // Toroidal shift 
                        if (new_index <  0)         new_index = toroidal ? cellCount - 1 : 0;
                        if (new_index >= cellCount) new_index = toroidal ?             0 : cellCount - 1;

                        checkIndex[d] = static_cast<uint32_t>(new_index);
                    }
                                            
                    auto it = grid.find(checkIndex);
                    if (it != grid.end())
                    {
                        if (dist_function(tmpPoints.data(), args_pts[it->second], D) < r * r)
                        {
                            reject = true;
                            break;
                        }
                    }
                }

                if (!reject)
                {
                    // Add a point
                    args_pts.PushBack(tmpPoints.data());
                    auto it = grid.insert({index, args_pts.Npts() - 1}).first;
                    activeList.push_back(it);
                }
            }

            // Too many tries, just remove point from the list 
            if (reject == true) activeList.pop_back();
        }

        return true;
    }

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
	uint32_t numCandidates;
	double initialDistance;

    PCG32 gen;
};

}
