#ifndef _UTK_SAMPLER_HAMMERSLEY_
#define _UTK_SAMPLER_HAMMERSLEY_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>
#include <sstream>

namespace utk
{

	class SamplerHammersley
	{
	protected:

	public:

		SamplerHammersley() { }

		template<unsigned int D, typename T, typename P>
		bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned long long int arg_points)
		{
			if (!arg_pts.empty())
			{
				WARNING("SamplerHammersley::generateSamples the pointset to fill is not empty, clearing it ...");
				arg_pts.clear();
			}
			if (D != 2)
			{
				ERROR("SamplerHammersley::generateSamples only generates 2D pointsets ...");
				exit(1);
			}
		
			arg_pts.resize(arg_points);
			arg_pts.toricity = 1;
			
			if(isIntegerType<T>())
			{
				int pmax = ceil((double)(log(arg_points)/log(2)));
				int smax = pow(2, pmax);
				
				for(int d=0; d<D; d++){
					arg_pts.domain.pMin.pos()[d] = 0;
					arg_pts.domain.pMax.pos()[d] = smax;
				}
				
				for(int i=0; i<arg_points; i++)
				{
					arg_pts[i].pos()[0] = radicalInverse_VdC(i)*smax;
					arg_pts[i].pos()[1] = i;
				}
			}
			else if (isFloatingType<T>())
			{
				for(int d=0; d<D; d++){
					arg_pts.domain.pMin.pos()[d] = 0;
					arg_pts.domain.pMax.pos()[d] = 1;
				}
				
				for(int i=0; i<arg_points; i++)
				{
					arg_pts[i].pos()[0] = radicalInverse_VdC(i);
					arg_pts[i].pos()[1] = (double)i/(double)arg_points;
				}
			}
			else
			{
				ERROR("SamplerHammersley::generateSamples The samples coordinates must be of numeric value");
				return false;
			}

			return true;
		};

	protected:
		
	//Code from http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
	static float radicalInverse_VdC(uint bits) {
		bits = (bits << 16u) | (bits >> 16u);
		bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
		bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
		bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
		bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
		return float(bits) * 2.3283064365386963e-10; // / 0x100000000
	}

	};

} //end namespace utk


#endif
