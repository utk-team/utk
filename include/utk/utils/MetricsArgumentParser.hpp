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
#include <vector>
#include <fstream>
#include <limits>

namespace utk
{
    template<typename Type = OPENMP_DOUBLE_DEFAULT>
    struct MetricArguments
    {
        std::string outFile;
        std::vector<std::string> inFile;

        std::ofstream outputFileStream;

        std::vector<Pointset<Type>> GetAllPointsets()
        {
            std::vector<Pointset<Type>> pointsets;
            for (const std::string& file : inFile)
            {
                std::vector<Pointset<Type>> ptsFile = read_pointsets<Type>(file.c_str());
                pointsets.insert(
                    std::end(pointsets), 
                    std::begin(ptsFile), 
                    std::end(ptsFile)
                );
            }
            return pointsets;
        } 

        std::ostream& GetOutputStream()
        {
            if (outFile.empty())
            {
                std::cout << std::setprecision(std::numeric_limits<Type>::digits10 + 2) << std::fixed;
                return std::cout;
            }
            
            outputFileStream.open(outFile);
            outputFileStream << std::setprecision(std::numeric_limits<Type>::digits10 + 2) << std::fixed;
            return outputFileStream;
        }
    };

    template<typename Type = OPENMP_DOUBLE_DEFAULT>
    MetricArguments<Type>* add_arguments(CLI::App& app)
    {
        MetricArguments<Type>* arguments = new MetricArguments<Type>;

        app.add_option("-i,--input", arguments->inFile, "Input file(s)")->required()->check(CLI::ExistingFile);
        app.add_option("-o,--output", arguments->outFile, "Output file (empty is stdout)")->default_val("");
        app.add_flag_callback("--silent", [](){UTK_LOG_DISABLE();}, "Silence UTK logs");

        return arguments;
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