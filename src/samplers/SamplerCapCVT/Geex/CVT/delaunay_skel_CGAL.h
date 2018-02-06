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

#ifndef __GEEX_CVT_DELAUNAY_SKEL_CGAL__
#define __GEEX_CVT_DELAUNAY_SKEL_CGAL__

#include <Geex/CVT/delaunay_skel.h>

namespace Geex {

    // Used by copy_Delaunay_to_skel, computes the bisector
    // of a primal edge.
    // Note: if we transform that into a CGAL module, we will
    // use CGAL's standard bisector() function and CGAL's Plane3.

    template <class POINT> inline Plane<real> gx_bisector(
        const POINT& p1, const POINT& p2
    ) {
        vec3 gx_p1(p1.cartesian(0), p1.cartesian(1), p1.cartesian(2)) ;
        vec3 gx_p2(p2.cartesian(0), p2.cartesian(1), p2.cartesian(2)) ;
        vec3 N = gx_p1 - gx_p2 ; 
        // N = normalize(N) ; Not sure we need to do that anymore ...
        return Geex::Plane<real>(0.5*(gx_p1 + gx_p2), N) ;
    }

    // Copies a CGAL triangulation into a Delaunay skeleton.
    // Requirement: del's vertices have an "id" member.
    template <class DEL> inline void copy_CGAL_Delaunay_to_skel(
        const DEL* del, DelaunaySkeleton* skel
    ) {
        typedef typename DEL::Edge Edge ;
        typedef typename DEL::Vertex_handle Vertex_handle ;
        typedef typename DEL::Vertex_iterator Vertex_iterator ;
        typedef typename DEL::Finite_vertices_iterator Finite_vertices_iterator ;
        typedef typename DEL::Cell_handle Cell_handle ;
        typedef typename DEL::Cell_circulator Cell_circulator ;

        skel->clear() ;
        for(
            Finite_vertices_iterator it = del->finite_vertices_begin() ; 
            it != del->finite_vertices_end(); it++
        ) {
            // Make sure that vertices numbering is correct
            gx_debug_assert(skel->nb_vertices() == it->id) ;

            skel->begin_star() ;
            
            // The algorithm operates by traversing the edges radiating from vertex 'it',
            // and turning around them using a Cell_circulator. Each time a cell is traversed,
            // it's non-already-visited edges incident to 'it' are inserted in the stack.

            std::stack<Edge> S ;

            // The algorithm is initialized by inserting the three edges radiating from
            // 'it' along three edges of 'it->cell()'
            Cell_handle c = it->cell() ;
            unsigned int i = c->index(it) ;
            for(unsigned int j=0; j<4; j++) {
                if(j != i) {
                    Vertex_handle neigh = c->vertex(j) ;
                    if(!del->is_infinite(neigh)) {
                        S.push(Edge(c,i,j)) ;
                        skel->add_to_star(
                            neigh->id,
                            gx_bisector(it->point(), neigh->point())
                        ) ;
                    }
                }
            }
            while(!S.empty()) {
                Edge e = S.top() ; S.pop() ;
                Cell_handle cell_from = e.first ;
                Cell_circulator cell_around_edge = del->incident_cells(e) ;
                Cell_circulator last_cell_around_edge = cell_around_edge ;
                do {
                    // An Edge = a triplet (cell, local vertex index, local vertex index),
                    // therefore we know from which cell (cell_from) it was 'seen' when it 
                    // was inserted in the stack. We know that 'cell_from' was already visited, 
                    // this saves some tests.
                    if(cell_around_edge != cell_from) {
                        unsigned int i = cell_around_edge->index(it) ;
                        for(unsigned int j=0; j<4; j++) {
                            Vertex_handle neigh = cell_around_edge->vertex(j) ;
                            // We use the DelaunaySkeleton under construction to
                            // test whether the current vertex (neigh) was already
                            // visited. This is done using linear search (tested
                            // with a std::set, it is faster with linear search since
                            // we got a small number of neighbors in each star).
                            if(
                                (j != i) && !del->is_infinite(neigh) && 
                                !skel->last_star_has_vertex(neigh->id) 
                            ) {
                                S.push(Edge(cell_around_edge,i,j)) ;
                                skel->add_to_star(
                                    neigh->id,
                                    gx_bisector(it->point(), neigh->point())
                                ) ;
                            }
                        }
                    }
                    cell_around_edge++ ;
                } while(cell_around_edge != last_cell_around_edge) ;
            }
            skel->end_star() ;
        }
    }

