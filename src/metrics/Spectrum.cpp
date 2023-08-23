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
#include <utk/utils/Omp.hpp>
#include <utk/utils/MetricsArgumentParser.hpp>
#include <utk/metrics/Spectrum.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

int main(int argc, char** argv)
{
    CLI::App app { "Spectrum calculator" };
    auto* margs = utk::add_arguments(app);
    std::string imgFile = "";
    uint32_t res = 0;
    bool cancelDc = false; 


    app.add_option("-r,--res", res, "Spectrum resolution (0 means automatic)")->default_val(res);
    app.add_flag("--canceldc", cancelDc, "When set, cancel the DC peak")->default_val(cancelDc);
    app.add_option("--img", imgFile, "When set, output PNG image of spectrum. Only available in 2D.")->default_val(imgFile);

    CLI11_PARSE(app, argc, argv);

    auto ptss = margs->GetAllPointsets();
    if (!utk::CheckPointsets(ptss))
        return 1;

    if (ptss.size() == 0)
        return 0;
    
    auto rslts = utk::Spectrum(res, cancelDc).compute(ptss);

    auto& ostream = margs->GetOutputStream();
    for (auto rslt : rslts)
        ostream << rslt << '\n';

    // Write PNG image
    if (!imgFile.empty())
    {
        res = std::round(std::sqrt(rslts.size()));
        uint32_t D = ptss[0].Ndim();
        if (D != 2)
        {
            UTK_WARN("Output image is only available for 2D pointsets. Found {}D pointsets", D);
            return 0;
        }

        unsigned char* data = new unsigned char[res * res];    
        double maxval = omp_parallel_max(rslts.data(), rslts.size());

        // Copy data to unsigned char buffer
        #pragma omp parallel for
        for (OPENMP_UINT y = 0; y < res; y++)
        {
            for (OPENMP_UINT x = 0; x < res; x++)
            {
                const OPENMP_UINT idx = x + y * res;

                double value = rslts[idx] / maxval;
                data[idx] = static_cast<unsigned char>(255 * value);
            }
        }

        // Write image
        stbi_write_png(imgFile.c_str(), res, res, 1, data, res * sizeof(unsigned char));

        delete[] data;
    }
    
    delete margs;
}
