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
#include <utk/metrics/IntegrationTest.hpp>
#include <utk/metrics/Integrands/ClippedGaussianMixture.hpp>

#include <utk/utils/MetricsArgumentParser.hpp>

int main(int argc, char** argv)
{
    // This is a developpent file, it is not meant to be part of main API ! 
    // It is left as a tool to plot UTK integrands.
    CLI::App app { "Integrand grid evaluator" };

    for (unsigned int i = 0; i < 16; i++)
    {
        utk::ClippedGaussianMixture integrand(2);
        integrand.GenerateRandom({
            {"smin" , utk::ParameterType(0.01)},
            {"smax" , utk::ParameterType(0.05)},
            {"nbmin", utk::ParameterType((uint32_t)10)},
            {"nbmax", utk::ParameterType((uint32_t)40)},
        }, std::random_device{}());

        unsigned int n = 100;

        std::vector<double> values(n * n, 0.);

        double nstep = 1. / static_cast<double>(n);
        for (unsigned int i = 0; i < n; i++)
        {
            for (unsigned int j = 0; j < n; j++)
            {
                double arr[2] = {i * nstep, j * nstep};
                values[j + i * n] = integrand.eval(arr);
            }
        }

        std::ofstream stream("values_" + std::to_string(i) + ".dat");
        for (unsigned int k = 0; k < values.size(); k++)
            stream << values[k] << "\n";
    }

    return 0;
}