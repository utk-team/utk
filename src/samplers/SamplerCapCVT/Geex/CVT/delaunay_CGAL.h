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

#ifndef __GEEX_CVT_DELAUNAY_CGAL__
#define __GEEX_CVT_DELAUNAY_CGAL__

#include <Geex/CVT/delaunay.h>

namespace Geex {

    class GEEX_API Delaunay_CGAL : public Delaunay {
    public:
        Delaunay_CGAL(bool use_spatial_sort) ;
        virtual ~Delaunay_CGAL() ;
        virtual void set_vertices(unsigned int n, const double* xyz) ;
        virtual void update_skeleton() const ;
        virtual unsigned int nearest_vertex_id(double x, double y, double z) const ;
        virtual void get_tetras(std::vector<int>& tetras, bool finite_only) ;
        virtual void get_facets(std::vector<int>& facets, bool finite_only) ;
        virtual void get_voronoi_cell(unsigned int i, VoronoiCell& cell, bool geometry = true, bool use_cell_id = false) ;
		virtual unsigned int get_number_of_finite_vertices() const;
		virtual unsigned int get_number_of_finite_cells() const;
	protected:
		void set_cells_id();
    protected:
        class Implementation ;
        Implementation* impl_ ;
        bool use_spatial_sort_ ;
		bool cell_id_tag_ ;
    } ;

} 

#endif

