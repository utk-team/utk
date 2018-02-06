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

#include <Geex/CVT/delaunay.h>
#include <Geex/CVT/delaunay_CGAL.h>
#include <Geex/CVT/regular_CGAL.h>

namespace Geex {

    Delaunay::Delaunay() : skeleton_dirty_(false), nb_vertices_(0) {
        vertices_ = nil ;
        aniso_ = nil ;
        aniso_metric_ = 0;
        m_kdtree_ = 0;
        weight_ = nil;
    }
    
    Delaunay::~Delaunay() {
        destroy_kdtree();
    }

    Delaunay* Delaunay::create(const std::string& name) {
        std::cerr << "Creating Delaunay implementation, using " << name << std::endl ;
        if(name == "CGAL") {
            return new Delaunay_CGAL(false) ;
        } else if(name == "CGAL_spatial_sort") {
            return new Delaunay_CGAL(true) ;
        } else if(name == "CGAL_regular"){
            return new Regular_CGAL(false); 
        } else if(name == "CGAL_regular_sort"){
            return new Regular_CGAL(true); 
        } else if(name == "ANN") {
            return new AnnDelaunay ;
        }
        else {
            std::cerr << name << ": no such Delaunay implementation." << std::endl ;
            return 0 ;
        }
    }

    void Delaunay::set_anisotropy(unsigned int n, const double* aniso, int aniso_metric) {
        aniso_ = aniso ;
        aniso_metric_ = aniso_metric;
    }

    void Delaunay::set_weight(unsigned int n, const double* weight){
        weight_ = weight;
    }

    void Delaunay::get_tetras(std::vector<int>& tetras, bool finite_only) {
        // Not implemented in baseclass
        gx_assert_not_reached ;
        return ;
    }
	void Delaunay::get_tetras(std::vector<int>& tetras, std::vector<int>& neighbors, std::vector<vec3>& duals) {
	    // Not implemented in baseclass
        gx_assert_not_reached ;
        return ;
	}

    void  Delaunay::get_facets(std::vector<int>& facets, bool finite_only) {
        // Not implemented in baseclass
        gx_assert_not_reached ;
        return ;
    }
    
    void Delaunay::create_kdtree()
    {
        destroy_kdtree();
        if (m_kdtree_ == 0 && vertices_)
        {
            m_kdtree_ = new ANNKdTree_3((double *)vertices_, nb_vertices_);
        }
    }

    void Delaunay::destroy_kdtree()
    {
        if (m_kdtree_)
        {
            delete m_kdtree_;
            m_kdtree_ = 0;
        }
    }


    void Delaunay::get_voronoi_cell(unsigned int i, VoronoiCell& cell, bool geometry, bool use_cell_id) {
        // Not implemented in baseclass
        gx_assert_not_reached ;
        return ;
    }

	unsigned int Delaunay::get_number_of_finite_vertices() const{
		return 0;
	}
	unsigned int Delaunay::get_number_of_finite_cells() const{
		return 0;
	}


//=======================================================================

    void AnnDelaunay::set_vertices(unsigned int n, const double* xyz) {
        vertices_ = xyz ;
        nb_vertices_ = n ;
        destroy_kdtree() ;
        create_kdtree() ;
    }
    
    void AnnDelaunay::set_anisotropy(unsigned int n, const double* aniso, int aniso_metric) {
        gx_assert_not_reached ;
    }
    
    void AnnDelaunay::set_weight(unsigned int n, const double* weight) {
        gx_assert_not_reached ;
    }
    
    unsigned int AnnDelaunay::nearest_vertex_id(double x, double y, double z) const {
        gx_assert_not_reached ;
        return 0;
    }
    
    void AnnDelaunay::update_skeleton() const {
        gx_assert_not_reached ;
    }

}
