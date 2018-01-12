#ifndef _UTK_SAMPLER_HALTON_
#define _UTK_SAMPLER_HALTON_

#include "../pointsets/Pointset.hpp"
#include "../utils.hpp"

namespace utk
{

static unsigned int primes[10] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};

class SamplerHalton
{
protected:
public:

	SamplerHalton() {
		setBaseList(primes, 10);
	}

	void setBaseList( unsigned int* arg_base_list, unsigned int arg_idBaseMax )
	{
		m_base_list = arg_base_list;
		m_idBaseMax = arg_idBaseMax;
	}

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned long long int arg_points)
	{
		if(D > m_idBaseMax)
		{
			ERROR("SamplerHalton::generateSamples the wanted number of dimensions is higher than the number of basis available");
			return false;
		}

		if (!arg_pts.empty())
		{
			WARNING("SamplerHalton::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}

		arg_pts.resize(arg_points);
		arg_pts.toricity = 1;
		if(isIntegerType<T>())
		{
			for(uint d=0; d<D; d++)
			{
				int pmax = ceil((double)(log(arg_points)/log(m_base_list[d])));
				int smax = pow(m_base_list[d], pmax);
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = smax;
			}

			for(unsigned int i=0; i<arg_points; i++)
			{
				T data[D];
				for(uint d=0;d<D;d++)
				{
					int pmax = ceil(log(arg_points)/log(m_base_list[d]));
					int smax = pow(m_base_list[d], pmax);
					int px = (int)((double)(generateCoordinate(i, m_base_list[d]))*smax);
					if (px > smax)
						px -= smax;

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
					T px = (generateCoordinate(i, m_base_list[d]));
					if (px > 1)
						px -= 1;
					data[d] = px;
				}

				arg_pts[i] = data;
			}
		}
		else
		{
			ERROR("SamplerHalton::generateSamples cannot generates samples of the given type. Can only generate samples as integers of floating values");
			return false;
		}
		return true;
	}

private:
	double generateCoordinate(int n, int BASE)
	{
		double res = 0.0f;
		double dev = 1.0f/BASE;

		while(n != 0)
		{
			int d = n % BASE;
			res += d * dev;
			n = (n-d)/BASE;
			dev = dev / BASE;
		}

		return res;
	}

	uint m_idBaseMax;
	unsigned int* m_base_list;
};

} //end namespace utk

#endif
