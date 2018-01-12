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

	SamplerGenerativeMatrix() {}
	
	void setInputFile(std::string arg_file) {
		if(!loadFromFile(arg_file)) exit(1);
	}

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerGenerativeMatrix::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		if(!sizeMatrixToNbPts(arg_points))
		{
			std::stringstream msg;
			msg << "SamplerGenerativeMatrix::generateSamples failed to resize the input matrix to " << matrixsize << " samples";
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
	int orig_matrixsize;
	int orig_matrixdim;
    bool orig_matrix[DMAX][30][30];
	
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
	
	bool sizeMatrixToNbPts(int arg_points)
	{
		matrixsize = ceil(log2(arg_points));
		matrixdim = orig_matrixdim;
		

		if(matrixsize%2 == 1 && orig_matrixsize%2 == 0)
		{
			std::stringstream msg;
			msg << "SamplerGenerativeMatrix::generateSamples can't resize a matrix of even size (" << orig_matrixsize << ") to a matrix of odd size (" << matrixsize << ")" << std::endl;
			ERROR(msg.str());
			return false;
		}
		if(matrixsize%2 == 0 && orig_matrixsize%2 == 1)
		{
			std::stringstream msg;
			msg << "SamplerGenerativeMatrix::generateSamples can't resize a matrix of odd size (" << orig_matrixsize << ") to a matrix of even size (" << matrixsize << ")" << std::endl;
			ERROR(msg.str());
			return false;
		}
		
		int total_added_rows = matrixsize-orig_matrixsize;
		int half_added_rows = total_added_rows/2;
		
		//Fill with zeros the first columns
		for(int d=0; d<orig_matrixdim; d++)
			for(int i=0; i<matrixsize; i++)
				for(int j=0; j<half_added_rows; j++)
				{
					if(d%2 == 0)
						matrix[d][i][j] = (i==j);
					else
						matrix[d][i][j] = ((matrixsize-i-1)==j);
				}
				
		//Fill with zeros the last columns
		for(int d=0; d<orig_matrixdim; d++)
			for(int i=0; i<matrixsize; i++)
				for(int j=half_added_rows+orig_matrixsize; j<matrixsize; j++)
				{
					if(d%2 == 0)
						matrix[d][i][j] = (i==j);
					else
						matrix[d][i][j] = ((matrixsize-i-1)==j);
				}
		
		//Fill with identity the first rows
		for(int d=0; d<orig_matrixdim; d++)
			for(int i=0; i<half_added_rows; i++)
				for(int j=0; j<matrixsize; j++)
				{
					if(d%2 == 0)
						matrix[d][i][j] = (i==j);
					else
						matrix[d][i][j] = ((matrixsize-i-1)==j);
				}
				
		//Fill with identity the last rows
		for(int d=0; d<orig_matrixdim; d++)
			for(int i=half_added_rows+orig_matrixsize; i<matrixsize; i++)
				for(int j=0; j<matrixsize; j++)
				{
					if(d%2 == 0)
						matrix[d][i][j] = (i==j);
					else
						matrix[d][i][j] = ((matrixsize-i-1)==j);
				}
				
		//copy the original matrix
		for(int d=0; d<orig_matrixdim; d++)
			for(int i=0; i<orig_matrixsize; i++)
				for(int j=0; j<orig_matrixsize; j++)
					matrix[d][i+half_added_rows][j+half_added_rows] = orig_matrix[d][i][j];
				
				
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
		file >> orig_matrixsize >> orig_matrixdim;
		if(orig_matrixsize > 30)
		{
			ERROR("SamplerGenerativeMatrix::loadFromFile Matrixisize cannot exceed 30");
			orig_matrixsize = 0;
			return false;
		}
		if(orig_matrixdim > DMAX)
		{
			ERROR("SamplerGenerativeMatrix::loadFromFile Matrixisize cannot exceed 10");
			orig_matrixdim = 0;
			return false;
		}
		
		std::cout << "Loading " << orig_matrixdim << "D matrix of size " << orig_matrixsize << "x" << orig_matrixsize << std::endl;
		
		for(int d=0; d<orig_matrixdim; d++)
		for(int i=0; i<orig_matrixsize; i++)
		for(int j=0; j<orig_matrixsize; j++)
		{
			file >> orig_matrix[d][i][j];
		}
		file.close();
		
		for(int d=0; d<orig_matrixdim; d++)
		{
			for(int i=0; i<orig_matrixsize; i++)
			{
				for(int j=0; j<orig_matrixsize; j++)
					std::cout << orig_matrix[d][i][j] << " ";
				std::cout << std::endl;
			}
			
			std::cout << std::endl;
		}
		
		return true;
	}
};

} //end namespace utk


#endif
