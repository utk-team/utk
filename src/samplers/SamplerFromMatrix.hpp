/*
 * Coded by Hélène Perrier helene.perrier@liris.cnrs.fr
 * Copyright (c) 2018 CNRS Université de Lyon
 * All rights reserved.
 *
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
 * either expressed or implied, of the Halton project.
 */
#ifndef _UTK_SAMPLER_GENERATIVEMATRIX_
#define _UTK_SAMPLER_GENERATIVEMATRIX_

#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>
#include <bitset>
#include <fstream>
#include <sstream>

#include "../utils.hpp"

namespace utk
{
	
#define DMAX 10

class SamplerGenerativeMatrix
{
protected:
public:

	SamplerGenerativeMatrix() { 
		if(!loadFromFile(std::string(UTK_DATA_PATH)+"/generative_matrix/mat0.dat")) exit(1);
	}

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerGenerativeMatrix::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		if (arg_points > (1 << matrixsize))
		{
			std::stringstream msg;
			msg << "SamplerGenerativeMatrix::generateSamples only generates up to " << (1 << matrixsize) << " samples with the current input matrix";
			ERROR(msg.str());
			return false;
		}
		
		if (D > matrixdim)
		{
			std::stringstream msg;
			msg << "SamplerGenerativeMatrix::generateSamples only generates up to " << matrixdim << " D samples  with the current input matrix";
			ERROR(msg.str());
			return false;
		}
		
		//load domain & toricity
		for(uint d=0; d<D; d++)
		{
			arg_pts.domain.pMin.pos()[d] = 0;
			arg_pts.domain.pMax.pos()[d] = 1;
		}
		arg_pts.toricity = 1;

		arg_pts.resize(arg_points);
		arg_pts.toricity = 1;
		if(isIntegerType<T>())
		{
			for(uint d=0; d<D; d++)
			{
				int smax = (1 << matrixsize);
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = smax;
			}

			for(unsigned int i=0; i<arg_points; i++)
			{
				T data[D];
				for(uint d=0;d<D;d++)
				{
					unsigned int px = generateCoordinate(i, d);
					data[d] = px;
				}

				arg_pts[i] = data;
			}
		}
		else if(isFloatingType<T>())
		{
			for(uint d=0; d<D; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = 1;
			}

			for(unsigned int i=0; i<arg_points; i++)
			{
				T data[D];
				for(uint d=0;d<D;d++)
				{
					double smax = (1 << matrixsize);
					T px = (double)generateCoordinate(i, d) / (double)smax;
					if (px > 1)
						px -= 1;
					data[d] = px;
				}

				arg_pts[i] = data;
			}
		}
		else
		{
			ERROR("SamplerGenerativeMatrix::generateSamples The samples coordinates must be of numeric value");
			return false;
		}

		return true;
	};

protected:
	int matrixsize;
	int matrixdim;
    bool matrix[DMAX][30][30];
	
	unsigned long int generateCoordinate(unsigned long int i, int d)
	{
		std::bitset<30> a = i;
		std::bitset<30> res = 0;
		
		for (int row = 0; row < matrixsize; row++)
		for (int col = 0; col < matrixsize; col++)
			res[matrixsize-1-row] = (res[matrixsize-1-row] ^ (matrix[d][row][col] * a[col]));

		return res.to_ulong();
	}
	
	bool loadFromFile(std::string matrixfile)
	{
		std::ifstream file;
		file.open(matrixfile);
		if(!file.is_open())
		{
			std::stringstream msg;
			msg << "SamplerGenerativeMatrix::loadFromFile couldn't open file " << matrixfile;
			ERROR(msg.str());
			return false;
		}
		file >> matrixsize >> matrixdim;
		if(matrixsize > 30)
		{
			ERROR("SamplerGenerativeMatrix::loadFromFile Matrixisize cannot exceed 30");
			matrixsize = 0;
			return false;
		}
		if(matrixdim > DMAX)
		{
			ERROR("SamplerGenerativeMatrix::loadFromFile Matrixisize cannot exceed 10");
			matrixdim = 0;
			return false;
		}
		
		std::cout << "Loading " << matrixdim << "D matrix of size " << matrixsize << "x" << matrixsize << std::endl;
		
		for(int d=0; d<matrixdim; d++)
		for(int i=0; i<matrixsize; i++)
		for(int j=0; j<matrixsize; j++)
		{
			file >> matrix[d][i][j];
		}
		file.close();
		
		for(int d=0; d<matrixdim; d++)
		{
			for(int i=0; i<matrixsize; i++)
			{
				for(int j=0; j<matrixsize; j++)
					std::cout << matrix[d][i][j] << " ";
				std::cout << std::endl;
			}
			
			std::cout << std::endl;
		}
		
		return true;
	}
};

} //end namespace utk


#endif
