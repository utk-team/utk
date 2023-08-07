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
#include <utk/utils/MetricsArgumentParser.hpp>
#include <utk/metrics/RadialSpectrum.hpp>

int main(int argc, char** argv)
{
    CLI::App app { "RadialSpectrum calculator" };
    auto* margs = utk::add_arguments(app);    
    uint32_t res = 0;
    bool cancelDc = false; 
    double scale = 0.5;
    uint32_t bins = 0;

    app.add_option("-r,--res", res, "Spectrum resolution (0 means automatic)")->default_val(res);
    app.add_flag("--canceldc", cancelDc, "When set, cancel the DC peak")->default_val(cancelDc);
    app.add_option("-s,--scale", scale, "Scale for distance to origin")->default_val(0.5);
    app.add_option("-b,--bins", bins, "Number of bins (0 means automatic)")->default_val(bins);

    CLI11_PARSE(app, argc, argv);

    auto ptss = margs->GetAllPointsets();
    if (!utk::CheckPointsets(ptss))
        return 1;
        
    auto rslts = utk::RadialSpectrum(bins, scale, res, cancelDc).compute(ptss);

    auto& ostream = margs->GetOutputStream();
    for (auto rslt : rslts)
        ostream << rslt << '\n';

    delete margs;
}