#ifndef _UTK_SAMPLER_HALTONSPP_
#define _UTK_SAMPLER_HALTONSPP_

#include "../pointsets/Pointset.hpp"
#include "../utils.hpp"
#include <sstream>

namespace utk
{

static unsigned int primes[10] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};

class SamplerHalton
{
protected:
public:

	SamplerHalton() {
		setBaseList(primes, 10);
		m_spp = 4;
	}

	void setBaseList( unsigned int* arg_base_list, unsigned int arg_idBaseMax )
	{
		m_base_list = arg_base_list;
		m_idBaseMax = arg_idBaseMax;
	}
	
	void setSpp( unsigned int arg_spp )
	{
		m_spp = arg_spp;
	}

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned long long int arg_points)
	{
		/*if(D != 2)
		{
			ERROR("SamplerHaltonSpp::generateSamples only generates 2D samples");
			return false;
		}*/

		if (!arg_pts.empty())
		{
			WARNING("SamplerHaltonSpp::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		int resolution = arg_points/m_spp;
		int size = sqrt(resolution);
		std::cout << arg_points << " samples, " << m_spp << " samples per pixel => " << size << "^2 pixel" << std::endl;
		
		if(resolution*m_spp != arg_points)
		{
			std::stringstream msg;
			msg << "SamplerHaltonSpp::generateSamples Can't subdivide " << arg_points << " into an integer number of blocks of " << m_spp << " samples" << std::endl;
			ERROR(msg.str());
			return false;
		}
		if(size*size != resolution)
		{
			std::stringstream msg;
			msg << "SamplerHaltonSpp::generateSamples Can't subdivide " << arg_points << " into an squared number of blocks of " << m_spp << " samples" << std::endl;
			ERROR(msg.str());
			return false;
		}
		
		int** count;
		count = new int*[size];
		for(int i=0; i<size; i++) count[i] = new int[size];
		for(int i=0; i<size; i++)
		for(int j=0; j<size; j++)
			count[i][j] = 0;

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

			int n=0;
			for(unsigned int i=0; i<arg_points; i++)
			{
				T data[D];
				for(uint d=0;d<D;d++)
				{
					int pmax = ceil(log(arg_points)/log(m_base_list[d]));
					int smax = pow(m_base_list[d], pmax);
					int px = (int)((double)(generateCoordinate(n, m_base_list[d]))*smax);
					if (px > smax)
						px -= smax;

					data[d] = px;
				}
				n++;

				int pixel_x = (data[0] / arg_points)*size;
				int pixel_y = (data[1] / arg_points)*size;
				if(count[pixel_x][pixel_y] == m_spp)
				{
					i--;
				}
				else if(count[pixel_x][pixel_y] < m_spp)
				{
					count[pixel_x][pixel_y]++;
					arg_pts[i] = data;
				}
				else
				{
					ERROR("SamplerHaltonSpp::generateSamples Found pixel with too many samples");
					return false;
				}
			}
		}
		else if(isFloatingType<T>())
		{
			for(uint d=0; d<D; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = 1;
			}

			int n=0;
			for(unsigned int i=0; i<arg_points; i++)
			{
				T data[D];
				for(uint d=0;d<D;d++)
				{
					T px = (generateCoordinate(n, m_base_list[d]));
					if (px > 1)
						px -= 1;
					data[d] = px;
				}
				n++;
				
				int pixel_x = (data[0]*size);
				int pixel_y = (data[1]*size);
				if(count[pixel_x][pixel_y] == m_spp)
				{
					i--;
				}
				else if(count[pixel_x][pixel_y] < m_spp)
				{
					count[pixel_x][pixel_y]++;
					arg_pts[i] = data;
				}
				else
				{
					ERROR("SamplerHaltonSpp::generateSamples Found pixel with too many samples");
					return false;
				}
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

	uint m_spp;
	uint m_idBaseMax;
	unsigned int* m_base_list;
};

} //end namespace utk

#endif
