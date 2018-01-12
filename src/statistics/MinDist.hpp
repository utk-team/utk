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