#ifndef _UTK_SAMPLER_FASTPOISSON_
#define _UTK_SAMPLER_FASTPOISSON_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include <cstdlib>
#include <random>
#include <chrono>
#include <sstream>

#include "SamplerFastPoisson/PDSampling.h"

namespace utk
{

/**
 * Code from
 * 
 * Dunbar, D., & Humphreys, G. (2006). 
 * A spatial data structure for fast Poisson-disk sample generation. 
 * ACM Transactions on Graphics (TOG), 25(3), 503-508.
 * http://www.cs.virginia.edu/~gfx/pubs/antimony/
 * 
 */

class SamplerFastPoisson
{
protected:

public:

	SamplerFastPoisson() { 
		multiplier = 1;
		minMaxThrows = 1000;
		maximize = false;
		isTiled = false;
		relax = 0;
		sampler = 0;
	}
	
	void setMaximize(bool arg_maximize) { maximize = arg_maximize; }
	void setTiled(bool arg_tiled) { isTiled = arg_tiled; }
	void setRelaxIter(int arg_iter) { relax = arg_iter; }
	void setMultiplier(int arg_multi) { multiplier = arg_multi; }
	void setRadius(float arg_radius)
	{
		radius = arg_radius;
		if (radius<0.0005) 
		{
			std::stringstream ss;
			ss << "Radius (" << radius << ") is outside allowable range [0.0005,0.2]. Using 0.0005 instead";
			WARNING(ss.str());
			radius = 0.0005;
		}
		else if  (radius>.2)
		{
			std::stringstream ss;
			ss << "Radius (" << radius << ") is outside allowable range [0.0005,0.2]. Using 0.2 instead";
			WARNING(ss.str());
			radius = 0.2;
		}
	}
	void setMinMaxThrows(int arg_minmaxthrows) { minMaxThrows = arg_minmaxthrows; }
	void setMethod(std::string arg_method) { method = arg_method; }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (D != 2)
		{
			ERROR("SamplerFastPoisson::generateSamples handles only 2D samples");
			return false;
		}
		
		if (!arg_pts.empty())
		{
			WARNING("SamplerFastPoisson::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}

		if (!isFloatingType<T>())
		{
			ERROR("SamplerFastPoisson::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
			return false;
		}
				
		if(sampler != 0)
			delete sampler;
		if (method == "DartThrowing") 
			sampler = new dunbarPoisson::DartThrowing(radius, isTiled, minMaxThrows, multiplier);
		//else if (method == "BestCandidate")
		//	sampler = new dunbarPoisson::BestCandidate(radius, isTiled, multiplier);
		//else if (method == "Boundary")
		//	sampler = new dunbarPoisson::BoundarySampler(radius, isTiled);
		else if (method == "Pure")
		{
			if (!isTiled) 
			{
				WARNING("Pure sampler does not support untiled domain.\n");
				isTiled = true;
			}
			sampler = new dunbarPoisson::PureSampler(radius);
		} 
		else if (method == "LinearPure")
		{
			if (!isTiled) 
			{
				WARNING("LinearPure sampler does not support untiled domain.\n");
				isTiled = true;
			}
			sampler = new dunbarPoisson::LinearPureSampler(radius);
		} 
		/*else if (method == "Penrose")
		{
			if (isTiled) 
			{
				WARNING("Penrose sampler does not support untiled domain.\n");
				isTiled = false;
			}
			sampler = new dunbarPoisson::PenroseSampler(radius);
		}*/
		/*else if (method == "Uniform")
			sampler = new dunbarPoisson::UniformSampler(radius);*/
		else 
		{
			std::stringstream ss;
			ss << "Unrecognized sampling method (" << method << ")";
			ERROR(ss.str());
			return false;
		}
		
		//load domain & toricity
		for(uint d=0; d<D; d++)
		{
			arg_pts.domain.pMin.pos()[d] = 0;
			arg_pts.domain.pMax.pos()[d] = 1;
		}
		arg_pts.toricity = 1;
		
		sampler->complete();
		if (maximize) sampler->maximize();
		for (int i=0; i<relax; i++) sampler->relax();
		
		int N = (int) sampler->points.size();
		arg_pts.resize(N);
		
		for(int i=0; i<N; i++)
		{
			arg_pts[i].pos()[0] = sampler->points[i].x;
			arg_pts[i].pos()[1] = sampler->points[i].y;
		}

		return true;
	};

protected:
	
	int multiplier;
	int minMaxThrows;
	bool maximize;
	bool isTiled;
	int relax;
	std::string method;
	float radius;
	char *outputPath;
	dunbarPoisson::PDSampler *sampler;
};

} //end namespace utk


#endif
