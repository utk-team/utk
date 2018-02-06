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
 
#include <Geex/combinatorics/map_lscm.h>
#include <Geex/combinatorics/map_geometry.h>

namespace Geex {

    LSCM::LSCM() {
        solver_ = new Solver ;
        map_ = nil ;
    }

    void LSCM::parameterize(Map* map) {
        map_ = map ;
        tex_coord_.bind(map, "tex_coord") ;
        vertex_id_.bind(map) ;
        is_locked_.bind(map) ;

        project() ;
        map_to_solver() ;
        setup_conformal_map_relations() ;
        solver_->solve() ;
        solver_to_map() ;

        tex_coord_.unbind() ;
        vertex_id_.unbind() ;
        is_locked_.unbind() ;
        map_ = nil ;
    }

    void LSCM::project() {
        real xmin, ymin, zmin, xmax, ymax, zmax ;
        Geom::get_map_bbox(map_, xmin, ymin, zmin, xmax, ymax, zmax) ;
        
        real dx = xmax - xmin ;
        real dy = ymax - ymin ;
        real dz = zmax - zmin ;

        // Find shortest bbox axis        
        vec3 V1,V2 ;
        if(dx < dy && dx < dz) {
            if(dy > dz) {
                V1 = vec3(0,1,0) ;
                V2 = vec3(0,0,1) ;
            } else {
                V2 = vec3(0,1,0) ;
                V1 = vec3(0,0,1) ;
            }
        } else if(dy < dx && dy < dz) {
            if(dx > dz) {
                V1 = vec3(1,0,0) ;
                V2 = vec3(0,0,1) ;
            } else {
                V2 = vec3(1,0,0) ;
                V1 = vec3(0,0,1) ;
            }
        } else if(dz < dx && dz < dy) {
            if(dx > dy) {
                V1 = vec3(1,0,0) ;
                V2 = vec3(0,1,0) ;
            } else {
                V2 = vec3(1,0,0) ;
                V1 = vec3(0,1,0) ;
            }
        }
        
        // Project onto shortest bbox axis,
        // and lock extrema vertices
        
        const Map::Vertex* vxmin = nil ;
        real  umin = 1e30f ;
        const Map::Vertex* vxmax = nil ;
        real  umax = -1e30f ;

        FOR_EACH_VERTEX(Map, map_, it) {
            const Map::Vertex* V = it ;
            real u = dot(V->point(), V1) ;
            real v = dot(V->point(), V2) ;
            tex_coord_[V] = vec2(u,v) ;
            if(u < umin) {
                vxmin = V ;
                umin = u ;
            } 
            if(u > umax) {
                vxmax = V ;
                umax = u ;
            } 
        }
        is_locked_[vxmin] = true ;
        is_locked_[vxmax] = true ;
    }

    void LSCM::map_to_solver() {
        solver_->allocate(2*map_->nb_vertices()) ;
        index_t cur_id = 0 ;
        FOR_EACH_VERTEX(Map, map_, it) {
            vertex_id_[it] = cur_id ; 
            solver_->variable(2*cur_id).set_value(tex_coord_[it].x) ;
            solver_->variable(2*cur_id+1).set_value(tex_coord_[it].y) ;
            if(is_locked_[it]) {
                solver_->variable(2*cur_id).lock() ;
                solver_->variable(2*cur_id+1).lock() ;
            }
            cur_id++ ;
        }
    }
    
    void LSCM::solver_to_map() {
        FOR_EACH_VERTEX(Map, map_, it) {
            index_t cur_id = vertex_id_[it] ;
            tex_coord_[it] = vec2(
                real(solver_->variable(2*cur_id).value()),
                real(solver_->variable(2*cur_id + 1).value())
            ) ;
        }
    }

    void LSCM::setup_conformal_map_relations() {
        solver_->begin_system() ;
        FOR_EACH_FACET(Map, map_, it) {
            setup_conformal_map_relations(it) ;
        }
        solver_->end_system() ;
    }
    
