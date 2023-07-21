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

#include "ForcedRandom/src/ForcedRandomSampling/main.hpp"
#include <utk/utils/Pointset.hpp>
#include <random>

namespace utk
{

class SamplerForcedRandom
{
protected:
public:

	SamplerForcedRandom(
		uint32_t sparsity_ = 8, 
		uint32_t matrixSize_ = 512
	) : sparsity(sparsity_) { setMatrixSize(matrixSize_); }

    uint32_t GetDimension() const { return 2; }

	void setMatrixSparsity(int sparsity_) 
	{
		sparsity = sparsity_;
	}

	void setMatrixSize(int matrixSize_)
	{
		matrixSize = matrixSize_;
		if (matrixSize != 512 && matrixSize != 1024 && matrixSize != 2048)
		{
			matrixSize = 512;
		}
	}

	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
        forcedrandom::n = N;
		forcedrandom::sigma = sparsity;
		forcedrandom::w_M = matrixSize;

		std::ostringstream matrixPathSStr;
		matrixPathSStr << UTK_DATA_PATH << "/ForcedRandom/" << matrixSize << "x" << matrixSize << ".matrix";

		forcedrandom::LoadMatrix(matrixPathSStr.str());
		forcedrandom::s_I = V2i(w_I, h_I);
		forcedrandom::GenerateImportanceMap();

		forcedrandom::Setup();
		forcedrandom::CreateBlockwiseSortedList();

		std::vector<V2d> samples = forcedrandom::GenerateSamples();
		arg_pts.Resize(N, 2);
        for(uint32_t i = 0; i < samples.size(); i++)
		{
			arg_pts[i][0] = samples[i].x;
			arg_pts[i][1] = samples[i].y;
		}
		return true;
	};

protected:
	int sparsity;
	int matrixSize;
};

}
