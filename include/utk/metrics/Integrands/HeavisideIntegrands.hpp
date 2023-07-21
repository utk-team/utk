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

#include <cmath>
#include "../IntegrationTest.hpp"

namespace utk
{
    class HeavisideIntegrand : public Integrand
    {
    public:
        static constexpr uint32_t MAX_CLOSE_FORM = 15;

        HeavisideIntegrand(uint32_t d) : Integrand(d) 
        { 
            r = 0.0;
            normal.resize(dim);
        }
    
        void GenerateRandom(const GenerationParameter& params, uint64_t seed) override
        {
            std::mt19937 mt(seed);
            std::uniform_real_distribution<double> posDist(0, 1);
            std::normal_distribution<double>    normalDist(0, 1);

            double norm = 0.0;
            for (uint32_t i = 0; i < dim; i++)
            {
                normal[i] = normalDist(mt);

                norm += normal[i] * normal[i];
            }

            norm = std::sqrt(norm);
            for (uint32_t i = 0; i < dim; i++)
            {
                normal[i] /= norm;
                r -= normal[i] * posDist(mt);
            }
            
        }   

        void ReadFromStream(std::istream& stream) override 
        {
            stream >> r;
            
            for (uint32_t i = 0; i < dim; i++)
                stream >> normal[i];
        }

        virtual void WriteInStream (std::ostream& stream) const override
        {
            stream << r << " ";
            
            for (uint32_t i = 0; i < dim; i++)
                stream << normal[i] << " ";
        }

        virtual double eval(const double* pts) const override
        {
            double value = r;
            for (uint32_t i = 0; i < dim; i++)
                value += normal[i] * pts[i];
            return value > 0.0;
        }

        virtual bool hasCloseForm() const override
        { 
            // Do not use close form formula for too high dimensions
            // It is exponential in dimension : (~= (D + 1)2^D)
            return (dim < MAX_CLOSE_FORM); 
        }

        virtual double getIntegral() const override 
        { 
            // https://arxiv.org/pdf/1512.07768.pdf

            double factorial = 1.0;
            double product = 1.0;

            double integralValue = 0.0;
            uint32_t vertices = ((uint32_t)1 << dim);

            for (uint32_t i = 0; i < dim; i++)
            {
                factorial *= (i + 1);
                product *= normal[i];
            }

            for (uint32_t i = 0; i < vertices; i++)
            {
                double eval = r;
                uint32_t sum = 0;
                
                uint32_t tmp = i;
                uint32_t d = 0;
                while (tmp)
                {
                    eval += normal[d++] * (tmp & 1);
                    sum += (tmp & 1);
                    tmp >>= 1;
                }

                if (eval > 0)
                {
                    const int zeros = dim - sum;
                    integralValue += (!(zeros & 1) - (zeros & 1)) * std::pow(eval, dim);
                }
                    
            }

            return integralValue / (factorial * product); 
        }

        ~HeavisideIntegrand()
        { }
        
    private:

        double r;
        std::vector<double> normal;
    };
}