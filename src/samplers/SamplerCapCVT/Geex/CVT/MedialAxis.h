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
#ifndef MEDIALAXIS_H
#define MEDIALAXIS_H


#include <Geex/mathematics/glsl_linear.h>
#include <list>

namespace Geex 
{
    class ANNKdTree_3 ;

    class GEEX_API MedialAxis 
    { 
    public:
        MedialAxis(const std::vector<vec3>& vertices, 
            const double min_dihedral_angle = 0.01 /*degree*/, 
            const double angle_condition = 22.5 /*degree*/, 
            const double ratio_condition = 8.0);
        ~MedialAxis();
        real squared_lfs(const vec3& p) ;
        real squared_lfs(const vec3& p, vec3& foot) ;
        void export_medial_axis_as_OBJ(const char filename[]);
        void export_medial_axis(const char filename[]);
        void import_medial_axis(const char filename[]);
		std::vector<vec3>& poles() { return used_poles ; }
		std::vector<double>& pole_radius() { return used_pole_radius ; }
		void filter_poles(double scale, std::vector<vec3>& filtered_poles, std::vector<vec3>& filtered_radius) ;

    protected:
        class Implementation ;
        Implementation* build_dt();
        void compute_poles(Implementation* dt);
        void compute();
        void construct_medial_axis(Implementation* dt);

    protected:
        const std::vector<vec3>& vertices_;
        double min_dihedral_angle_;
        double angle_condition_;
        double ratio_condition_;
        ANNKdTree_3* m_kdtree;
        std::vector<vec3> used_poles;
		std::vector<double> used_pole_radius ; 
        std::vector< std::list<int> > medialaxis_voronoi_facets;
        std::vector< std::list<int> > vertice_to_voronoi_faces;
    };
}

#endif