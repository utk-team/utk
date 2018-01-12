#ifndef _UTK_SAMPLER_REGULAR_GRID_
#define _UTK_SAMPLER_REGULAR_GRID_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>
#include <sstream>

namespace utk
{

	class SamplerRegularGrid
	{
	protected:

	public:

		SamplerRegularGrid() { ; }

		template<unsigned int D, typename T, typename P>
		bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned long long int arg_points)
		{
			if (!arg_pts.empty())
			{
				WARNING("SamplerRegularGrid::generateSamples the pointset to fill is not empty, clearing it ...");
				arg_pts.clear();
			}

			if(!isFloatingType<T>())
			{
				ERROR("SamplerRegularGrid::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
				return false;
			}

			/// Number of points to generate needs to be a power of D
			double nthroot = pow(arg_points, 1.0 / D);
			bool is_whole_number = floor(nthroot) == nthroot;
			int near_nthroot = int(nthroot);



			if(!is_whole_number)
			{
				std::stringstream ss_dim;
				ss_dim << D;

				std::stringstream ss_given;
				ss_given << arg_points;

				std::stringstream ss_used;
				ss_used << pow(near_nthroot, D);

				WARNING("SamplerRegularGrid::generateSamples needs " + ss_given.str() + " (the number of points to generate) to be a power of " + ss_dim.str() + ". Using " + ss_used.str() + " points instead.");

				arg_points = pow(near_nthroot, D);
			}

			//load domain & toricity
			for(uint d = 0; d<D; d++)
			{
				arg_pts.domain.pMin.pos()[d] = 0;
				arg_pts.domain.pMax.pos()[d] = 1;
			}
			arg_pts.toricity = 1;

			arg_pts.resize(arg_points);

			std::vector<int> counts;
			counts.resize(D+1, 0); /// D+1 to avoid overflow
			for(uint i = 0; i < arg_points; i++)
			{
				for(uint d = 0; d < D; d++)
				{
					arg_pts[i].pos()[d] = counts[d] * (1.0 / near_nthroot);
				}

				counts[0] += 1;
				for(uint j = 0; j < D; j++)
				{
					if(counts[j] >= near_nthroot)
					{
						counts[j] = 0;
						counts[j + 1] += 1;
					}
				}
			}

			return true;
		};
	};

} //end namespace utk


#endif
