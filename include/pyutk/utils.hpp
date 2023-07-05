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
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <utk/utils/Pointset.hpp>

namespace py = pybind11;
using namespace utk;

// Global constants for various samplers
namespace utk
{
    inline static constexpr uint64_t NO_SEED = 0;
};

template<typename DataType>
auto ToNumpyArray(Pointset<DataType>* pts)
{
    py::capsule freeArray(pts, [](void* data) {
        Pointset<DataType>* toFree = reinterpret_cast<Pointset<DataType>*>(data);
        delete toFree;
    }); 

    return py::array_t<DataType>(
        { pts->Npts(), pts->Ndim() },
        { pts->Ndim() * sizeof(DataType), sizeof(DataType) },
          pts->Data(),
          freeArray
    );       
}

template<typename T, typename DataType = double>
auto GetSampleFunction()
{
    return [](T& sp, uint32_t n) {
        // Use pointer to avoid 'automatic' deletion
        Pointset<DataType>* pts = new Pointset<DataType>;
        bool rslt = sp.generateSamples(*pts, n);

        if (rslt)
            return ToNumpyArray<DataType>(pts);

        throw std::runtime_error("Error while generating samples");
    };
}

template<typename T, typename InputType, typename OutputType>
auto GetScrambleFunction()
{
    return [](T& scrambler, py::buffer b){
        py::buffer_info info = b.request();

        if (info.format != py::format_descriptor<InputType>::format())
            throw std::runtime_error("Type not supported");

        if (info.ndim != 2)
            throw std::runtime_error("Only 2D array are supported");
        
        std::vector<py::ssize_t> desiredStride = {
            info.shape[1] * static_cast<py::ssize_t>(sizeof(InputType)), sizeof(InputType)
        };
        if (info.strides != desiredStride)
            throw std::runtime_error("Only contiguous C-ordered array are supported");
        
        Pointset<InputType> in = Pointset<InputType>::View(
            static_cast<InputType*>(info.ptr), 
            info.shape[0], info.shape[1]
        );

        Pointset<OutputType>* out = new Pointset<OutputType>;
        scrambler.Scramble(in, *out);
        return ToNumpyArray<OutputType>(out);
    };
}

template<typename T, typename U>
struct ComputeOutType
{ using type = std::variant<T, U>; };

template<typename T>
struct ComputeOutType<T, T>
{ using type = T; };


template<typename T, typename InputType, typename OutputType = InputType, typename VectorizedOutputType = std::vector<OutputType>>
auto GetComputeFunction()
{
    using outtype = typename ComputeOutType<OutputType, VectorizedOutputType>::type;

    return [](T& metrics, py::buffer b){
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
            outtype out = metrics.compute(in);
            return out;
        }
        else if (info.ndim == 3)
        {
             std::vector<py::ssize_t> desiredStride = {
                info.shape[2] * info.shape[1] * static_cast<py::ssize_t>(sizeof(InputType)), 
                                info.shape[2] * static_cast<py::ssize_t>(sizeof(InputType)), 
                                                                         sizeof(InputType)
            };

            if (info.strides != desiredStride)
                throw std::runtime_error("Only contiguous C-ordered array are supported");
            
            std::vector<Pointset<InputType>> ptss(info.shape[0]);
            for (uint32_t k = 0; k < info.shape[0]; k++)
            {
                ptss[k] =  Pointset<InputType>::View(
                    static_cast<InputType*>(info.ptr) + k * info.shape[1] * info.shape[2], 
                    info.shape[1], info.shape[2]
                );
            }

            outtype out = metrics.compute(ptss);
            return out;
        }

        throw std::runtime_error("Only 2D/3D array are supported");
        return outtype{};
    };
}

template<typename T, typename InputType = T>
auto GetVComputeFunction()
{
    return [](T& metrics, py::buffer b){
        py::buffer_info info = b.request();

        if (info.format != py::format_descriptor<InputType>::format())
            throw std::runtime_error("Type not supported");

        if (info.ndim != 2)
            throw std::runtime_error("Only 2D array are supported");
        
        std::vector<py::ssize_t> desiredStride = {
            info.shape[1] * static_cast<py::ssize_t>(sizeof(InputType)), sizeof(InputType)
        };
        if (info.strides != desiredStride)
            throw std::runtime_error("Only contiguous C-ordered array are supported");
        
        Pointset<InputType> in = Pointset<InputType>::View(
            static_cast<InputType*>(info.ptr), 
            info.shape[0], info.shape[1]
        );

        auto out = metrics.compute(in);
        return out;
    };
}

template<typename T>
auto GetSetSeedFunction()
{
    return [](T& sp, uint64_t seed)
    {
        if (seed == NO_SEED)
            sp.setRandomSeed();
        else
            sp.setRandomSeed(seed);
    };
}
