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

#include <utk/utils/log.hpp>
#include <utk/utils/Pointset.hpp>
#include "Kronecker/Kronecker.hpp"
#include <random>

namespace utk
{
class SamplerKronecker
{
protected:
public:
    // List of natively supported shifts
    static std::vector<std::string> GetAlphas()
    {
        return {
            "R2", 
            "R3", 
            "R4", 
            "K21_2",
            "K21_3",
            "K21_4",
            "K21b_2",
            "K21b_3",
            "K21b_4"
        };
    }

    // Return alpha from name and verify dimension
    static const double* ParseAlpha(const std::string& name, uint32_t dim)
    {
        switch (dim)
        {
        case 2:
            if (name.empty())     return Kronecker::R2;
            if (name == "R2")     return Kronecker::R2;
            if (name == "K21_2")  return Kronecker::K21_2;
            if (name == "K21b_2") return Kronecker::K21b_2;
            return nullptr;
        case 3:
            if (name.empty())     return Kronecker::R3;
            if (name == "R3")     return Kronecker::R3;
            if (name == "K21_3")  return Kronecker::K21_3;
            if (name == "K21b_3") return Kronecker::K21b_3;
            return nullptr;
        case 4:
            if (name.empty())     return Kronecker::R4;
            if (name == "R4")     return Kronecker::R4;
            if (name == "K21_4")  return Kronecker::K21_4;
            if (name == "K21b_4") return Kronecker::K21b_4;
            return nullptr;
        default:
            return nullptr;
        };
    }

	SamplerKronecker(uint32_t d) : D(d) 
    {
             if (d == 2) setAlphas(ParseAlpha("R2", d));
        else if (d == 3) setAlphas(ParseAlpha("R3", d));
        else if (d == 4) setAlphas(ParseAlpha("R4", d));
        else  setAlphas(nullptr);
     }

    void setAlphas(const double* al)  { m_alphas = al; }
	void setDimension(uint32_t d) { D = d; }
    uint32_t GetDimension() const { return D; }
    
	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
        if (m_alphas == nullptr)
        {
            UTK_ERROR("Sampler Kronecker: No vector given", 0);
            return false;
        }
            
        arg_pts.Resize(N, D);
		for(uint32_t i = 0; i < N; i++)
        {
            for (uint32_t d = 0; d < D; d++)
            {
                arg_pts[i][d] = Kronecker::get_value(i, d, m_alphas);
            }
        }
        
		return true;
	};

protected:
	uint32_t D;
    const double* m_alphas;
};

}
