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

#include "PMJ/PJSampleSequenceGenerator.h"
#include "PMJ/PMJSampleSequenceGenerator.h"
#include "PMJ/PMJ02SampleSequenceGenerator.h"
#include "PMJ/PMJ02SampleSequenceGenerator_Pharr.h"

#include <utk/utils/FastPRNG.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/utils/Log.hpp>
#include <random>

namespace utk
{

class SamplerPMJ
{
protected:
public:

    static std::vector<std::string> GetMethods()
    {
        return {
            "PJ", "PMJ", "PMJ02", "PMJ02_Pharr"
        };
    }

    SamplerPMJ(
        const std::string& method_ = "PMJ02",
        uint32_t candidates = 10
    ) : method(method_) { setRandomSeed(); }

    uint32_t GetDimension() const { return 2; }

    void setMethod(const std::string& m) { method = m; }
    void setCandidates(uint32_t nb) { candidates = nb; }
    void setRandomSeed( uint64_t arg_seed ) { seed = arg_seed; }
    void setRandomSeed() { seed = std::random_device{}(); }

    template<typename T>
    bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
    {
        if (method == "PJ")
        {
            // Increment seed for samples not to be the same next time they are asked.
            PJSampleSequenceGenerator g = PJSampleSequenceGenerator(seed++);
            g.progressiveJitteredAlgorithm2D(N);
            
            SetSamples(N, arg_pts, g);
        }
        else if (method == "PMJ")
        {
            // Increment seed for samples not to be the same next time they are asked.
            PMJSampleSequenceGenerator g = PMJSampleSequenceGenerator(seed++);
            g.ProgressiveMultiJitteredAlgorithm2D(N, candidates);
        
            SetSamples(N, arg_pts, g);
        }
        else if (method == "PMJ02_Pharr")
        {
            // Increment seed for samples not to be the same next time they are asked.
            PMJ02SampleSequenceGenerator_Pharr g = PMJ02SampleSequenceGenerator_Pharr(seed++);
            g.ProgressiveMultiJittered02Algorithm2D(N, candidates);
        
            SetSamples(N, arg_pts, g);
        }
        else /* if (method == "PMJ02") */ // Assume method == "PMJ02"
        {
            // Increment seed for samples not to be the same next time they are asked.
            PMJ02SampleSequenceGenerator g = PMJ02SampleSequenceGenerator(seed++);
            g.ProgressiveMultiJittered02Algorithm2D(N, candidates);

            SetSamples(N, arg_pts, g);
        }

        return true;
    };

protected:
    template<typename Pts, typename Sampler>
    void SetSamples(uint32_t N, Pts& arg_pts, const Sampler& sampler)
    {
        const SamplePMJ* samples = sampler.GetSamples();
        const uint32_t Npts = sampler.GetSampleCount();
        
        if (N != Npts)
            UTK_WARN("Sampler PMJ, returning {} samples when asked for {}", Npts, N);

        arg_pts.Resize(Npts, 2);
        for (uint32_t i = 0; i < Npts; i++)
        {
            arg_pts[i][0] = samples[i][0];
            arg_pts[i][1] = samples[i][1];
        }
    }

    uint32_t candidates;
    unsigned long long int seed;
    std::string method;
    utk::PCG32 gen;
};

}
