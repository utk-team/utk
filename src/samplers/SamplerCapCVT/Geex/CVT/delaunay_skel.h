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

#ifndef __GEEX_CVT_DELAUNAY_SKEL__
#define __GEEX_CVT_DELAUNAY_SKEL__

#include <Geex/CVT/containers.h>
#include <Geex/CVT/geometry.h>
#include <stack>

namespace Geex {

    /**
     * DelaunaySkeleton stores the vertex graph of a Delaunay triangulation
     * and the equations of the bisectors associated with each primal edge.
     * All information is stored in a Compressed Row Storage array.
     */
    class DelaunaySkeleton {
    public:
        DelaunaySkeleton() {
            star_ptr_.push_back(0) ;
        }

        void clear() {
            // Note: we use resize(0) instead of clear()
            // since this is guaranteed to keep the reserved
            // memory (this is what we want since we keep 
            // clearing and filling the same DelaunaySkeleton).
            star_ptr_.resize(0) ; 
            neighbor_.resize(0) ; 
            bisector_.resize(0) ;
            star_ptr_.push_back(0) ;
        }

        unsigned int nb_vertices() const { return (unsigned int)star_ptr_.size() - 1 ; }
        unsigned int star_begin(unsigned int i) const {
            return star_ptr_[i] ;
        }
        unsigned int star_end(unsigned int i) const {
            return star_ptr_[i+1] ;
        }
        unsigned int nb_neighbors(unsigned int i) const {
            return star_end(i) - star_begin(i) ;
        }
        unsigned int neighbor(unsigned int j) const {
            return neighbor_[j] ;
        }
        const Plane<real>& bisector(unsigned int j) const {
            return bisector_[j] ;
        }
        unsigned int neighbor(unsigned int i, unsigned int j) const {
            gx_debug_assert(j < nb_neighbors(i)) ;
            return neighbor(star_begin(i) + j) ;
        }
        const Plane<real>& bisector(unsigned int i, unsigned int j) const {
            gx_debug_assert(j < nb_neighbors(i)) ;
            return bisector(star_begin(i) + j) ;
        }
        
        void begin_star() { }
        void add_to_star(unsigned int neighbor, const Plane<real>& bisector) {
            neighbor_.push_back(neighbor) ;
            bisector_.push_back(bisector) ;
        }
        void end_star() {
            star_ptr_.push_back((unsigned int)neighbor_.size()) ;
        }

        // Used internally by copy_Delaunay_to_skel
        bool last_star_has_vertex(unsigned int i) const {
            for(
                unsigned int k = star_ptr_[star_ptr_.size() - 1]; k < neighbor_.size(); k++) {
                if(neighbor_[k] == i) { return true ; }
            }
            return false ;
        }


    private:
        std::vector<unsigned int> star_ptr_ ;
        std::vector<unsigned int> neighbor_ ;
        std::vector<Plane<real> > bisector_ ;
    } ;

//===========================================================================================


} 

#endif