    // Same algorithm, but uses a vector of handles

    // Copies a CGAL triangulation into a Delaunay skeleton.
    // Requirement: del's vertices have an "id" member.
    template <class DEL> inline void copy_CGAL_Delaunay_to_skel(
        const DEL* del, DelaunaySkeleton* skel, std::vector<typename DEL::Vertex_handle> vertices
    ) {
        typedef typename DEL::Edge Edge ;
        typedef typename DEL::Vertex_handle Vertex_handle ;
        typedef typename DEL::Vertex_iterator Vertex_iterator ;
        typedef typename DEL::Finite_vertices_iterator Finite_vertices_iterator ;
        typedef typename DEL::Cell_handle Cell_handle ;
        typedef typename DEL::Cell_circulator Cell_circulator ;

        skel->clear() ;
        for(unsigned int iv=0; iv<vertices.size(); iv++) {
            Vertex_handle it = vertices[iv] ;

            // Make sure that vertices numbering is correct
            gx_debug_assert(skel->nb_vertices() == it->id) ;

            skel->begin_star() ;
            
            // The algorithm operates by traversing the edges radiating from vertex 'it',
            // and turning around them using a Cell_circulator. Each time a cell is traversed,
            // it's non-already-visited edges incident to 'it' are inserted in the stack.

            std::stack<Edge> S ;

            // The algorithm is initialized by inserting the three edges radiating from
            // 'it' along three edges of 'it->cell()'
            Cell_handle c = it->cell() ;
            unsigned int i = c->index(it) ;
            for(unsigned int j=0; j<4; j++) {
                if(j != i) {
                    Vertex_handle neigh = c->vertex(j) ;
                    if(!del->is_infinite(neigh)) {
                        S.push(Edge(c,i,j)) ;
                        skel->add_to_star(
                            neigh->id,
                            gx_bisector(it->point(), neigh->point())
                        ) ;
                    }
                }
            }
            while(!S.empty()) {
                Edge e = S.top() ; S.pop() ;
                Cell_handle cell_from = e.first ;
                Cell_circulator cell_around_edge = del->incident_cells(e) ;
                Cell_circulator last_cell_around_edge = cell_around_edge ;
                do {
                    // An Edge = a triplet (cell, local vertex index, local vertex index),
                    // therefore we know from which cell (cell_from) it was 'seen' when it 
                    // was inserted in the stack. We know that 'cell_from' was already visited, 
                    // this saves some tests.
                    if(cell_around_edge != cell_from) {
                        unsigned int i = cell_around_edge->index(it) ;
                        for(unsigned int j=0; j<4; j++) {
                            Vertex_handle neigh = cell_around_edge->vertex(j) ;
                            // We use the DelaunaySkeleton under construction to
                            // test whether the current vertex (neigh) was already
                            // visited. This is done using linear search (tested
                            // with a std::set, it is faster with linear search since
                            // we got a small number of neighbors in each star).
                            if(
                                (j != i) && !del->is_infinite(neigh) && 
                                !skel->last_star_has_vertex(neigh->id) 
                            ) {
                                S.push(Edge(cell_around_edge,i,j)) ;
                                skel->add_to_star(
                                    neigh->id,
                                    gx_bisector(it->point(), neigh->point())
                                ) ;
                            }
                        }
                    }
                    cell_around_edge++ ;
                } while(cell_around_edge != last_cell_around_edge) ;
            }
            skel->end_star() ;
        }
    }
    
}

#endif
