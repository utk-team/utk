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

#include "Config.hpp"
#include <vector>

#include <utk/samplers/SamplerSobol.hpp>
#include <utk/metrics/GeneralizedL2.hpp>
#include <utk/metrics/IntegrationTest.hpp>
#include <utk/metrics/Integrands/GaussianIntegrands.hpp>
#include <utk/metrics/Integrands/HeavisideIntegrands.hpp>

struct Evaluator
{
public:
    struct Eval
    {
        Eval& operator+=(const Eval& other)
        {
            gl2   += other.gl2;
            gauss += other.gauss;
            heavi += other.heavi;

            return *this;
        }

        Eval operator+(const Eval& other)
        {
            Eval ret = *this;
            ret += other;
            return ret;
        }

        Eval& operator/=(unsigned int N)
        {
            gl2 /= N;
            gauss /= N;
            heavi /= N;

            return *this;
        }

        Eval operator/(unsigned int N)
        {
            Eval ret = *this;
            ret /= N;
            return ret;
        }

        unsigned int N = 0;
        unsigned int d = 0;
        double gl2   = 0.;
        double gauss = 0.;
        double heavi = 0.;
    };

    Evaluator(const Config& config)
    {
        utk::Pointset<double> sobol;
        utk::SamplerSobol<uint32_t>(config.dim, 32).generateSamples(sobol, config.nInt);
        
        gauss.BuildDatabase<utk::GaussianIntegrand> ("tmp_gauss.txt", config.dim, config.mInt, config.seed, {}, sobol);
        heavi.BuildDatabase<utk::HeavisideIntegrand>("tmp_heavi.txt", config.dim, config.mInt, config.seed, {}, sobol);
    }

    Eval EvalPointset(const utk::Pointset<double>& pts) const
    {
        double gl2    = utk::GL2Discrepancy().compute(pts);
        double gaussi = gauss.compute(pts).mean;
        double heavii = heavi.compute(pts).mean; 

        return Eval { .N = pts.Npts(), .d = pts.Ndim(), .gl2 = gl2, .gauss = gaussi, .heavi = heavii };
    }

    utk::IntegrationTest  gauss;
    utk::IntegrationTest heavi;
};

inline void WriteEval(
    const char* out, 
    const std::vector<const char*>& names, 
    const std::vector<std::vector<Evaluator::Eval>>& evals)
{
    std::ofstream samplers_data { "evals.csv" };
    samplers_data << "name" << "," << "N" << "," << "dim" << "," << "metric" << "," << "value" << "\n";
    for (unsigned int i = 0; i < names.size(); i++)
    {
        for (unsigned int j = 0; j < evals[i].size(); j++)
        {
            samplers_data << names[i] << "," << evals[i][j].N << "," << evals[i][j].d << ",";
            samplers_data << "gl2" << "," << evals[i][j].gl2 << '\n';

            samplers_data << names[i] << "," << evals[i][j].N << "," << evals[i][j].d << ",";
            samplers_data << "gauss" << "," << evals[i][j].gauss << '\n';
            
            samplers_data << names[i] << "," << evals[i][j].N << "," << evals[i][j].d << ",";
            samplers_data << "heavi" << "," << evals[i][j].heavi << '\n';
        }
    }
    samplers_data.close();
}