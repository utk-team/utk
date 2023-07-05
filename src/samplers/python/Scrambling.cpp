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

#include <utk/scrambling/Owen.hpp>
#include <utk/scrambling/OwenTreeScrambling.hpp>
#include <utk/scrambling/CranleyPatterson.hpp>
#include <utk/scrambling/DigitalShift.hpp>
#include <utk/scrambling/MicroJittering.hpp>

void init_Scrambling(py::module& m)
{
    using IntegerType = uint32_t;
    using OwenScrambling = ScramblingOwen<IntegerType>;
    using OwenTreeScramblingI = OwenTreeScrambling<IntegerType>;
    using DigitalShiftScramplingI = DigitalShift<IntegerType>;

    py::class_<OwenScrambling>(m, "OwenScrambling")
        .def(py::init<uint32_t>(), py::arg("depth") = 32)
        .def("setOwenDepth", &OwenScrambling::setOwenDepth)
        .def("setSeed", GetSetSeedFunction<OwenScrambling>(), py::arg("seed") = NO_SEED)
        .def("scramble",  GetScrambleFunction<OwenScrambling, IntegerType, double>())
        .def("iscramble", GetScrambleFunction<OwenScrambling, IntegerType, IntegerType>());
    
    py::class_<CranleyPattersonScrambling>(m, "CranleyPattersonScrambling")
        .def(py::init<double, double>(), py::arg("max") = 1.0, py::arg("domain") = 1.0)
        .def("setSeed" ,  GetSetSeedFunction <CranleyPattersonScrambling>(), py::arg("seed") = NO_SEED)
        .def("scramble",  GetScrambleFunction<CranleyPattersonScrambling, double, double>());

    py::class_<DigitalShiftScramplingI>(m, "DigitalShift")
        .def(py::init<>())
        .def("setSeed" ,  GetSetSeedFunction <DigitalShiftScramplingI>(), py::arg("seed") = NO_SEED)
        .def("scramble",  GetScrambleFunction<DigitalShiftScramplingI, IntegerType, double>());

    py::class_<MicroJittering>(m, "MicroJittering")
        .def(py::init<double>(), py::arg("intensity") = -1.0)
        .def("setSeed" ,  GetSetSeedFunction <MicroJittering>(), py::arg("seed") = NO_SEED)
        .def("scramble",  GetScrambleFunction<MicroJittering, double, double>());

    
    py::class_<OwenTreeScramblingI>(m, "OwenTreeScrambling")
        .def(py::init<uint32_t>(), py::arg("depth") = 32)
        .def("setOwenDepth", &OwenTreeScramblingI::setOwenDepth)
        .def("setSeed", GetSetSeedFunction<OwenTreeScramblingI>(), py::arg("seed") = NO_SEED)
        .def("setBitPattern", &OwenTreeScramblingI::setBitPattern)
        .def("fillRandom", &OwenTreeScramblingI::FillRandom, py::arg("dim"), py::arg("depth") = 0)
        .def("scramble",  GetScrambleFunction<OwenTreeScramblingI, IntegerType, double>())
        .def("iscramble", GetScrambleFunction<OwenTreeScramblingI, IntegerType, IntegerType>());
}