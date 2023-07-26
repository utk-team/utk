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

#ifdef PYUTK_LLOYD 
    #include <utk/samplers/SamplerLloyd.hpp>
    
    void init_Lloyd(py::module& m)
    {
        // TODO
    }
#else 
    void init_Lloyd(py::module& m)
    {
        // Empty, do not compile anything
    }
#endif

#ifdef PYUTK_BNOT
    #include <utk/samplers/SamplerBoundedBNOT.hpp>
    #include <utk/samplers/SamplerPeriodicBNOT.hpp>

    void init_BNOT(py::module& m)
    {
        using namespace utk;

        py::class_<SamplerBoundedBNOT>(m, "BBNOT")
            .def(py::init<uint32_t, uint32_t, double>(), py::arg("it") = 20, py::arg("newtonit") = 20, py::arg("eps") = 0.2)
            .def("__repr__", [](const SamplerPeriodicBNOT& wn) { return "BBNOT()"; })
            .def("setIterations", &SamplerBoundedBNOT::setIterations)
            .def("setNewtonIterations", &SamplerBoundedBNOT::setNewtonIterations)
            .def("setEpsilon", &SamplerBoundedBNOT::setEpsilon)
            .def("setSeed", GetSetSeedFunction<SamplerBoundedBNOT>(), py::arg("seed") = NO_SEED)
            .def("sample",  GetSampleFunction <SamplerBoundedBNOT>(), py::arg("N"));

        py::class_<SamplerPeriodicBNOT>(m, "PBNOT")
            .def(py::init<uint32_t, uint32_t, double>(), py::arg("it") = 20, py::arg("newtonit") = 20, py::arg("eps") = 0.2)
            .def("__repr__", [](const SamplerPeriodicBNOT& wn) { return "BBNOT()"; })
            .def("setIterations", &SamplerPeriodicBNOT::setIterations)
            .def("setNewtonIterations", &SamplerPeriodicBNOT::setNewtonIterations)
            .def("setEpsilon", &SamplerPeriodicBNOT::setEpsilon)
            .def("setSeed", GetSetSeedFunction<SamplerPeriodicBNOT>(), py::arg("seed") = NO_SEED)
            .def("sample",  GetSampleFunction <SamplerPeriodicBNOT>(), py::arg("N"));   
    }
#else
    void init_BNOT(py::module& m)
    {
        // Empty, do not compile anything
    }
#endif

void init_Externals(py::module& m)
{
    init_Lloyd(m);
    init_BNOT(m);
}