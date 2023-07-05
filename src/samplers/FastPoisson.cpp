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
#include <utk/samplers/SamplerFastPoisson.hpp>

int main(int argc, char** argv)
{
    CLI::App app { "FastPoisson sampler" };
    // NOTE : here no seed : but because the sampler does not parametrize it,
    // not because of the method
    utk::SamplerArguments* args = utk::add_arguments(app, 2, false);
    
    auto methods = utk::SamplerFastPoisson::GetMethods();
    std::ostringstream helpMethod;
    std::copy(methods.begin(), 
              methods.end(), 
              std::ostream_iterator<std::string>(helpMethod, ", ")
    );

    std::string method_ = "DartThrowing";
    double radius_ = 0.0;
    uint32_t throwMultiplier_ = 1;
    uint32_t throws_ = 1000;
    bool maximize_ = false;
    bool isTiled_ = false;
    uint32_t relaxCount_ = 0;


    app.add_option("--method", method_, "Method. [" + helpMethod.str() + "]")->default_val(method_);
    app.add_option("-r,--radius", radius_, "Radius in [0.0005, 0.2]. If outside the range, will attempt to find a radius given N.")->default_val(radius_);
    app.add_option("--throwMult", throwMultiplier_, "Multiplier for number of throw (for DartThrowing & BestCandidate methods)")->default_val(throwMultiplier_);
    app.add_option("--throws", throws_, "Number of throws ().")->default_val(throws_);
    app.add_flag("--maximize", maximize_, "Enable maximization");
    app.add_flag("--tiled", isTiled_, "Enable tiling. Might be ignored for tiled-only methods");
    app.add_flag("--relaxCount", relaxCount_, "Number of relaxation to perform")->default_val(relaxCount_);
    
    CLI11_PARSE(app, argc, argv);
    
    std::vector<utk::Pointset<double>> pts = args->GetPointsets();
    
    if (radius_ < 0.0005 || radius_ > 2.)
    {
        // Less complicated than UTK's method, but the same idea :)
        radius_ = 1.0 / (1.25 * std::sqrt(args->N));
    }

    utk::SamplerFastPoisson fp(
        method_, radius_, throwMultiplier_,
        throws_, maximize_, isTiled_, relaxCount_
    );
    for (uint32_t i = 0; i < pts.size(); i++)
    {
        if(!fp.generateSamples(pts[i], args->N))
        {
            std::cerr << "Sampler returned non-zero output" << std::endl; // No log here, must be visible whatsoever
            return 1;
        }
    }

    args->WritePointsets(pts);

    delete args;
    return 0;
}