#ifndef _UTK_SAMPLER_CAPCVT_
#define _UTK_SAMPLER_CAPCVT_

#include "../pointsets/Pointset.hpp"
#include "SamplerCapCVT/gx_capcvt2d/main.hpp"
#include <cstdlib>
#include <random>
#include <chrono>

#include "../utils.hpp"

namespace utk
{

class SamplerCapCVT
{
protected:
public:

	SamplerCapCVT() : capcvt(1, NULL) { }

	void setBoundaryFile(std::string boundary_file) { capcvt.boundary_filename_ = boundary_file; }
	void setLambda(float lambda) { capcvt.nb_lambda_ = lambda; }
	void setIter(uint iter) { capcvt.nb_iter_ = iter; }

	template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned int arg_points)
	{
		if (!arg_pts.empty())
		{
			WARNING("SamplerCapCVT::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}

		//load domain & toricity
		for(uint d=0; d<D; d++)
		{
			arg_pts.domain.pMin.pos()[d] = 0;
			arg_pts.domain.pMax.pos()[d] = 1;
		}
		arg_pts.toricity = 1;
		
		capcvt.nb_points_ = arg_points;
		
		srand(time(NULL));
	
		std::cout << "Using boundary file " << capcvt.boundary_filename_ << std::endl;
		capcvt.init_scene();
		capcvt.reset();
		capcvt.NewtonLloyd();
		std::vector<double> samples = capcvt.get_samples();

		int n=0;
		arg_pts.resize(arg_points);
		for(uint i=0; i<arg_points; i++)
			for(uint d=0; d<D; d++)
				arg_pts[i].pos()[d] = samples[n++];

		return true;
	};

protected:

    Geex::CVTApp capcvt;
};

} //end namespace utk


#endif
