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

#ifndef __GEEX_CVT_VORONOI_CELL__
#define __GEEX_CVT_VORONOI_CELL__

#include <Geex/mathematics/geometry.h>
#include <vector>

namespace Geex {

    /**
     * VoronoiCell stores the dual facets in a Compressed Row Storage array.
     * - Each facet knows the bisector it is on, and the list of vertices/edges.
     *    - Each vertex knows the tet it is dual to.
     *    - Each edge knows the other bisector it is on (an edge is defined as the
     * intersection between the facet bisector and the edge bisector).
     */
    class VoronoiCell {
    public:
        VoronoiCell() { facet_ptr_.push_back(0) ; }
        void clear() {
            facet_ptr_.resize(0) ;
            facet_bisector_.resize(0) ;
            edge_bisector_.resize(0) ;
            vertex_.resize(0) ;
            infinite_.resize(0) ;
            facet_ptr_.push_back(0) ;
			cell_id_.resize(0);
        }

        unsigned int nb_facets() const { return (unsigned int)facet_ptr_.size() - 1 ; }

        unsigned int facet_begin(unsigned int f) const {
            gx_debug_assert(f < nb_facets()) ;
            return facet_ptr_[f] ;
        }

        unsigned int facet_end(unsigned int f) const {
            gx_debug_assert(f < nb_facets()) ;
            return facet_ptr_[f+1] ;
        }

        unsigned int nb_vertices(unsigned int f) const {
            gx_debug_assert(f < nb_facets()) ;
            return facet_end(f) - facet_begin(f) ;
        }

        unsigned int next_around_facet(unsigned int f, unsigned int i) const {
            gx_debug_assert(i >= facet_begin(f) && i < facet_end(f)) ;
            return (i+1 == facet_end(f) ? facet_begin(f) : i+1) ;
        }

        unsigned int prev_around_facet(unsigned int f, unsigned int i) const {
            gx_debug_assert(i >= facet_begin(f) && i < facet_end(f)) ;            
            return (i == facet_begin(f) ? facet_end(f)-1 : i-1) ;
        }

        unsigned int facet_bisector(unsigned int f) const {
            gx_debug_assert(f < nb_facets()) ;
            return facet_bisector_[f] ;
        }

        int edge_bisector(unsigned int i) const {
            gx_debug_assert(i < edge_bisector_.size()) ;
            return edge_bisector_[i] ;
        }

        const vec3& vertex(unsigned int i) const {
            gx_debug_assert(i < vertex_.size()) ;
            return vertex_[i] ;
        }

        bool vertex_is_infinite(unsigned int i) const {
            gx_debug_assert(i < infinite_.size()) ;
            return infinite_[i] ;
        }

		const int cell_id(unsigned int i) const {
			return i < cell_id_.size() ? cell_id_[i] : -1;
		}

        void begin_facet(unsigned int f_bisector) {
            facet_bisector_.push_back(f_bisector) ;
        }

        void add_to_facet(
            int e_bisector, const vec3& v, bool infinite
        ) {
            edge_bisector_.push_back(e_bisector) ;
            vertex_.push_back(v) ;
            infinite_.push_back(infinite) ;
        }

		void add_to_facet(
			int e_bisector, const vec3& v, bool infinite, int cell_id
			) {
				edge_bisector_.push_back(e_bisector) ;
				vertex_.push_back(v) ;
				infinite_.push_back(infinite) ;
				cell_id_.push_back(cell_id);
		}

        void add_to_facet(
            int e_bisector, bool infinite, int cell_id
        ) {
            edge_bisector_.push_back(e_bisector) ;
            infinite_.push_back(infinite) ;
			cell_id_.push_back(cell_id);
        }

		void add_to_facet(
			int e_bisector, bool infinite
			) {
				edge_bisector_.push_back(e_bisector) ;
				infinite_.push_back(infinite) ;
		}

        void end_facet() {
            facet_ptr_.push_back((unsigned int)edge_bisector_.size()) ;            
        }

        unsigned int find_facet(unsigned int bisector) {
            for(unsigned int i=0; i<facet_bisector_.size(); i++) {
                if(facet_bisector_[i] == bisector) {
                    return i ;
                }
            }
            std::cerr << "bisector = " << bisector ;
            std::cerr << "facet = [" ;
            for(unsigned int i=0; i<facet_bisector_.size(); i++) {
                std::cerr << facet_bisector_[i] << " " ;
            }
            std::cerr << "]" << std::endl ;
            gx_assert_not_reached ;
            return 0 ;
        }

    private:
        std::vector<unsigned int> facet_ptr_ ;
        std::vector<unsigned int> facet_bisector_ ;
        std::vector<int> edge_bisector_ ;
        std::vector<vec3> vertex_ ;
        std::vector<bool> infinite_ ; 
		std::vector<int> cell_id_;
    } ;

}

#endif
