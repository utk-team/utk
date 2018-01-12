#ifndef _UTK_SAMPLER_HEXAGONALGRID_
#define _UTK_SAMPLER_HEXAGONALGRID_

#include "../utils.hpp"
#include "../pointsets/Pointset.hpp"
#include <cstdlib>


namespace utk
{

class SamplerHexagonalGrid
{
public:

    SamplerHexagonalGrid() { }

    template<unsigned int D, typename T, typename P>
	bool generateSamples(Pointset<D, T, P>& arg_pts, unsigned long long int arg_points)
    {
		if (!arg_pts.empty())
		{
			WARNING("SamplerHexagonalGrid::generateSamples the pointset to fill is not empty, clearing it ...");
			arg_pts.clear();
		}
		
		if (D != 2)
		{
			WARNING("SamplerHexagonalGrid::generateSamples is only a 2D sampler ...");
			return false;
		}

		if (!isFloatingType<T>())
		{
			ERROR("SamplerHexagonalGrid::generateSamples generates samples in [0, 1]^d, can only run with points of type float or double");
			return false;
		}
		
        int n = sqrt(arg_points);
        if (n*n != arg_points)
            std::cerr << "The number of samples required from hexagonal sampling (" << arg_points << ") is not an exact square ! Rounding to " << n*n << std::endl;
        arg_points=n*n;
		
		for(uint d=0; d<D; d++)
		{
			arg_pts.domain.pMin.pos()[d] = 0;
			arg_pts.domain.pMax.pos()[d] = 1;
		}
		arg_pts.toricity = 1;

        double s = sqrt(2.0 / (3.0*arg_points*sqrt(3.0)));

        int i=0;
        double x=0, y=0;
        while(x<1 && y<1)
        {
            if (x >= 0)
            {
                /*double pt[D] = {0};
                pt[0]=x;
                pt[1]=y;*/
				Point<D, T> pt;
				pt.pos()[0] = x;
				pt.pos()[1] = y;
                arg_pts.push_back(pt);
            }
            x+=sqrt(3.0)*s;
            if(x > 1)
            {
                i++;
                x = -(i%2)*(sqrt(3.0)*s)/2.0;
                y+=3.0/2.0 * s;
            }
        }
        
        return true;
    }
};

} //end namespace utk


#endif