    void LSCM::setup_conformal_map_relations(const Map::Facet* f) {
        // "virtually triangulate" the facet
        Map::Halfedge* cur = f-> halfedge() ;
        Map::Halfedge* h0 = cur ;
        cur = cur-> next() ;
        Map::Halfedge* h1 = cur ;
        cur = cur-> next() ;
        Map::Halfedge* h2 = cur ;
        do {
            setup_conformal_map_relations(h0->vertex(), h1->vertex(), h2->vertex()) ;
            h1 = cur ;
            cur = cur-> next() ;
            h2 = cur ;
        } while (h2 != h0) ;
    }

    // Computes the coordinates of the vertices of a triangle
    // in a local 2D orthonormal basis of the triangle's plane.
    static void project_triangle(
        const vec3& p0, 
        const vec3& p1, 
        const vec3& p2,
        vec2& z0,
        vec2& z1,
        vec2& z2
    ) {
        vec3 X = normalize(p1 - p0) ;
        vec3 Z = normalize(cross(X,p2 - p0)) ;
        vec3 Y = cross(Z,X) ;
        const vec3& O = p0 ;
        
        real x0 = 0 ;
        real y0 = 0 ;
        real x1 = length(p1 - O) ;
        real y1 = 0 ;
        real x2 = dot(p2 - O, X) ;
        real y2 = dot(p2 - O, Y) ;        
            
        z0 = vec2(x0,y0) ;
        z1 = vec2(x1,y1) ;
        z2 = vec2(x2,y2) ;        
    }

    // LSCM equation, geometric form :
    // (Z1 - Z0)(U2 - U0) = (Z2 - Z0)(U1 - U0)
    // Where Uk = uk + i.vk is the complex number 
    //                       corresponding to (u,v) coords
    //       Zk = xk + i.yk is the complex number 
    //                       corresponding to local (x,y) coords
    // cool: no divide with this expression,
    //  makes it more numerically stable in
    //  the presence of degenerate triangles.
    
    void LSCM::setup_conformal_map_relations(
        const Map::Vertex* v0, const Map::Vertex* v1, const Map::Vertex* v2
    ) {
        
        index_t id0 = vertex_id_[v0] ;
        index_t id1 = vertex_id_[v1] ;
        index_t id2 = vertex_id_[v2] ;
            
        const vec3& p0 = v0->point() ; 
        const vec3& p1 = v1->point() ;
        const vec3& p2 = v2->point() ;
            
        vec2 z0,z1,z2 ;
        project_triangle(p0,p1,p2,z0,z1,z2) ;
        vec2 z01 = z1 - z0 ;
        vec2 z02 = z2 - z0 ;
        real a = z01.x ;
        real b = z01.y ;
        real c = z02.x ;
        real d = z02.y ;
        gx_assert(b == 0.0) ;

        // Note  : 2*id + 0 --> u
        //         2*id + 1 --> v
        index_t u0_id = 2*id0     ;
        index_t v0_id = 2*id0 + 1 ;
        index_t u1_id = 2*id1     ;
        index_t v1_id = 2*id1 + 1 ;
        index_t u2_id = 2*id2     ;
        index_t v2_id = 2*id2 + 1 ;
        
        // Note : b = 0

        // Real part
        solver_->begin_row() ;
        solver_->add_coefficient(u0_id, -a+c)  ;
        solver_->add_coefficient(v0_id,  b-d)  ;
        solver_->add_coefficient(u1_id,   -c)  ;
        solver_->add_coefficient(v1_id,    d)  ;
        solver_->add_coefficient(u2_id,    a) ;
        solver_->end_row() ;

        // Imaginary part
        solver_->begin_row() ;
        solver_->add_coefficient(u0_id, -b+d) ;
        solver_->add_coefficient(v0_id, -a+c) ;
        solver_->add_coefficient(u1_id,   -d) ;
        solver_->add_coefficient(v1_id,   -c) ;
        solver_->add_coefficient(v2_id,    a) ;
        solver_->end_row() ;
    }



}
