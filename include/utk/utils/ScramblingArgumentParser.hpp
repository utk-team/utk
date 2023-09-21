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

#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>

#include <utk/utils/Omp.hpp>
#include <utk/utils/Log.hpp>
#include <utk/utils/PointsetIO.hpp>

#include <iomanip>
#include <random>
#include <vector>
#include <fstream>
#include <limits>

namespace utk
{
    template<typename INType = OPENMP_DOUBLE_DEFAULT, typename OUTType = OPENMP_DOUBLE_DEFAULT>
    struct ScramblingArgument
    {
        // Provide access to 'ouside class'
        using InputType = INType;
        using OutputType = OUTType;

        constexpr static const char TOKEN_INDEX[] = "{i}";

        uint64_t seed;
        std::string outFile;
        std::vector<std::string> inFile;

        std::ofstream outputFileStream;

        std::vector<Pointset<INType>> GetAllPointsets()
        {
            std::vector<Pointset<INType>> pointsets;
            for (const std::string& file : inFile)
            {
                std::vector<Pointset<INType>> ptsFile = read_text_pointset<INType>(file.c_str());
                pointsets.insert(
                    std::end(pointsets), 
                    std::begin(ptsFile), 
                    std::end(ptsFile)
                );
            }
            return pointsets;
        } 

        std::string GetFileName(uint32_t i)
        {
            std::string filepath = outFile;
            std::string to = std::to_string(i);

            for(size_t pos = 0; (pos = filepath.find(TOKEN_INDEX, pos)) != std::string::npos; pos += to.size())
                filepath.replace(pos, sizeof(TOKEN_INDEX), to);

            return filepath;
        }

        bool WritePointsets(const std::vector<Pointset<OUTType>>& pts)
        {
            if (outFile.find(TOKEN_INDEX) == std::string::npos)
                return write_text_pointsets(outFile, pts);
                
            bool ret = true;
            for (uint32_t i = 0; i < pts.size(); i++)
                ret = ret && write_text_pointset(GetFileName(i), pts[i]);
            return ret;
        }
    };

    template<typename INType = std::uint32_t, typename OUTType = OPENMP_DOUBLE_DEFAULT>
    ScramblingArgument<INType, OUTType>* add_arguments(CLI::App& app)
    {
        ScramblingArgument<INType, OUTType>* args = new ScramblingArgument<INType, OUTType>;

        args->seed = std::random_device{}();
        app.add_option("-i,--input", args->inFile, "Input file(s)")->required()->check(CLI::ExistingFile);        
        app.add_option("-o,--out", args->outFile, "Output file (format). {i} splits outputs in multiple files and token is replaced by index.")->default_val("out.dat");
        app.add_option("-s,--seed", args->seed, "Seed (unspecified means 'random')");
        app.add_flag_callback("--silent", [](){UTK_LOG_DISABLE();}, "Silence UTK logs");

        return args;
    }

    template<typename T>
    bool CheckPointsets(const std::vector<Pointset<T>>& pts)
    {
        // Can't say it's wrong ...
        if (pts.size() == 0) 
        {
            UTK_WARN("No pointset read...", 0);
            return true;
        }

        bool isValid = true;
        uint32_t N = pts[0].Npts();
        uint32_t D = pts[0].Ndim();

        for (uint32_t i = 1; i < pts.size(); i++)
        {
            if (N != pts[i].Npts() || D != pts[i].Ndim())
            {
                UTK_WARN("Pointset shape mismatch: [0]: {}, {}, [{}]: {}, {}", N, D, i, pts[i].Npts(), pts[i].Ndim());
                isValid = false;
            }
        }
        return isValid;
    }
}