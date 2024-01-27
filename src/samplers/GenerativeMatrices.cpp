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
#include <utk/utils/SamplerArgumentParser.hpp>
#include <utk/samplers/SamplerGenerativeMatrices.hpp>

int main(int argc, char** argv)
{
    CLI::App app { "Generative Matrix sampler" };
    auto* args = utk::add_arguments(app);
    
    std::string file;
    int size, base;
    bool scramble;
    app.add_option("--file" , file, "Matrices file");
    app.add_flag("--scramble", scramble, "Owen scrambling depth")->default_val(false);
    app.add_option("--base" , base, "Base of matrices");
    app.add_option("--size" , size, "Size of matrices");
    
    CLI11_PARSE(app, argc, argv);
    
    std::vector<utk::Pointset<double>> pts = args->GetPointsets();
    
    utk::SamplerGenerativeMatrices<uint32_t> sampler(file, size, base, args->D, scramble);

    for (uint32_t i = 0; i < pts.size(); i++)
    {
        if(!sampler.generateSamples(pts[i], args->N))
        {
            std::cerr << "Sampler returned non-zero output" << std::endl; // No log here, must be visible whatsoever
            return 1;
        }
    }

    args->WritePointsets(pts);

    delete args;
    return 0;
}