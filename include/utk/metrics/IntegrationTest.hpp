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

#ifdef UTK_USE_OPENMP
    #include <omp.h>
#endif

#include <iomanip>
#include <fstream>
#include <ostream>
#include <istream>
#include <sstream>
#include <cmath>
#include <random>
#include <vector>
#include <map>
#include <variant>

#include <utk/utils/log.hpp>
#include <utk/utils/Pointset.hpp>
#include <utk/utils/utk_types.hpp>

namespace utk
{
    using ParameterType = std::variant<int, double, std::string>;
    using GenerationParameter = std::map<std::string, ParameterType>;

    class Integrand
    {
    public:
        Integrand(uint32_t d) : dim(d) { }

        virtual void GenerateRandom(const GenerationParameter& params, uint64_t seed) = 0;
        virtual void ReadFromStream(std::istream& stream) = 0;
        virtual void WriteInStream (std::ostream& stream) const = 0;

        virtual double eval(const double* pts) const = 0;

        virtual bool hasCloseForm() const  { return false; }
        virtual double getIntegral() const { return 0.; }

        virtual ~Integrand() { }
    protected:
        uint32_t dim;
    };

    class IntegrationTest
    {
    public:
        IntegrationTest()
        { }
        
        struct ErrorReport
        {
            double min = std::numeric_limits<double>::max();
            double max = std::numeric_limits<double>::min();
            double mean = 0.0;
            double var = 0.0;
            uint32_t count = 0;
        };


        // T : integrand type, Args: parameters for integrand 
        template<typename IntegrandType>
        void BuildDatabase(
            const std::string& file,           /* Output file                         */
            uint32_t dim,                      /* Dimension                           */
            uint32_t count,                    /* Number of integrands to generate    */
            uint64_t seed,                     /* Seed for randomness                 */
            const GenerationParameter& params, /* Parameters for integrand generation */
            const Pointset<double>& pointsets  /* to compute integral                 */)
        {     
            integrands.resize(count);
            values.resize(count);

            #pragma omp parallel for
            for (OPENMP_UINT i = 0; i < count; i++)
            {
                integrands[i] = new IntegrandType(dim);
                integrands[i]->GenerateRandom(params, seed + i);

                if (integrands[i]->hasCloseForm())
                    values[i] = integrands[i]->getIntegral();
                else
                    values[i] = Evaluate(pointsets, integrands[i]);
            }

            std::ofstream f(file);
            f << std::setprecision(std::numeric_limits<double>::digits10 + 2);

            for (uint32_t i = 0; i < count; i++)
            {
                f << values[i] << " ";
                integrands[i]->WriteInStream(f);
                f << '\n';
            }
        }

        template<typename IntegrandType>
        void ReadDatabase(const std::string& file, uint32_t D)
        {
            integrands.resize(0);
            values.resize(0);

            std::ifstream rfile(file);

            if (!rfile.is_open())
            {
                UTK_WARN("File {} does not exists. No integrands loaded", file);
                return;
            }

            std::string buffer;
            while (std::getline(rfile, buffer))
            {
                std::istringstream f(buffer);
                double value; 
                f >> value;
                
                IntegrandType* integrand = new IntegrandType(D);
                integrand->ReadFromStream(f);

                integrands.push_back(integrand);
                values.push_back(value);
            }
        }

        double Evaluate(const Pointset<double>& pts, Integrand* integrand) const
        {
            double contribs = 0.0;
            for (uint32_t i = 0; i < pts.Npts(); i++)
                contribs += integrand->eval(pts[i]);
            return contribs / pts.Npts();
        }

        ErrorReport compute(const Pointset<double>& pts) const
        {
            if (integrands.size() == 0)
            {
                UTK_ERROR("No integrand. Load a database first or build it", 0);
                return ErrorReport{};
            }

            std::vector<ErrorReport> errors;

            #pragma omp parallel
            {
                #ifdef UTK_USE_OPENMP
                    #pragma omp single
                    {
                        errors.resize(omp_get_num_threads());
                    }
                #else
                    errors.resize(1);
                #endif
    
                #pragma omp for
                for (OPENMP_UINT i = 0; i < integrands.size(); i++)
                {
                    #ifdef UTK_USE_OPENMP
                        uint32_t tId = omp_get_thread_num();
                    #else
                        uint32_t tId = 0;
                    #endif
                
                    double value = Evaluate(pts, integrands[i]);
                    double error = (value - values[i]) * (value - values[i]);

                    errors[tId].count ++;
                    errors[tId].min = std::min(errors[tId].min, error);
                    errors[tId].max = std::max(errors[tId].max, error);

                    double delta  = error - errors[tId].mean;
                    errors[tId].mean += delta / errors[tId].count;
                    double delta2 = error - errors[tId].mean;

                    errors[tId].var += delta * delta2; 
                }
            }
            
            ErrorReport report = errors[0];
            for (uint32_t i = 1; i < errors.size(); i++)
            {
                uint32_t newCount = report.count + errors[i].count;
                double delta = report.mean - errors[i].mean;
                
                report.mean = (report.mean * report.count +  errors[i].mean * errors[i].count) / newCount;
                report.min = std::min(report.min, errors[i].min);
                report.max = std::max(report.max, errors[i].max);

                // Algorithms for computing the sample variance, Chan 1983
                report.var = (
                    report.var + errors[i].var + 
                    (delta * delta * report.count * errors[i].count) / newCount
                );
                report.count = newCount;
            }
            report.var /= (report.count - 1);
            return report;
        }

        std::vector<ErrorReport> compute(const std::vector<Pointset<double>>& ptss) const
        {
            if (integrands.size() == 0)
            {
                // UTK_ERROR("No integrand. Load a database first or build it", 0);
                return std::vector<ErrorReport>(ptss.size(), ErrorReport{});
            }

            std::vector<ErrorReport> reports;
            reports.reserve(ptss.size());

            for (const auto& pts : ptss)
                reports.push_back(compute(pts));
            return reports;
        }

        ~IntegrationTest()
        {
            for (const auto ptr : integrands)
                delete ptr;
        }

        const std::vector<Integrand*>& getIntegrands() const
        {
            return integrands;
        }
    private:
        std::vector<Integrand*> integrands;
        std::vector<double>     values;
        std::string file;
    };
};