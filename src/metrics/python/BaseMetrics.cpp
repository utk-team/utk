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
#include <pyutk/utils.hpp>

#include <utk/metrics/Diaphony.hpp>
#include <utk/metrics/BoundariesStarDiscrepancy.hpp>
#include <utk/metrics/CenteredL2Discrepancy.hpp>
#include <utk/metrics/GeneralizedL2Discrepancy.hpp>
#include <utk/metrics/L2Discrepancy.hpp>
#include <utk/metrics/SymmetricL2Discrepancy.hpp>
#include <utk/metrics/UnanchoredL2Discrepancy.hpp>

#include <utk/metrics/Spectrum.hpp>
#include <utk/metrics/RadialSpectrum.hpp>
#include <utk/metrics/PCF.hpp>
#include <utk/metrics/MinDist.hpp>

#include <utk/metrics/IntegrationTest.hpp>
#include <utk/metrics/Integrands/GaussianIntegrands.hpp>
#include <utk/metrics/Integrands/HeavisideIntegrands.hpp>
#include <utk/metrics/Integrands/BlinnPhong.hpp>


template<typename InputType, typename IntegrandType>
auto GetBuildComputeFunction()
{
    return [](IntegrationTest& inttest, const char* outname, int dim, int m, int seed, utk::GenerationParameter params, py::buffer b){
        py::buffer_info info = b.request();

        if (info.format != py::format_descriptor<InputType>::format())
            throw std::runtime_error("Type not supported");

        if (info.ndim == 2)
        {
            std::vector<py::ssize_t> desiredStride = {
                info.shape[1] * static_cast<py::ssize_t>(sizeof(InputType)), sizeof(InputType)
            };

            if (info.strides != desiredStride)
                throw std::runtime_error("Only contiguous C-ordered array are supported");
            
            Pointset<InputType> in = Pointset<InputType>::View(
                static_cast<InputType*>(info.ptr), 
                info.shape[0], info.shape[1]
            );
            inttest.BuildDatabase<IntegrandType>(outname, dim, m, seed, params, in);

            return;
        }
        
        throw std::runtime_error("Only 2D array are supported");
    };
}


void init_Metrics(py::module& m)
{
    using namespace utk;

    py::class_<Spectrum>(m, "Spectrum")
        .def(py::init<uint32_t, bool>(), py::arg("resolution") = 0, py::arg("cancelDC") = true)
        .def("setResoluion", &Spectrum::setResolution)
        .def("setCancelDC", &Spectrum::setCancelDC)
        .def("compute", GetComputeFunction<Spectrum, double, std::vector<double>, std::vector<double>>());

    py::class_<RadialSpectrum>(m, "RadialSpectrum")
        .def(py::init<uint32_t, double, uint32_t, bool>(), 
            py::arg("nbins") = 0, py::arg("scale") = 0.5, py::arg("resolution") = 0, py::arg("cancelDC") = true
        )
        .def("setResoluion", &RadialSpectrum::setResolution)
        .def("setScale", &RadialSpectrum::setScale)
        .def("setCancelDC", &RadialSpectrum::setCancelDC)
        .def("setBins", &RadialSpectrum::setBins)
        .def("compute", GetComputeFunction<RadialSpectrum, double, std::vector<double>, std::vector<double>>());
    
    py::class_<PCF>(m, "PCF")
        .def(py::init<bool, double, double, uint32_t, double>(), 
            py::arg("toroidal") = true, py::arg("rmin") = 0.01, py::arg("rmax") = 0.15, 
            py::arg("nbbins") = 200, py::arg("smoothing") = 0.001
        )
        .def("setToroidal", &PCF::setToroidal)
        .def("setRmin", &PCF::setRmin)
        .def("setRmax", &PCF::setRmax)
        .def("setSmoothing", &PCF::setSmoothing)
        .def("setNbBins", &PCF::setNbBins)
        .def("compute", GetComputeFunction<PCF, double, std::vector<double>, std::vector<double>>());

    py::class_<Diaphony>(m, "Diaphony")
        .def(py::init<>())
        .def("compute", GetComputeFunction<Diaphony, double>());


    py::class_<CenteredL2Discrepancy>(m, "CenteredL2Discrepancy")
        .def(py::init<>())
        .def("compute", GetComputeFunction<CenteredL2Discrepancy, double>());

    
    py::class_<L2Discrepancy>(m, "L2Discrepancy")
        .def(py::init<>())
        .def("compute", GetComputeFunction<L2Discrepancy, double>());

    
    py::class_<GL2Discrepancy>(m, "GL2Discrepancy")
        .def(py::init<>())
        .def("compute", GetComputeFunction<GL2Discrepancy, double>());
    

    py::class_<SymmetricL2Discrepancy>(m, "SymmetricL2Discrepancy")
        .def(py::init<>())
        .def("compute", GetComputeFunction<SymmetricL2Discrepancy, double>());


    py::class_<UnanchoredL2Discrepancy>(m, "UnanchoredL2Discrepancy")
        .def(py::init<>())
        .def("compute", GetComputeFunction<UnanchoredL2Discrepancy, double>());

    
    py::class_<BoundariesStarDiscrepancy>(m, "BoundariesStarDiscrepancy")
        .def(py::init<double>(), py::arg("eps") = -1.)
        .def("compute", GetComputeFunction<BoundariesStarDiscrepancy, double, std::pair<double, double>>());

    py::class_<MinDist>(m, "MinDistance")
        .def(py::init<double>(), py::arg("toroidal") = false)
        .def("compute", GetComputeFunction<MinDist, double>());

    // Expose error reporting
    py::class_<IntegrationTest::ErrorReport>(m, "IntegrationErrorReport")
        .def_readonly("min", &IntegrationTest::ErrorReport::min)
        .def_readonly("max", &IntegrationTest::ErrorReport::max)
        .def_readonly("mean", &IntegrationTest::ErrorReport::mean)
        .def_readonly("var", &IntegrationTest::ErrorReport::var)
        .def_readonly("count", &IntegrationTest::ErrorReport::count);


    // Not the cleanest, but pybind disallow binding same class to 
    // different name. So either IntegrationTest becomes templated
    // Or this works...
    py::class_<IntegrationTest>(m, "IntegrationTest")
        .def(py::init<>())
        .def("BuildGaussianDatabase" , GetBuildComputeFunction<double, GaussianIntegrand>())
        .def( "ReadGaussianDatabse"  , &IntegrationTest:: ReadDatabase<GaussianIntegrand>)
        .def("BuildHeavisideDatabase", GetBuildComputeFunction<double, HeavisideIntegrand>())
        .def( "ReadHeavisideDatabse" , &IntegrationTest:: ReadDatabase<HeavisideIntegrand>)
        .def("BuildBlinnPhongDatabase", GetBuildComputeFunction<double, BlinnPhong>())
        .def( "ReadBlinnPhongDatabase" , &IntegrationTest:: ReadDatabase<BlinnPhong>)
        .def("compute"      , GetComputeFunction<IntegrationTest, double, IntegrationTest::ErrorReport>());   
}
