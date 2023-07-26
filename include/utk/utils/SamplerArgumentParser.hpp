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

#include <vector>
#include <random>
#include <string>

#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>

#include <utk/utils/log.hpp>
#include <utk/utils/PointsetIO.hpp>

namespace utk
{
    struct SamplerArguments
    {
        static constexpr uint32_t ANY_DIM = 0;

        using Type = double;

        uint32_t N;
        uint32_t D;
        uint32_t M;
        uint64_t seed;

        uint32_t dumbD;
        uint64_t dumbS; 
        std::string outFile;

        constexpr static const char TOKEN_INDEX[] = "{i}";

        std::vector<Pointset<Type>> GetPointsets()
        {
            std::vector<Pointset<Type>> pts(M, Pointset<Type>(N, D));
            return pts;
        }

        std::string GetFileName(uint32_t i)
        {
            std::string filepath = outFile;
            std::string to = std::to_string(i);

            for(size_t pos = 0; (pos = filepath.find(TOKEN_INDEX, pos)) != std::string::npos; pos += to.size())
                filepath.replace(pos, sizeof(TOKEN_INDEX), to);

            return filepath;
        }

        bool WritePointsets(const std::vector<Pointset<Type>>& pts)
        {
            if (outFile.find(TOKEN_INDEX) == std::string::npos)
                return write_text_pointsets(outFile, pts);
                
            bool ret = true;
            for (uint32_t i = 0; i < pts.size(); i++)
                ret = ret && write_text_pointset(GetFileName(i), pts[i]);
            return ret;
        }
    };

    inline SamplerArguments* add_arguments(
        CLI::App& app,
        uint32_t force_dim = SamplerArguments::ANY_DIM,
        bool seedable = true
    )
    {
        SamplerArguments* args = new SamplerArguments();
        args->seed = std::random_device{}();

        app.add_option("-n", args->N, "Number of points")->required();

        if (force_dim == SamplerArguments::ANY_DIM)
        {
            app.add_option("-d", args->D, "Dimensions")->required();
        }
        else
        {
            // Leave it here so all sampler have the same arguments !
            app.add_option("-d", args->dumbD, "UNUSED ! Here for compatibility with others.");
        }

        if (seedable)
        {
            app.add_option("-s,--seed", args->seed, "Seed (unspecified means 'random')");
        }
        else
        {
            // Leave it here so all sampler have the same arguments !
            app.add_option("-s,--seed", args->dumbS, "UNUSED ! Here for compatibility with others.");
        }
            
        app.add_option("-m", args->M, "Number of pointsets")->default_val(1);
        app.add_option("-o,--out", args->outFile, "Output file (format). {i} splits outputs in multiple files and token is replaced by index.")->default_val("out.dat");

        return args;
    }
};
