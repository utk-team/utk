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
 

#ifndef __GEEX_COMBINATORICS_MAP_GEOMETRY__
#define __GEEX_COMBINATORICS_MAP_GEOMETRY__

#include <Geex/mathematics/geometry.h>
#include <Geex/combinatorics/map.h>

namespace Geex {

    namespace Geom {

        Box3d GEEX_API get_map_bbox(Map* map) ;
        void GEEX_API get_map_bbox(
            Map* map, 
            real& xmin, real& ymin, real& zmin, real& xmax, real& ymax, real& zmax
        ) ;

        void GEEX_API normalize_map_geometry(Map* map) ;

        inline vec3 vector(const Map::Halfedge* h) {
            return h->vertex()->point() - h->opposite()->vertex()->point() ;
        }

        /**
         * returns the normalized facet normal.
         */
        inline vec3 facet_normal(const Map::Facet* f) {
            vec3 result(0,0,0) ;
            const Map::Halfedge* h = f->halfedge() ;
            const Map::Halfedge* cur = h ;
            do {
                result += cross(vector(cur),vector(cur->next())) ;
                cur = cur->next() ;
            } while(cur != h) ;
	    double l = length(result) ;
	    if(l < 1e-30) { return result ; }
            return (1.0 / l) * result ;
        }

        inline vec3 vertex_normal(const Map::Vertex* v) {
            vec3 result(0,0,0) ;

            const Map::Halfedge* h = v->halfedge() ;
            const Map::Halfedge* cur = h ;
            if(v->degree() == 2) {
                result = cross(vector(cur),vector(cur->next())) ;                
            } else {
                do {
                    result += cross(vector(cur),vector(cur->next())) ;
                    cur = cur->next_around_vertex() ;
                } while(cur != h) ;
            }
            return normalize(result) ;
        }

        /**
         * returns the barycenter of the vertices of the facet incident to the
         * specified halfedge. Note: this is different from the barycenter of 
         * the facet (but faster to compute).
         */
        vec3 GEEX_API facet_vertices_barycenter(const Map::Facet* f) ;

        inline vec3 edge_barycenter(const Map::Halfedge* h) {
            return mix(
                h->vertex()->point(),
                h->opposite()->vertex()->point(),
                0.5f
            ) ;
        }
        
        vec3 GEEX_API facet_barycenter(const Map::Facet* f) ;

        real GEEX_API average_star_edge_length(const Map::Vertex* v) ;

        real GEEX_API facet_area(const Map::Facet* f) ;

        inline double edge_length(const Map::Halfedge* h) {
            return length(vector(h)) ;
        }

		real GEEX_API map_area(const Map* m) ;
    }

}

#endif

