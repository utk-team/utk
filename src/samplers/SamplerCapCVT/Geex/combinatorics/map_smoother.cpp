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

#include <Geex/combinatorics/map_smoother.h>
#include <Geex/combinatorics/map_geometry.h>

namespace Geex {

    MapSmoother::MapSmoother() {
        solver_ = new Solver ;
        map_ = nil ;
        weight_ = 1.0 ;
    }
    
    void MapSmoother::smooth(Map* map) {
        map_ = map ;
        has_locked_vertices_ = false ;
        is_locked_.bind(map_) ;
        vertex_id_.bind(map_) ;
        index_t cur_id = 0 ;
        FOR_EACH_VERTEX(Map, map_, it) {
            vertex_id_[it] = cur_id ;
            cur_id++ ;
        }
        geometry_.bind_if_defined(map_, "geometry") ;
        if(!geometry_.is_bound()) {
            geometry_.bind(map_, "geometry") ;
            FOR_EACH_VERTEX(Map, map_, it) {
                geometry_[it] = it->point() ;
            }
        }

        solver_->allocate(map_->nb_vertices()) ;
        for(index_t coord=0; coord<3; coord++) {
            map_to_solver(coord) ;
            build_matrix(coord) ;
            solver_->solve() ;
            solver_to_map(coord) ;
            solver_->reset(has_locked_vertices_) ;
        }

        geometry_.unbind() ;
        vertex_id_.unbind() ;
        is_locked_.unbind() ;
        map_ = nil ;
    }

    void MapSmoother::build_matrix(index_t coord) {
        solver_->begin_system() ;
        
        FOR_EACH_VERTEX(Map, map_, it) {
            solver_->begin_row() ;
            Map::Halfedge* h = it->halfedge() ;
            double aii = 0.0 ;
            do {
                double aij = 1.0 ;
                aii += aij ;
                solver_->add_coefficient(vertex_id_[h->opposite()->vertex()], aij) ;
                h = h->next_around_vertex() ;
            } while(h != it->halfedge()) ;
            solver_->add_coefficient(vertex_id_[it], -aii) ;
            solver_->normalize_row() ;
            solver_->end_row() ;
        }

        FOR_EACH_VERTEX(Map, map_, it) {
            solver_->begin_row() ;
            solver_->add_coefficient(vertex_id_[it], 1.0) ;
            solver_->set_right_hand_side(geometry_[it][coord]) ;
            solver_->normalize_row(weight_) ;
            solver_->end_row() ;
        }
        solver_->end_system() ;
    }

    void MapSmoother::map_to_solver(index_t coord) {
        has_locked_vertices_ = false ;
        FOR_EACH_VERTEX(Map, map_, it) {
            solver_->variable(vertex_id_[it]).set_value(it->point()[coord]) ;
            if(is_locked_[it]) {
                solver_->variable(vertex_id_[it]).lock() ;
                has_locked_vertices_ = true ;
            }
        }
    }

    void MapSmoother::solver_to_map(index_t coord) {
        FOR_EACH_VERTEX(Map, map_, it) {
            it->point()[coord] = solver_->variable(vertex_id_[it]).value() ;
        }
    }

}
