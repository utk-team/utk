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
#ifndef _UTK_PCF_HPP_
#define _UTK_PCF_HPP_

#include "../pointsets/Pointset.hpp"

/*
 * Code from David Coeurjolly (david.coeurjolly@liris.cnrs.fr)
 */

namespace utk
{

class MinDist
{
	
public:
	template <uint D, typename T, typename P>
	bool compute(Pointset<D, T, P> arg_pts, double& mindist, bool toric=true)
	{			
		ERROR("MinDist::compute not yet implemented");
		return false;
	}
	
	//first orders the points in stratas and then only compares them to their neighbours for mindist computation
	template <uint D, typename T, typename P>
	bool computeStratified(Pointset<D, T, P> arg_pts, double& mindist, bool toric=true)
	{	
		WARNING("MinDist::computeStratified Untested");
		if(D!=2)
		{
			ERROR("MinDist::computeStratified only take 2D pointsets as input");
			return false;
		}
		
		uint nbpts = arg_pts.size();
		if(nbpts > 128)
		{
			ERROR("MinDist::computeStratified only pointsets of size <= 128 as input");
		}
		
		Point<D, T> pts[128][128];
		uint stratas= sqrt(nbpts);
		for(int i=0; i<nbpts; i++)
			pts[arg_pts[i].pos()[0]/stratas][arg_pts[i].pos()[1]/stratas] = arg_pts[i];
		
		mindist=nbpts;
		for(int i=0; i<stratas; i++)
		for(int j=0; j<stratas; j++)
		{
			for(int neighbourx=-1;neighbourx<=1; neighbourx++)
			for(int neighboury=-1;neighboury<=1; neighboury++)
			{
				if(neighbourx == 0 && neighboury == 0)
					continue;
				
				if (!toric && (neighboury+j < 0 || neighboury+j > stratas))
					continue;
				if (!toric && (neighbourx+i < 0 || neighbourx+i > stratas))
					continue;
				
				double dist = distance<D, T, P>(pts[i][j], pts[(neighbourx+i)%stratas][(neighboury+j)%stratas]);
				if(dist < mindist)
					mindist = dist;
			}
		}
		
		return true;
	}
	
private:
	
	template <uint D, typename T, typename P>
	double distance(bool toric, P pts0, P pts1)
	{
		double dist_val;
		for(int d=0; d<D; d++)
		{
			double p0 = (double)pts0.pos()[d];
			double p1 = (double)pts1.pos()[d];
			
			double diff = p0-p1;
			if(toric)
				diff = std::min(1 - std::abs(diff), std::abs(diff));
			
			dist_val += diff*diff;
		}
		
		return sqrt(dist_val);
	}
	
};

}//end namespace utk

#endif