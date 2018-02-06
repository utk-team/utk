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

#ifndef __GEEX_CVT_RVD_PREDICATES__
#define __GEEX_CVT_RVD_PREDICATES__

#include <Geex/CVT/geometry.h>

namespace Geex {

    class TopoPolyVertexEdge ;
    class TopoPolyMesh ;
    class RestrictedVoronoiDiagram_poly ;
    class vec3Sym ;

    class GEEX_API RVD_predicates {
    public:
        /*
         * returns the side of vertex v with respect
         * to the bisector of [p1,p2].
         * Positive side (resp. negative) is p1's side (resp. p2)
         * v needs to have symbolic information (RVD needs
         * to be in symbolic or exact mode).
         */
        static Sign side(
            const vec3& p1, const vec3& p2,
            const vec3Sym& v,
            const RestrictedVoronoiDiagram_poly* RVD
        ) ;

        static void set_verbose(bool x) ;
        static void begin_stats() ;
        static void end_stats() ;

        // Used to debug code that manipulates symbolic information.
        static vec3 sym_to_geometry(
            unsigned int center_vertex, const vec3Sym& v,
            const RestrictedVoronoiDiagram_poly* RVD
        ) ;

    protected:
        static bool find_edge(
            const TopoPolyMesh* M, int f1, int f2, vec3& q1, vec3& q2
        ) ;

    } ;

}


#endif

