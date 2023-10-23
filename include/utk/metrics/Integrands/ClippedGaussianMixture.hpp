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
#include "../IntegrationTest.hpp"
#include "GaussianIntegrands.hpp"

#include <vector>
#include <random>

#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

namespace utk
{
    struct Squares
    {
        std::vector<double> mins;
        std::vector<double> maxs;
    };

    class ClippedGaussianMixture : public Integrand
    {
    public:
        ClippedGaussianMixture(uint32_t d) : Integrand(d) 
        { 
        }

        void GenerateRandom(const GenerationParameter& params, uint64_t seed) override
        {
            double sigmaMin = 0.01;
            double sigmaMax = 1.00;

            if (params.find("smin") != params.end())
                sigmaMin = std::get<double>(params.at("smin")); 
            if (params.find("smax") != params.end())
                sigmaMax = std::get<double>(params.at("smax"));

            uint32_t nBoxesMin = 2 * dim;
            uint32_t nBoxesMax = 2 * dim * dim;
            if (params.find("nbmin") != params.end())
                nBoxesMin = std::get<uint32_t>(params.at("nbmin"));
            if (params.find("nbmax") != params.end())
                nBoxesMax = std::get<uint32_t>(params.at("nbmax"));

            // At least one box !
            if (nBoxesMin == 0) nBoxesMin = 1;

            utk::PCG32 mt(seed);

            uint32_t nBoxes = std::uniform_int_distribution<uint32_t>(nBoxesMin, nBoxesMax)(mt);

            squares.resize(nBoxes);
            mus.resize(dim * nBoxes);
            invSigmas.resize(dim * dim * nBoxes * nBoxes);

            for (unsigned int i = 0; i < nBoxes; i++)
            {
                squares[i].mins.resize(dim);
                squares[i].maxs.resize(dim);

                mat rot = random_unitary_matrix(dim, mt);

                std::uniform_real_distribution<double> unifSquare(0, 1);
                for (unsigned int d = 0; d < dim; d++)
                {
                    double x = unifSquare(mt);
                    double y = unifSquare(mt);
                    squares[i].mins[d] = std::min(x, y);
                    squares[i].maxs[d] = std::max(x, y);
                }

                std::uniform_real_distribution<double> unifSigma(sigmaMin, sigmaMax);
                
                for (uint32_t d = 0; d < dim; d++)
                {
                    std::uniform_real_distribution<double> unifMu(squares[i].mins[d], squares[i].maxs[d]);
                    mus[i * dim + d] = unifMu(mt);
                }
                    
                std::vector<double> sigmas(dim, 0.0);
                for (uint32_t i = 0; i < dim; i++)
                    sigmas[i] = 1. / unifSigma(mt);

                // Q = rot matrix (rotation), S = diagonal matrix (size)
                // Covariance matrix is QSQ' which is SDP
                // Its inverse : Q S^-1 Q', because Q^-1 = Q'
                const uint32_t idx = i * dim * dim;
                for (uint32_t di = 0; di < dim; di++)
                {
                    for (uint32_t dj = 0; dj < dim; dj++)
                    {
                        invSigmas[idx + dj + di * dim] = 0.0;
                        for (uint32_t dk = 0; dk < dim; dk++)
                        {
                            invSigmas[idx + dj + di * dim] += rot->v[di][dk] * sigmas[dk] * rot->v[dk][dj];
                        }
                    }
                }

                matrix_delete(rot);
            }
        }   

        void ReadFromStream(std::istream& stream) override 
        {
            uint32_t nBoxes = 0; stream >> nBoxes;
            squares.resize(nBoxes);
            mus.resize(nBoxes * dim);
            invSigmas.resize(nBoxes * dim * dim);

            for (uint32_t i = 0; i < nBoxes; i++)
            {
                squares[i].mins.resize(dim);
                squares[i].maxs.resize(dim);
                
                for (unsigned int d = 0; d < dim; d++)
                    stream >> squares[i].mins[d] >> squares[i].maxs[d];
                
                for (unsigned int d = 0; d < dim; d++)
                    stream >> mus[i * dim + d];
                
                for (unsigned int d = 0; d < dim * dim; d++)
                    stream >> invSigmas[i * dim * dim + d];
            }
        }

        virtual void WriteInStream (std::ostream& stream) const override
        {
            stream << squares.size();
            for (uint32_t i = 0; i < squares.size(); i++)
            {
                for (unsigned int d = 0; d < dim; d++)
                    stream << " " << squares[i].mins[d] << " " << squares[i].maxs[d];
                
                for (unsigned int d = 0; d < dim; d++)
                    stream << " " << mus[i * dim + d];
                
                for (unsigned int d = 0; d < dim * dim; d++)
                    stream << " " << invSigmas[i * dim * dim + d];
            }
        }

        virtual double eval(const double* pts) const override
        {
            double rslt = 0.0;
            for (unsigned int i = 0; i < squares.size(); i++)
            {
                bool inBox = true;
                for (unsigned int d = 0; d < dim; d++)
                    inBox = inBox && (pts[d] >= squares[i].mins[d] && pts[d] <= squares[i].maxs[d]); 

                if (inBox)
                {
                    double rslt_tmp = 0.0;
                    for (uint32_t di = 0; di < dim; di++)
                    {
                        double tmp = 0.0;
                        for (uint32_t dj = 0; dj < dim; dj++)
                            tmp += invSigmas[dj + di * dim] * (pts[dj] - mus[dj]);
                        rslt_tmp += tmp * (pts[di] - mus[di]);
                    }
                    rslt += std::exp(-0.5 * rslt_tmp);
                }
            }
            return rslt;
        }

        ~ClippedGaussianMixture()
        { }
        
    private:
        std::vector<Squares> squares;
        std::vector<double> mus;    // Note: GaussianIntegrands does not allow mu parametetrisation... 
        std::vector<double> invSigmas; // Note: GaussianIntegrands does not allow mu parametetrisation... 
    };
}