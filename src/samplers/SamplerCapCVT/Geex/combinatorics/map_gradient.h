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

#ifndef __GEEX_COMBINATORICS_MAP_GRADIENT__
#define __GEEX_COMBINATORICS_MAP_GRADIENT__

#include <Geex/combinatorics/map.h>
#include <Geex/combinatorics/map_properties.h>

namespace Geex {


    class GEEX_API MapGradient {
    public:
        MapGradient(Map* map) ;

        void compute_tangent_and_binormal() ;

        void compute_tangent_and_binormal(
            MapVertexProperty<vec3>& tangent,
            MapVertexProperty<vec3>& binormal
        ) ;

        /** 
         * returns the interpolated directional derivative relative to W
         * at vertex v.
         */
        vec3 compute_gradient(Map::Vertex* v, const vec2& w) ;


        /** 
         * returns the directional derivative relative to W in
         * the triangle ( h1->vertex(), h2->vertex(), h3->vertex() ).
         */
        vec3 compute_gradient(
            Map::Halfedge* h1,
            Map::Halfedge* h2,
            Map::Halfedge* h3,
            const vec2& W
        ) ;


    protected:
        
        void compute_gradient(
            const vec3& p1, const vec3& p2, const vec3& p3,
            const vec2& q1, const vec2& q2, const vec2& q3,
            double* TU,
            double* TV
        ) ;

        void compute_gradient(
            Map::Halfedge* h1,
            Map::Halfedge* h2,
            Map::Halfedge* h3,
            double* TU,
            double* TV
        ) ;

    private:
        Map* map_ ;
        MapHalfedgeProperty<Map::TexVertex_var> halfedge_tex_vertex_ ;
    } ;
} 

#endif
