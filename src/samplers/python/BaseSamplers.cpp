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

#include <utk/samplers/SamplerWhitenoise.hpp>
#include <utk/samplers/SamplerHalton.hpp>
#include <utk/samplers/SamplerHammersley.hpp>
#include <utk/samplers/SamplerRegularGrid.hpp>
#include <utk/samplers/SamplerStratified.hpp>
#include <utk/samplers/SamplerHexagonalGrid.hpp>
#include <utk/samplers/SamplerNRooks.hpp>
#include <utk/samplers/SamplerCMJ.hpp>
#include <utk/samplers/SamplerFaure.hpp>
#include <utk/samplers/SamplerNiederreiter.hpp>
#include <utk/samplers/SamplerLutLDBN.hpp>
#include <utk/samplers/SamplerPMJ.hpp>
#include <utk/samplers/SamplerDartThrowing.hpp>
#include <utk/samplers/SamplerBridson.hpp>
#include <utk/samplers/SamplerProjectiveBlueNoise.hpp>
#include <utk/samplers/SamplerSobol.hpp>
#include <utk/samplers/SamplerCascadedSobol.hpp>
#include <utk/samplers/SamplerFromGenerativeMatrix.hpp>
#include <utk/samplers/SamplerR1.hpp>
#include <utk/samplers/SamplerKronecker.hpp>
#include <utk/samplers/SamplerKorobov.hpp>
#include <utk/samplers/SamplerAAPatterns.hpp>
#include <utk/samplers/SamplerART.hpp>
#include <utk/samplers/SamplerPenrose.hpp>
#include <utk/samplers/SamplerFastPoisson.hpp>
#include <utk/samplers/SamplerFattal.hpp>
#include <utk/samplers/SamplerForcedRandom.hpp>
#include <utk/samplers/SamplerStep.hpp>
#include <utk/samplers/SamplerSinglePeak.hpp>

namespace utk
{
    // Global constants for various samplers
    std::vector<uint32_t> HALTON_BASIS; 
    std::vector<double>   KRONECKER_ALPHAS;
};

