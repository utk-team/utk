#ifndef _UTK_SAMPLER_FORCEDRANDOM_
#define _UTK_SAMPLER_FORCEDRANDOM_

#include "../pointsets/Pointset.hpp"
#include "SamplerForcedRandom/src/ForcedRandomSampling/main.hpp"
#include <cstdlib>
#include <random>
#include <chrono>

#include "../utils.hpp"

namespace utk
{
	
/**
 * Code from
 * 
 * Cornel, D., Tobler, R. F., Sakai, H., Luksch, C., & Wimmer, M. (2017). 
 * Forced Random Sampling: fast generation of importance-guided blue-noise samples. 
 * The Visual Computer, 1-11.
 * https://www.researchgate.net/publication/317002694_Forced_Random_Sampling_Reference_Implementation
 * 
 *
 *
 *This is free and unencumbered software released into the public domain.
 
 Anyone is free to copy, modify, publish, use, compile, sell, or
 distribute this software, either in source code form or as a compiled
 binary, for any purpose, commercial or non-commercial, and by any
 means.
 
 In jurisdictions that recognize copyright laws, the author or authors
 of this software dedicate any and all copyright interest in the
 software to the public domain. We make this dedication for the benefit
 of the public at large and to the detriment of our heirs and
 successors. We intend this dedication to be an overt act of
 relinquishment in perpetuity of all present and future rights to this
 software under copyright law.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
 
 For more information, please refer to <http://unlicense.org/>
 */

class SamplerForcedRandom
{
protected:

public:

	SamplerForcedRandom() {
		sparsity=8;
		matrix_size=512;
	}
	
	void setMatrixSize(int arg_matrix_size) { matrix_size = arg_matrix_size;
		if(matrix_size != 512 && matrix_size != 1024 && matrix_size != 2048)
		{
			WARNING("Matrix size must be either 512,1024 or 2048. Matrix Size set to 512");
			matrix_size = 512;
		}
	}
	void setMatrixSparsity(int arg_sparsity) { sparsity = arg_sparsity; }


	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerForcedRandom::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}

		if (!isFloatingType<T>())
		{
			ERROR("SamplerForcedRandom::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
			return false;
		}

		//load domain & toricity
		for(uint d=0; d<D; d++)
		{
			arg_pts.domain.pMin.pos()[d] = 0;
			arg_pts.domain.pMax.pos()[d] = 1;
		}
		arg_pts.toricity = 1;


		forcedrandom::n = arg_points;
		forcedrandom::sigma = sparsity;
		forcedrandom::w_M = matrix_size;

		std::stringstream matrixPathSStr;
		std::stringstream samplesOutputPathSStr;
		std::stringstream samplesCLOutputPathSStr;
		std::stringstream samplesImageOutputPathSStr;

#ifdef WIN32
	// For correct relative paths when starting the application from within Visual Studio
	if (IsDebuggerPresent() == TRUE)
	{
		matrixPathSStr << "../";
		samplesOutputPathSStr << "../";
		samplesCLOutputPathSStr << "../";
		samplesImageOutputPathSStr << "../";
	}
#endif

		matrixPathSStr << UTK_DATA_PATH << "/ForcedRandom/" << forcedrandom::w_M << "x" << forcedrandom::w_M << ".matrix";

		forcedrandom::LoadMatrix(matrixPathSStr.str());

		// Generate or load an importance map
		forcedrandom::s_I = V2i(w_I, h_I);
		forcedrandom::GenerateImportanceMap();

		forcedrandom::Setup();
		forcedrandom::CreateBlockwiseSortedList();

		
	#ifdef USEOPENCL
		SetupOpenCL();
		std::vector<V2d> samples = GenerateSamplesOpenCL();
		CleanupOpenCL();
	#else
		std::vector<V2d> samples =forcedrandom:: GenerateSamples();
	#endif

		arg_pts.resize(samples.size());
		for(int i=0; i<samples.size(); i++)
		{
			arg_pts[i].pos()[0] = samples[i].x;
			arg_pts[i].pos()[1] = samples[i].y;
		}

		return true;
	};

protected:
	int sparsity;
	int matrix_size;
};

} //end namespace utk


#endif
