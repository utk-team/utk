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
 
 
 
 
 
 
 

#include <Geex/combinatorics/map_combels.h>

namespace Geex {

//_________________________________________________________

    namespace MapCombels {

        //______________________________________

        void Vertex::assert_is_valid() const {
            gx_assert(halfedge()-> vertex() == this) ;
        }

        size_t Vertex::degree() const {
            size_t result = 0 ;
            Halfedge* it = halfedge() ;
            do {
                result++ ;
                it = it->next_around_vertex() ;
            } while(it != halfedge()) ;
            return result ;
        }

        bool Vertex::is_on_border() const {
            Halfedge* it = halfedge() ;
            do {
                if(it->is_border()) {
                    return true ;
                }
                it = it->next_around_vertex() ;
            } while(it != halfedge()) ;
            return false ;
        }

        //______________________________________

        void Halfedge::assert_is_valid() const {
            gx_assert(opposite()-> opposite() == this) ;
            gx_assert(next()-> prev() == this) ;
            gx_assert(prev()-> next() == this) ;
        }

        //______________________________________

        size_t Facet::degree() const {
            size_t result = 0 ;
            Halfedge* it = halfedge() ;
            do {
                result++ ;
                it = it->next() ;
            } while(it != halfedge()) ;
            return result ;
        }

        bool Facet::is_triangle() const {
            return ( halfedge()-> next()-> next()-> next() == halfedge() ) ;
        }

        bool Facet::is_on_border() const {
            Halfedge* it = halfedge() ;
            do {
                if(it->opposite()->is_border()) {
                    return true ;
                }
                it = it->next() ;
            } while(it != halfedge()) ;
            return false ;
        }

        void Facet::assert_is_valid() const {
            gx_assert(halfedge()-> facet() == this) ;
            gx_assert(nb_edges() > 2) ;
        }

		real Facet::area() const {
			double area = 0 ;
			Halfedge* h =halfedge() ;
			Vertex*   v0 = h->vertex() ;
			do {
				Vertex*   v1 = h->next()->vertex() ;
				Vertex*   v2 = h->next()->next()->vertex() ;
				area += 0.5*cross(v2->point()-v0->point(), v1->point()-v0->point()).length() ; 
				h = h->next() ;
			} while(h->next()!=halfedge()) ;
			return area ;
		}

		vec3 Facet::normal() const {
			return normalize(cross(
				halfedge()->next()->vertex()->point()-halfedge()->vertex()->point(), 
				halfedge()->prev()->vertex()->point()-halfedge()->vertex()->point()
				)) ;
			
		}
        //______________________________________

    }

//_________________________________________________________

}

