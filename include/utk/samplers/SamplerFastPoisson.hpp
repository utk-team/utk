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
#include <utk/utils/log.hpp>
#include "FastPoisson/PDSampling.h"
#include <random>

namespace utk
{

/**
 * Code from
 * 
 * Dunbar, D., & Humphreys, G. (2006). 
 * A spatial data structure for fast Poisson-disk sample generation. 
 * ACM Transactions on Graphics (TOG), 25(3), 503-508.
 * http://www.cs.virginia.edu/~gfx/pubs/antimony/
 * 
 */

class SamplerFastPoisson
{
protected:
public:
    // Ugly, but let them in 'floating' string
    static std::vector<std::string> GetMethods()
    {
        return 
        {
            "DartThrowing",
            "BestCandidate",
            "BoundarySampler",
            "Pure",
            "LinearPure",
            "Penrose",
            "Uniform"
        };
    }

	SamplerFastPoisson(
        std::string method_ = "DartThrowing", 
        double radius_ = 0.1,
        uint32_t throwMultiplier_ = 1,
        uint32_t throws_ = 1000, 
        bool maximize_ = false, 
        bool isTiled_ = false, 
        uint32_t relaxCount_ = 0
    ) : 
        method(method_), radius(std::clamp(radius_, 0.0005, 0.2)),
        throwMultiplier(throwMultiplier_), throws(throws_), 
        maximize(maximize_), isTiled(isTiled_), 
        relaxCount(relaxCount_), sampler(nullptr)
    {  }

    uint32_t GetDimension() const { return 2; }

    void setMethod(const std::string& m) { method = m; }
    void setRadius(double rd) { radius = std::clamp(rd, 0.0005, 0.2); }
    void setThrowMultiplier(uint32_t tm) { throwMultiplier = tm; }
    void setThrows(uint32_t t) { throws = t; }
    void setMaximize(bool m) { maximize = m; }
    void setTiled(bool t) { isTiled = t; }
    void setRelaxCount(uint32_t rc) { relaxCount = rc; }


	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
        if (sampler != nullptr)
            delete sampler;

             if (method == "DartThrowing")    sampler = new dunbarPoisson::DartThrowing(radius, isTiled, throws, throwMultiplier);
        else if (method == "BestCandidate")   sampler = new dunbarPoisson::BestCandidate(radius, isTiled, throwMultiplier);
        else if (method == "BoundarySampler") sampler = new dunbarPoisson::BoundarySampler(radius, isTiled);
        else if (method == "Pure")            sampler = new dunbarPoisson::PureSampler(radius); // Ignore tiling
        else if (method == "LinearPure")      sampler = new dunbarPoisson::LinearPureSampler(radius);
        else if (method == "Penrose")         sampler = new dunbarPoisson::PenroseSampler(radius);
        else if (method == "Uniform")         sampler = new dunbarPoisson::UniformSampler(radius);
        else return false;

        sampler->complete();
        if (maximize) sampler->maximize();
        for (uint32_t i = 0; i < relaxCount; i++) sampler->relax();

        if (N != sampler->points.size())
            UTK_WARN("Sampler FastPoisson, returning {} samples when asked for {}", sampler->points.size(), N);
       
        N = sampler->points.size();
        arg_pts.Resize(N, 2);
        for (uint32_t i = 0; i < N; i++)
        {
            // Points are in [-1, 1]
            arg_pts[i][0] = 0.5 * (sampler->points[i].x + 1);
            arg_pts[i][1] = 0.5 * (sampler->points[i].y + 1);
        }
        
		return true;
	};

    ~SamplerFastPoisson()
    {
        if (sampler != nullptr)
            delete sampler;
    }
protected:
    std::string method; 
    double radius;
    uint32_t throwMultiplier;
    uint32_t throws;
    bool maximize; 
    bool isTiled; 
    uint32_t relaxCount;

    dunbarPoisson::PDSampler* sampler;
};

}
