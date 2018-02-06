/*
*  _____ _____ ________  _
* /  __//  __//  __/\  \//
* | |  _|  \  |  \   \  / 
* | |_//|  /_ |  /_  /  \ 
* \____\\____\\____\/__/\\
*
* Graphics Environment for EXperimentations.
*  Copyright (C) 2006 INRIA - Project ALICE
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*  If you modify this software, you should include a notice giving the
*  name of the person performing the modification, the date of modification,
*  and the reason for such modification.
*
*  Contact: 
*
*     ALICE Project - INRIA
*     INRIA Lorraine, 
*     Campus Scientifique, BP 239
*     54506 VANDOEUVRE LES NANCY CEDEX 
*     FRANCE
*
*  Note that the GNU General Public License does not permit incorporating
*  the Software into proprietary programs. 
*/

#include "ann_kdtree_2.h"
#include <string.h>

namespace Geex {

	ANNKdTree_2::ANNKdTree_2(const double *data, int N, int neighboursize) 
	{
		m_eps = 0;
		m_points = annAllocPts(N, 2);	
		memcpy(&m_points[0][0], data, sizeof(double)*2*N);
		m_kdtree = new ANNkd_tree(m_points, N, 2);
	}

	ANNKdTree_2::~ANNKdTree_2()
	{
		annDeallocPts(m_points);
		delete m_kdtree;
		annClose() ;
	}

	void ANNKdTree_2::queryNearestNeighbor(const double pt[2], int neighborsize, std::vector<int>& indices)
	{
		std::vector<ANNdist>  m_dist(neighborsize);
		indices.resize(neighborsize);
		double xyz[2];
		xyz[0] = pt[0]; xyz[0] = pt[1]; //xyz[0] = pt[2];
		m_kdtree->annkSearch(xyz, neighborsize, &indices[0], &m_dist[0], m_eps);
	}

	int ANNKdTree_2::queryNearestNeighbor(double x, double y)
	{
		double xyz[2] ;
		xyz[0] = x ; xyz[1] = y ; // xyz[2] = z ;
		int idx ;
		double dist ;
		m_kdtree->annkSearch(xyz, 1, &idx, &dist, m_eps);
		return idx ;
	}

	int ANNKdTree_2::queryNearestNeighbor(double x, double y, double& dist)
	{
		double xyz[2] ;
		xyz[0] = x ; xyz[1] = y ; //xyz[2] = z ;
		int idx ;
		m_kdtree->annkSearch(xyz, 1, &idx, &dist, m_eps);
		return idx ;
	}

	double ANNKdTree_2::queryNearestDist(const double* p)
	{
		int idx ;
		double dist;
		double xyz[2];
		xyz[0] = p[0]; xyz[1] = p[1]; //xyz[0] = p[2];
		m_kdtree->annkSearch(xyz, 1, &idx, &dist, m_eps);
		return dist ;
	}

	double ANNKdTree_2::queryNearestDist(const vec2& p){
		int idx ;
		double dist;
		double xyz[2];
		xyz[0] = p[0]; xyz[1] = p[1]; //xyz[0] = p[2];
		m_kdtree->annkSearch(xyz, 1, &idx, &dist, m_eps);
		return dist ;
	}
}

