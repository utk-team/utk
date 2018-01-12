#ifndef _UTK_SAMPLER_PENROSE_
#define _UTK_SAMPLER_PENROSE_

#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>

#include "../utils.hpp"
#include "SamplerPenrose/quasisampler_prototype.h"

namespace utk
{
	
class MyQuasisampler : public penrose::Quasisampler
{
  unsigned const_val;

public:
  MyQuasisampler(unsigned val) : penrose::Quasisampler(100,100), const_val(val) {}

  unsigned getImportanceAt( penrose::Point2D pt ) 
  { 
    return const_val; // This function returns a constant value.
  }
};


class SamplerPenrose
{
protected:
public:

	SamplerPenrose() { }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerPenrose::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}

		if (!isFloatingType<T>())
		{
			ERROR("SamplerPenrose::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
			return false;
		}

		//load domain & toricity
		for(uint d=0; d<D; d++)
		{
			arg_pts.domain.pMin.pos()[d] = 0;
			arg_pts.domain.pMax.pos()[d] = 1;
		}
		arg_pts.toricity = 1;

		unsigned mag_factor = arg_points*10;

		// initialize sampler
		MyQuasisampler test(mag_factor);

		// generate points
		std::vector<penrose::Point2D> points = test.getSamplingPoints();

		arg_pts.resize(points.size());
		for (int i=0; i<points.size(); i++)
		{
				arg_pts[i].pos()[0] = (points[i].x / 100);
				arg_pts[i].pos()[1] = (points[i].y / 100);
		}
		return true;
	};

protected:

};

} //end namespace utk


#endif
