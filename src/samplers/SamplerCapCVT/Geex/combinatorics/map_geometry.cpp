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
 

#include <Geex/combinatorics/map_geometry.h>

namespace Geex {

    namespace Geom {

		Box3d GEEX_API get_map_bbox(Map* map) {
			Box3d b ;
            FOR_EACH_VERTEX(Map, map, it) {
                const Geex::vec3& p = it->point() ;
				b.add_point(p) ;
            }
			return b ;
		}

        void get_map_bbox(
            Map* map, 
            real& xmin, real& ymin, real& zmin, real& xmax, real& ymax, real& zmax
        ) {
            xmin = ymin = zmin = 1e30f ;
            xmax = ymax = zmax = -1e30f ;
            FOR_EACH_VERTEX(Map, map, it) {
                const Geex::vec3& p = it->point() ;
                xmin = Geex::gx_min(xmin, p.x) ;
                ymin = Geex::gx_min(ymin, p.y) ;
                zmin = Geex::gx_min(zmin, p.z) ;
                xmax = Geex::gx_max(xmax, p.x) ;
                ymax = Geex::gx_max(ymax, p.y) ;
                zmax = Geex::gx_max(zmax, p.z) ;
            }
        }

        void normalize_map_geometry(Map* map) {
            real xmin, ymin, zmin, xmax, ymax, zmax ;
            get_map_bbox(map, xmin, ymin, zmin, xmax, ymax, zmax) ;
            real scal = real(0.5 * ::sqrt(gx_sqr(xmax - xmin) + gx_sqr(ymax - ymin) + gx_sqr(zmax - zmin))) ;
            scal = real(1.0 / scal) ;
            vec3 center(real(0.5 * (xmin + xmax)), real(0.5 * (ymin + ymax)), real(0.5 * (zmin + zmax))) ;
            FOR_EACH_VERTEX(Map, map, it) {
                it->set_point( scal * (it->point() - center) ) ;
            }
        }

        vec3 facet_vertices_barycenter(const Map::Facet* f) {
            real x=0,y=0,z=0,count=0 ;
            const Map::Halfedge* h = f->halfedge() ;
            do {
                x += h->vertex()->point().x ;
                y += h->vertex()->point().y ;
                z += h->vertex()->point().z ;
                count++ ;
                h = h->next() ;
            } while(h != f->halfedge()) ;
            return vec3(
                x/count, y/count,z/count
            ) ;
        }

        vec3 facet_barycenter(const Map::Facet* f){
            real x=0 ; real y=0 ; real z=0 ;
            int nb_vertex = 0 ;
            Map::Halfedge* cir = f->halfedge();
            do {
                nb_vertex++;
                x+= cir->vertex()->point().x ;
                y+= cir->vertex()->point().y ;
                z+= cir->vertex()->point().z ;
                cir = cir->next();
            } while (cir != f->halfedge());
            return  vec3(
                x / real(nb_vertex),
                y / real(nb_vertex),
                z / real(nb_vertex)
            ) ;
        }


        real average_star_edge_length(const Map::Vertex* v) {
            int cnt = 0 ;
            real result = 0.0 ;
            Map::Halfedge* h = v->halfedge() ;
            do {
                result += length(Geom::vector(h)) ;
                cnt++ ;
                h = h->next_around_vertex() ;
            } while(h != v->halfedge()) ;
            return result / real(cnt) ;
        }

       
        real facet_area(const Map::Facet* f) {
            real result = 0 ;
            Map::Halfedge* h = f->halfedge() ;
            const vec3& p = h->vertex()->point() ;
            h = h->next() ;
            do {
                result += triangle_area(
                    p,
                    h->vertex()->point(),
                    h->next()->vertex()->point() 
                ) ;
                h = h->next() ;
            } while(h != f->halfedge()) ;
            return result ;
        }   

		real map_area(const Map* m) {
			real area = 0 ;

			FOR_EACH_FACET_CONST(Map, m, f) {
				area += facet_area(f) ;
			}
			return area ;
		}

    }
   
}
