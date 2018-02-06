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

#ifndef __GEEX_CVT_SYMBOLIC_VERTEX__
#define __GEEX_CVT_SYMBOLIC_VERTEX__

#include <Geex/CVT/containers.h>

namespace Geex {

    typedef Numeric::uint64 SymbolicVertexKey ;

    typedef union {
        SymbolicVertexKey k ;
        Numeric::int32 words[2] ;
    } SymbolicVertexKeyAccess ;


    // This set of three integers encodes the equation of this vertex.
    // * Each positive entry i denotes the bisector of the segment that connects
    //   the center vertex to the i-th vertex (note that the center vertex
    //   needs to be stored elsewhere, but is known when a TriMesh is used,
    //   since we know which dual cell we are processing).
    // * Each negative entry i denotes the i-th face in the boundary TriMesh.
    //   Note: indexing starts with 1 (resp. -1), 0 is kept for error codes.

    template <int N> class GenericSymbolicVertex : public small_set<int,N> {
    public:
        typedef small_set<int,N> baseclass ;
        typedef baseclass Sym ;
        // Returns a key formed of two integers packed into a single 64 bits integers.
        // They are used for chaining the boundary edges in a table (note that only
        // two equations are needed: since we just computed the intersection with a 
        // plane, all the boundary edges have this plane in their equation).
        SymbolicVertexKey key(int E) const {
            // Will not work with degenerate cases.
            gx_debug_assert(baseclass::size() == 3) ;
            SymbolicVertexKeyAccess result ;
            if((*this)[0] == E) {
                result.words[0] = (*this)[1] ;
                result.words[1] = (*this)[2] ;
            } else if((*this)[1] == E) {
                result.words[0] = (*this)[0] ;
                result.words[1] = (*this)[2] ;
            } else {
                gx_debug_assert((*this)[2] == E) ;
                result.words[0] = (*this)[0] ;
                result.words[1] = (*this)[1] ;
            }
            return result.k ;
        }

        void add_bisector(int i) {
            baseclass::insert(i+1) ;
        }

        void add_boundary_facet(int i) {
            baseclass::insert(-i-1) ;
        }

        unsigned int nb_boundary_facets() const {
            unsigned int result = 0 ;
            for(
                typename baseclass::const_iterator it = baseclass::begin(); 
                it != baseclass::end() && *it<0; it++
            ) {
                result++ ;
            }
            return result ;
        }

        unsigned int nb_bisectors() const {
            unsigned int result = 0 ;
            for(
                typename baseclass::const_iterator it = baseclass::end() - 1; 
                it != baseclass::begin() - 1 && *it>0; it--
            ) {
                result++ ;
            }
            return result ;
        }

        unsigned int bisector(int i) const {
            gx_debug_assert(i < (int)nb_bisectors()) ;
            return (baseclass::end()[-1-i])-1 ;
        }

        unsigned int boundary_facet(int i) const {
            gx_debug_assert(i < (int)nb_boundary_facets()) ;
            return -(baseclass::begin()[i])-1 ;
        }

        bool has_bisector(int i) const {
            return baseclass::find(i+1) != baseclass::end() ;
        }

        bool has_boundary_facet(int i) const {
            return baseclass::find(-i-1) != baseclass::end() ;
        }
    } ;

    typedef GenericSymbolicVertex<3> SymbolicVertex ;

    inline bool operator<(const SymbolicVertex::Sym& k1, const SymbolicVertex::Sym& k2) { 
        if(k1[0] < k2[0]) { return true ;  }
        if(k1[0] > k2[0]) { return false ; }        
        if(k1[1] < k2[1]) { return true ;  }
        if(k1[1] > k2[1]) { return false ; }        
        if(k1[2] < k2[2]) { return true ;  }
        return false ;
    }

    inline bool operator==(
        const SymbolicVertex::Sym& k1, const SymbolicVertex::Sym& k2
    ) { 
        return 
            k1[0] == k2[0] &&
            k1[1] == k2[1] &&
            k1[2] == k2[2] ;
    }

    //__________________________________________________________________

    class vec3Sym : public vec3 {
    public:
        vec3Sym(
            const vec3& v, const SymbolicVertex& k
        ) : vec3(v), sym(k) {
        }
        vec3Sym(const vec3& v) : vec3(v) { }
        vec3Sym() { }
        SymbolicVertex sym ;
    } ;

}

#endif
