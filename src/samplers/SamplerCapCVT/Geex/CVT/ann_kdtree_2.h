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

#ifndef __ANN_KDTREE_2__
#define __ANN_KDTREE_2__

#include <Geex/third_party/tsANN/include/ANN/ANN.h>
#include <Geex/third_party/misc/linkage.h>
#include <Geex/mathematics/glsl_linear.h>
#include <vector>

namespace Geex {

	class GEEX_API ANNKdTree_2 {
	public:
		ANNKdTree_2(const double *data, int N, int neighboursize = 1) ;
		~ANNKdTree_2();

		void queryNearestNeighbor(const double pt[2], int neighborsize, std::vector<int>& indices);
		int  queryNearestNeighbor(double x, double y);
		int  queryNearestNeighbor(double x, double y, double& dist);
		double queryNearestDist(const double *p);
		double queryNearestDist(const vec2& p);

	private:
		ANNkd_tree*    m_kdtree;
		ANNpointArray  m_points;
		double         m_eps;
	};

}

#endif //
