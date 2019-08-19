/*
 * Hélène Perrier helene.perrier@liris.cnrs.fr
 * and David Coeurjolly david.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2018 CNRS Université de Lyon
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the UTK project.
 */
#ifndef _UTK_MINIMALDISTANCE_HPP_
#define _UTK_MINIMALDISTANCE_HPP_

#include "../pointsets/Pointset.hpp"

namespace utk
{

class MinimalDistance
{
public:
	template <uint D, typename T, typename P>
	bool compute(Pointset<D, T, P>& arg_pts, bool toric, double& minDist)
	{
		if( (arg_pts.toricity == 0) && toric)
			WARNING("MinimalDistance::compute computing the toric minimal distance on a non toric pointset...");
		
		minDist = -1;
		for(int i=0; i<arg_pts.size(); i++)
		for(int j=0; j<arg_pts.size(); j++)
		{
			if(i==j) continue;
			
			double dist = distance<D, T, P>(arg_pts, toric, arg_pts[i], arg_pts[j]);
			if(dist < minDist || minDist < 0)
				minDist = dist;
		}
		
		return true;
	}
	
private:
	
	template <uint D, typename T, typename P>
	double distance(Pointset<D, T, P>& arg_pts, bool toric, P pts0, P pts1)
	{
		double dist_val = 0;
		for(int d=0; d<D; d++)
		{
			double domain_size = arg_pts.domain.pMax.pos()[d] - arg_pts.domain.pMin.pos()[d];
			
			double p0 = (double)(pts0.pos()[d]);
			double p1 = (double)(pts1.pos()[d]);
			
			double diff = p0-p1;
			if(toric)
			{
				if(domain_size == 0)
					WARNING("MinimalDistance::distance,  Domain size = 0, no toricity");
				else
					diff = std::min(domain_size - std::fabs(diff), std::fabs(diff));
			}
			dist_val += diff*diff;
		}
		
		return sqrt(dist_val);
	}
};

}//end namespace utk

#endif