void init_BaseSampler(py::module& m)
{
    py::class_<SamplerWhitenoise>(m, "Whitenoise")
        .def(py::init<uint32_t>(), py::arg("d"))
        .def("__repr__", [](const SamplerWhitenoise& wn) { return "Whitenoise(d=" + std::to_string(wn.GetDimension()) +")"; })
        .def("setSeed", GetSetSeedFunction<SamplerWhitenoise>(), py::arg("seed") = NO_SEED)
        .def("sample",  GetSampleFunction <SamplerWhitenoise>(), py::arg("N"));

    py::class_<SamplerHalton>(m, "Halton")
        .def(py::init<uint32_t>(), py::arg("d"))
        .def("__repr__", [](const SamplerHalton& wn) { return "Halton(d=" + std::to_string(wn.GetDimension()) +")"; })
        .def("setBasisList", [](SamplerHalton& ht, const std::vector<uint32_t>& list)
        {
            if (list.size() < ht.GetDimension())
                throw std::runtime_error("Basis list should be greater than sampler dimension");

            HALTON_BASIS = list;
            ht.setBasisList(HALTON_BASIS.data(), list.size());
        })
        .def("sample",  GetSampleFunction <SamplerHalton>()          , py::arg("N"))
        .def("isample", GetSampleFunction <SamplerHalton, uint32_t>(), py::arg("N"));

    py::class_<SamplerHammersley>(m, "Hammersley")
        .def(py::init<>())
        .def("__repr__", [](const SamplerHammersley& wn) { return "Hammersley()"; })
        .def("sample",  GetSampleFunction <SamplerHammersley>()          , py::arg("N"))
        .def("isample", GetSampleFunction <SamplerHammersley, uint32_t>(), py::arg("N"));

    py::class_<SamplerRegularGrid>(m, "RegularGrid")
        .def(py::init<uint32_t>(), py::arg("d"))
        .def("__repr__", [](const SamplerRegularGrid& wn) { return "RegularGrid(d=" + std::to_string(wn.GetDimension()) +")"; })
        .def("sample",  GetSampleFunction <SamplerRegularGrid>(), py::arg("N"));

    py::class_<SamplerStratified>(m, "Stratified")
        .def(py::init<uint32_t>(), py::arg("d"))
        .def("__repr__", [](const SamplerStratified& wn) { return "Stratified(d=" + std::to_string(wn.GetDimension()) +")"; })
        .def("setSeed", GetSetSeedFunction<SamplerStratified>(), py::arg("seed") = NO_SEED)
        .def("sample",  GetSampleFunction <SamplerStratified>(), py::arg("N"));

    py::class_<SamplerHexagonalGrid>(m, "HexGrid")
        .def(py::init<>())
        .def("__repr__", [](const SamplerHexagonalGrid& wn) { return "HexGrid()"; })
        .def("sample",  GetSampleFunction <SamplerHexagonalGrid>() , py::arg("N"));

    py::class_<SamplerCMJ>(m, "CMJ")
        .def(py::init<>())
        .def("__repr__", [](const SamplerCMJ& wn) { return "CMJ()"; })
        .def("setSeed", GetSetSeedFunction<SamplerCMJ>(), py::arg("seed") = NO_SEED)
        .def("sample",  GetSampleFunction <SamplerCMJ>() , py::arg("N"));

    py::class_<SamplerFaure>(m, "Faure")
        .def(py::init<uint32_t>(), py::arg("d"))
        .def("__repr__", [](const SamplerFaure& wn) { return "Faure(d=" + std::to_string(wn.GetDimension()) +")"; })
        .def("sample",  GetSampleFunction <SamplerFaure>()          , py::arg("N"))
        .def("isample", GetSampleFunction <SamplerFaure, uint32_t>(), py::arg("N"));

    py::class_<SamplerNRooks>(m, "NRooks")
        .def(py::init<uint32_t>(), py::arg("d"))
        .def("__repr__", [](const SamplerNRooks& wn) { return "NRooks(d=" + std::to_string(wn.GetDimension()) +")"; })
        .def("setSeed", GetSetSeedFunction<SamplerNRooks>(), py::arg("seed") = NO_SEED)
        .def("sample",  GetSampleFunction <SamplerNRooks>(), py::arg("N"));

    py::class_<SamplerNiederreiter>(m, "Niederreiter")
        .def(py::init<uint32_t, uint32_t>(), py::arg("d"), py::arg("b") = 2)
        .def("__repr__", [](const SamplerNiederreiter& wn) { return "Niederreiter(d=" + std::to_string(wn.GetDimension()) +")"; })
        .def("setBasis", &SamplerNiederreiter::setBasis)
        .def("sample",  GetSampleFunction <SamplerNiederreiter>()          , py::arg("N"))
        .def("isample", GetSampleFunction <SamplerNiederreiter, uint32_t>(), py::arg("N"));

    py::class_<SamplerLutLDBN>(m, "LDBN")
        .def(py::init<>())
        .def("__repr__", [](const SamplerLutLDBN& wn) { return "LDBN()"; })
        .def_static("getAvailableTargets", &SamplerLutLDBN::GetTargets)
        .def("setTarget", &SamplerLutLDBN::setTarget)
        .def("sample",  GetSampleFunction <SamplerLutLDBN>()          , py::arg("N"))
        .def("isample", GetSampleFunction <SamplerLutLDBN, uint32_t>(), py::arg("N"));
    
    py::class_<SamplerPMJ>(m, "PMJ")
        .def(py::init<const std::string&, uint32_t>(), py::arg("method") = "PMJ02", py::arg("nbCandidates") = 10)
        .def("__repr__", [](const SamplerPMJ& wn) { return "PMJ()"; })
        .def_static("getAvailableMethods", &SamplerPMJ::GetMethods)
        .def("setMethod", &SamplerPMJ::setMethod)
        .def("setNbCandidates", &SamplerPMJ::setCandidates)
        .def("setSeed", GetSetSeedFunction<SamplerPMJ>(), py::arg("seed") = NO_SEED)
        .def("sample",  GetSampleFunction <SamplerPMJ>(), py::arg("N"));

    py::class_<SamplerDartThrowing>(m, "DartThrowing")
        .def(py::init<
                uint32_t, bool, bool,
                uint32_t, double, double
            >(), 
                py::arg("d"), py::arg("relaxed") = true, py::arg("toroidal") = true,
                py::arg("trials") = 1000, py::arg("relaxedFactor") = 0.9, py::arg("distance") = -1.0
        )
        .def("__repr__", [](const SamplerDartThrowing& wn) { return "DartThrowing(d=" + std::to_string(wn.GetDimension()) +")"; })
        .def("setRelaxed" ,      &SamplerDartThrowing::setRelaxed)
        .def("setToroidal",      &SamplerDartThrowing::setToroidal)
        .def("setMaxTrials",     &SamplerDartThrowing::setMaxIters)
        .def("setRelaxedFactor", &SamplerDartThrowing::setRelaxedFactor)
        .def("setDistance"     , &SamplerDartThrowing::setDistance, py::arg("distance") = -1.0)
        .def("setSeed", GetSetSeedFunction<SamplerDartThrowing>(), py::arg("seed") = NO_SEED)
        .def("sample",  GetSampleFunction <SamplerDartThrowing>(), py::arg("N"));

     py::class_<SamplerBridson>(m, "Bridson")
        .def(py::init<
                uint32_t, bool, 
                uint32_t, double
            >(), 
                py::arg("d"), py::arg("toroidal") = true,
                py::arg("candidates") = 30, py::arg("distance") = -1.0
        )
        .def("__repr__", [](const SamplerBridson& wn) { return "Bridson(d=" + std::to_string(wn.GetDimension()) +")"; })
        .def("setToroidal"  , &SamplerBridson::setToroidal)
        .def("setCandidates", &SamplerBridson::setCandidates)
        .def("setDistance"  , &SamplerBridson::setDistance, py::arg("distance") = -1.0)
        .def("setSeed", GetSetSeedFunction<SamplerBridson>(), py::arg("seed") = NO_SEED)
        .def("sample",  GetSampleFunction <SamplerBridson>(), py::arg("N"));

    py::class_<SamplerProjectiveBlueNoise>(m, "ProjectiveBlueNoise")
        .def(py::init<
                uint32_t, bool, bool,
                uint32_t, double
            >(), 
                py::arg("d"), py::arg("relaxed") = true, py::arg("toroidal") = true,
                py::arg("trials") = 1000, py::arg("relaxedFactor") = 0.9
        )
        .def("__repr__", [](const SamplerProjectiveBlueNoise& wn) { return "ProjectiveBlueNoise(d=" + std::to_string(wn.GetDimension()) +")"; })
        .def("setRelaxed" ,      &SamplerProjectiveBlueNoise::setRelaxed)
        .def("setToroidal",      &SamplerProjectiveBlueNoise::setToroidal)
        .def("setMaxTrials",     &SamplerProjectiveBlueNoise::setMaxIters)
        .def("setRelaxedFactor", &SamplerProjectiveBlueNoise::setRelaxedFactor)
        .def("setSeed", GetSetSeedFunction<SamplerProjectiveBlueNoise>(), py::arg("seed") = NO_SEED)
        .def("sample",  GetSampleFunction <SamplerProjectiveBlueNoise>(), py::arg("N"));

    py::class_<SamplerFastPoisson>(m, "FastPoisson")
        .def(py::init<
                const std::string&, double, 
                uint32_t, uint32_t, 
                bool, bool, uint32_t 
            >(), 
                py::arg("method") = "DartThrowing", py::arg("radius") = 0.1, 
                py::arg("throwMultiplier") = 1, py::arg("throws") = 1000,
                py::arg("maximize") = false, py::arg("tiled") = false,
                py::arg("relaxCount") = 0
        )
        .def("__repr__", [](const SamplerFastPoisson& wn) { return "FastPoisson()"; })
        .def_static("getAvailableMethods", &SamplerFastPoisson::GetMethods)
        .def("setMethod" ,         &SamplerFastPoisson::setMethod)
        .def("setRadius",          &SamplerFastPoisson::setRadius)
        .def("setThrowMultiplier", &SamplerFastPoisson::setThrowMultiplier)
        .def("setThrows",          &SamplerFastPoisson::setThrows)
        .def("setMaximize",        &SamplerFastPoisson::setMaximize)
        .def("setTiled",           &SamplerFastPoisson::setTiled)
        .def("setRelaxCount",      &SamplerFastPoisson::setRelaxCount)
        .def("sample",  GetSampleFunction <SamplerFastPoisson>(), py::arg("N"));

    py::class_<SamplerRank1>(m, "Rank1")
        .def(py::init<bool, bool>(), py::arg("exhaustiveSearch") = false, py::arg("fibonnaci") = false)
        .def("__repr__", [](const SamplerRank1& wn) { return "Rank1()"; })
        .def("setExhaustiveSearch", &SamplerRank1::setExhaustSearch)
        .def("setFibonnacci"      , &SamplerRank1::setFibonnacci)
        .def("sample",  GetSampleFunction <SamplerRank1>(), py::arg("N"));

    py::class_<SamplerKorobov>(m, "Korobov")
        .def(py::init<uint32_t, uint32_t>(), py::arg("d"), py::arg("generator") = 3)
        .def("__repr__", [](const SamplerKorobov& wn) {  return "Korobov(d=" + std::to_string(wn.GetDimension()) +")"; })
        .def("setGenerator", &SamplerKorobov::setGeneratorA)
        .def("sample",  GetSampleFunction <SamplerKorobov>(), py::arg("N"));

    py::class_<SamplerKronecker>(m, "Kronecker")
        .def(py::init<uint32_t>(), py::arg("d"))
        .def("__repr__", [](const SamplerKronecker& wn) {  return "Kronecker(d=" + std::to_string(wn.GetDimension()) +")"; })
        .def("setAlphas", [](SamplerKronecker& kronecker, const std::vector<double>& alphas)
        {
            if (alphas.size() < kronecker.GetDimension())
                throw std::runtime_error("Alphas should have a greater count than sampler");    

            KRONECKER_ALPHAS = alphas;
            kronecker.setAlphas(KRONECKER_ALPHAS.data());
        })
        .def("setAlphas", [](SamplerKronecker& kronecker, const std::string& name)
        {
            const double* alphas = SamplerKronecker::ParseAlpha(name, kronecker.GetDimension());
            if (alphas == nullptr)
                throw std::runtime_error("Alphas do not match dimension");
            
            kronecker.setAlphas(alphas);
        })
        .def_static("getAvailableAlphasNames", &SamplerKronecker::GetAlphas)
        .def("sample",  GetSampleFunction <SamplerKronecker>(), py::arg("N"));

    // TODO : Kronecker

    py::class_<SamplerAAPatterns>(m, "AAPatterns")
        .def(py::init<>())
        .def("__repr__", [](const SamplerAAPatterns& wn) { return "AAPatterns()"; })
        .def_static("getAvailableTiling", &SamplerAAPatterns::GetDefaultFiles)
        .def("setVectorFile", &SamplerAAPatterns::setVectorFile, py::arg("file"))
        .def("setSeed", GetSetSeedFunction<SamplerAAPatterns>(), py::arg("seed") = NO_SEED)
        .def("sample",  GetSampleFunction <SamplerAAPatterns>(), py::arg("N"));

    py::class_<SamplerART>(m, "ART")
        .def(py::init<>())
        .def("__repr__", [](const SamplerART& wn) { return "ART()"; })
        .def("sample",  GetSampleFunction <SamplerAAPatterns>(), py::arg("N"));

    py::class_<SamplerPenrose>(m, "Penrose")
        .def(py::init<>())
        .def("__repr__", [](const SamplerPenrose& wn) { return "Penrose()"; })
        .def("sample",  GetSampleFunction <SamplerAAPatterns>(), py::arg("N"));

    
    py::class_<SamplerFattal>(m, "Fattal")
        .def(py::init<double, uint32_t>(), py::arg("temperature") = 0.5, py::arg("level") = 2)
        .def("__repr__", [](const SamplerFattal& wn) { return "Fattal()"; })
        .def("setTemperature", &SamplerFattal::setTemperature)
        .def("setLevel"      , &SamplerFattal::setLevel)
        .def("sample",  GetSampleFunction <SamplerFattal>(), py::arg("N"));

    py::class_<SamplerForcedRandom>(m, "ForcedRandom")
        .def(py::init<uint32_t, uint32_t>(), py::arg("sparcity") = 8, py::arg("matrixSize") = 512)
        .def("__repr__", [](const SamplerForcedRandom& wn) { return "ForcedRandom()"; })
        .def("setMatrixSparsity", &SamplerForcedRandom::setMatrixSparsity)
        .def("setMatrixSize"    , &SamplerForcedRandom::setMatrixSize)
        .def("sample",  GetSampleFunction <SamplerForcedRandom>(), py::arg("N"));

    py::class_<SamplerStep>(m, "Step")
        .def(py::init<float, float>(), py::arg("critFreq") = 0.606, py::arg("smooth") = 8.f)
        .def("__repr__", [](const SamplerStep& wn) { return "STEP()"; })
        .def("setSeed", GetSetSeedFunction<SamplerStep>(), py::arg("seed") = NO_SEED)
        .def("setCriticalFrequency", &SamplerStep::setCriticalFrequency)
        .def("setSmoothing"        , &SamplerStep::setSmoothing)
        .def("sample",  GetSampleFunction <SamplerStep>(), py::arg("N"));

    py::class_<SamplerSinglePeak>(m, "SinglePeak")
        .def(py::init<
                float, float, float, float
             >(), 
                py::arg("critFreq") = 36.5, py::arg("smooth") = 8.f,
                py::arg("peakSmoothing") = 2.0, py::arg("peakPower") = 16.0
        )
        .def("__repr__", [](const SamplerSinglePeak& wn) { return "SinglePeak()"; })
        .def("setSeed", GetSetSeedFunction<SamplerSinglePeak>(), py::arg("seed") = NO_SEED)
        .def("setPeakPower"        , &SamplerSinglePeak::setPeakPower)
        .def("setPeakSmoothing"    , &SamplerSinglePeak::setPeakSmoothing)
        .def("setCriticalFrequency", &SamplerSinglePeak::setCriticalFrequency)
        .def("setSmoothing"        , &SamplerSinglePeak::setSmoothing)
        .def("sample",  GetSampleFunction <SamplerSinglePeak>(), py::arg("N"));
        
    using SobolSampler = SamplerSobol<uint32_t>; 
    py::class_<SobolSampler>(m, "Sobol")
        .def(
            py::init<uint32_t, uint32_t, const std::string&>(),
            py::arg("d"), py::arg("depth") = 32, py::arg("file") = ""
        )
        .def("__repr__", [](const SobolSampler& wn) { return "Sobol(d=" + std::to_string(wn.GetDimension()) +")"; })
        .def("setDirectionFile", &SobolSampler::setDirectionFile)
        .def("setOwenDepth",     &SobolSampler::setOwenDepth)
        .def("setSeed", GetSetSeedFunction<SobolSampler>()              , py::arg("seed") = NO_SEED)
        .def("sample",  GetSampleFunction <SobolSampler>()              , py::arg("N"))
        .def("isample", GetSampleFunction <SobolSampler, uint32_t>(), py::arg("N"));

    
    using CascadedSobolSampler = SamplerCascadedSobol<uint32_t>; 
    py::class_<CascadedSobolSampler>(m, "CascadedSobol")
        .def(
            py::init<uint32_t, uint32_t>(),
            py::arg("d"), py::arg("depth") = 32
        )
        .def("__repr__", [](const CascadedSobolSampler& wn) { return "CascadedSobolSampler(d=" + std::to_string(wn.GetDimension()) +")"; })
        .def("setDirectionFile", &CascadedSobolSampler::setDirectionFile)
        .def("setOwenDepth",     &CascadedSobolSampler::setOwenDepth)
        .def("setSeed", GetSetSeedFunction<CascadedSobolSampler>()          , py::arg("seed") = NO_SEED)
        .def("sample",  GetSampleFunction <CascadedSobolSampler>()          , py::arg("N"))
        .def("isample", GetSampleFunction <CascadedSobolSampler, uint32_t>(), py::arg("N"));

    using GenerativeMatrices = GenerativeMatrices<uint32_t>; 
    py::class_<GenerativeMatrices>(m, "GenerativeMatrices")
        .def(
            py::init<std::string, uint8_t, uint8_t, uint8_t, bool>(),
            py::arg("filename"), py::arg("m"), py::arg("p"), py::arg("d"), py::arg("scramble") = false
        )
        .def("__repr__", [](const GenerativeMatrices& wn) { return "GenerativeMatricesSampler(d=" + std::to_string(wn.GetDimension()) +")"; })
        .def("setSeed", GetSetSeedFunction<GenerativeMatrices>()          , py::arg("seed") = NO_SEED)
        .def("sample",  GetSampleFunction <GenerativeMatrices>()          , py::arg("N"))
        .def("isample", GetSampleFunction <GenerativeMatrices, uint32_t>(), py::arg("N"));

}