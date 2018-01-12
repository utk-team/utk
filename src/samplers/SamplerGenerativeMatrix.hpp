#ifndef _UTK_SAMPLER_GENERATIVEMATRIX_
#define _UTK_SAMPLER_GENERATIVEMATRIX_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include <fstream>
#include <eigen3/Eigen/Dense>

#define DMAX 20
#define NBPATTERNS 10
//#define TMAX 10

using namespace Eigen;

namespace utk
{

class SamplerGenerativeMatrix
{
public:
	
	SamplerGenerativeMatrix() { 
		srand(time(NULL));

		matrixsize = 0;
		increaseStep = 1;
		base=3;
	};
	
	void setBasis(double arg_base) { base=arg_base; }
	
	void resetMatrices()
	{
		for(int d=0; d<DMAX; d++)
			generatingMatrices[d].resize(0,0);
		matrixsize = 0;
	}

	void fillMatricesRandom(uint D, uint from, uint to)
	{
		for(int d=0; d<D; d++)
		{
			generatingMatrices[d].conservativeResize(to,to);
				
			for(int x=from; x<to; x++)
			for(int y=0; y<to; y++)
				generatingMatrices[d](x, y) = 0;
			
			for(int y=from; y<to; y++)
			{
				for(int x=0; x<to; x++)
					generatingMatrices[d](x, y) = rand()%base;
			}
		}
	}
	void increaseMatrixSize(uint D)
	{
		std::cout << "Increasing from " << matrixsize << " to " << matrixsize+increaseStep << std::endl;
		uint oldsizemat = matrixsize;
		matrixsize+=increaseStep;
		uint newsizemat = matrixsize;
		
		bool orthomat=0;
		while(!orthomat)
		{
			fillMatricesRandom(D, oldsizemat, newsizemat);

			orthomat = true;
			for(int d1=0; d1<=newsizemat && orthomat; d1++)
			{
				Matrix<double, Dynamic, Dynamic> orthotest(newsizemat, newsizemat);
				
				for(int i=0; i<d1; i++)
				for(int j=0; j<newsizemat; j++)
				{
					orthotest(i,j) = generatingMatrices[0](i,j);
				}
				for(int i=d1; i<newsizemat; i++)
				for(int j=0; j<newsizemat; j++)
				{
					orthotest(i,j) = generatingMatrices[1](i-d1,j);
				}

				int det = orthotest.determinant();
				if(det > 0) det%=base;
				else if(det < 0) while(det<0) det+=base;
				if(det == 0)
					orthomat = false;
			}
		}
		std::cout << "Done" << std::endl;
		
		std::cout << generatingMatrices[0] << std::endl;
		std::cout << "--" << std::endl;
		std::cout << generatingMatrices[1] << std::endl;
	}
	
	std::vector<uint> todigits(uint n)
	{
		std::vector<uint> ret;
		ret.resize(30);
		double dev = 1.0f/(double)base;

		int i=0;
		while(n != 0)
		{
			int d = n % base;
			n = (n-d)/base;
			dev = dev /base;
			ret[i++] = d;
		}

		return ret;
	}
	
	uint tovalue(std::vector<uint> n)
	{
		uint res = 0;
		uint dev = pow(base, 0);

		for(int i=0; i<n.size(); i++)
		{
			res += n[i] * dev;
			dev *= base;
		}

		return res;
	}

	template<uint D>
	double generateCoordinate(unsigned int index, unsigned int d)
	{
		uint indexmax = pow(base, matrixsize);
		if(index >= indexmax || matrixsize == 0)
			increaseMatrixSize(D);
		
		std::vector<uint> a = todigits(index);
		std::vector<uint> res = todigits(0);
		
		for (int row = 0; row < matrixsize; row++)
		for (int col = 0; col < matrixsize; col++)
		{
			res[matrixsize-1-row] = (res[matrixsize-1-row] + (generatingMatrices[d](row,col) * a[col])%base )%base;
		}
		
		return (double)tovalue(res) / (double)indexmax;
	}
	
	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerGenerativeMatrix::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		/*if (matrixsize != 0)
		{
			WARNING("SamplerGenerativeMatrix::generateSamples matrices already filled, resetting ...");
			resetMatrices();
		}*/
		
		if (D > DMAX)
		{
			std::stringstream msg;
			msg << "SamplerGenerativeMatrix::generateSamples only generates up to " << DMAX << " D samples  with the current input matrix";
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
		if(isIntegerType<T>())
		{
			int pmax = ceil( logbase(base, arg_points) );
			int smax = pow(base, pmax);
			
			for(uint d=0; d<D; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = smax;
			}

			for(unsigned int i=0; i<arg_points; i++)
			{
				T data[D];
				for(uint d=0;d<D;d++)
				{
					unsigned int px = (int)(generateCoordinate<D>(i, d)*smax);
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
					T px = generateCoordinate<D>(i, d);
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
	}
	
	Matrix<unsigned int, Dynamic, Dynamic>& getMatrice(uint d)
	{
		return generatingMatrices[d];
	}
	
private:
	uint matrixsize;
	uint base;
	uint increaseStep;
	Matrix<unsigned int, Dynamic, Dynamic> generatingMatrices[DMAX];
	
	double logbase(double b, double x)
	{
		return (log(x)/log(b));
	}
	
};

} //end namespace utk

#endif
