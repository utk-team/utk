/*
 *  OGF/Graphite: Geometry and Graphics Programming Library + Utilities
 *  Copyright (C) 2000-2009 INRIA - Project ALICE
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
 *  Contact: Bruno Levy - levy@loria.fr
 *
 *     Project ALICE
 *     LORIA, INRIA Lorraine, 
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX 
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 *
 * As an exception to the GPL, Graphite can be linked with the following (non-GPL) libraries:
 *     Qt, SuperLU, WildMagic and CGAL
 */
 

#include "connectivity_optimizer.h"

namespace Geex {
    ConnectivityOptimizer::ConnectivityOptimizer(Map* map) 
	:editor_(map) { 
	}
        
    ConnectivityOptimizer::~ConnectivityOptimizer() { 
    }        

	static double regular(Map::Vertex* v, int degree) {
		int diff ;
		if(v->is_on_border()) {
			diff =  degree-4 ;
		} 
		else 
			diff = degree-6 ;

		return diff*diff ;
	}

	static double flip_cost(Map::Halfedge* h) {
		Map::Vertex *v0 = h->vertex() ;
		Map::Vertex *v1 = h->opposite()->vertex() ;
		Map::Vertex *v2 = h->next()->vertex() ;
		Map::Vertex *v3 = h->opposite()->next()->vertex() ;

		double reg_bef   = regular(v0, v0->degree())+regular(v1, v1->degree())
			+ regular(v2, v2->degree())+regular(v3, v3->degree()) ;
		double reg_after = regular(v0, v0->degree()-1)+regular(v1, v1->degree()-1)
			+ regular(v2, v2->degree()+1)+regular(v3, v3->degree()+1) ;
		
		return reg_after-reg_bef ;
	}

	void ConnectivityOptimizer::apply() {
		MapVertexProperty<double> cost ;
		MapVertexProperty<Map::Halfedge*> to_swap ;

		cost.bind(target()) ;
		to_swap.bind(target()) ;

		FOR_EACH_VERTEX(Map, target(), vi) {
			cost[vi] = 1e19 ;
		}

		FOR_EACH_HALFEDGE(Map, target(), hi) {
			if(!editor_.is_flippable(hi)) 
				continue ;

			double cost_h = flip_cost(hi) ;
			if(cost_h < cost[hi->vertex()]) {
				cost[hi->vertex()] = cost_h ;
				to_swap[hi->vertex()] = hi ;
			}
		}

		bool done = false ;
		MapVertexHeap Q(target(), cost) ;	
		Q.init_with_all_surface_vertices() ;

		while(!done && !Q.empty()) {
			Map::Vertex *v = Q.pop() ;
			Map::Halfedge *h = to_swap[v] ;
			double costv = cost[v] ;

			if(cost[v]<0 && editor_.is_flippable(h)) {
				Map::Vertex* to_update[4] ;
				to_update[0] = h->vertex() ;
				to_update[1] = h->opposite()->vertex() ;
				to_update[2] = h->next()->vertex() ;
				to_update[3] = h->opposite()->next()->vertex() ;

				editor_.edge_flip(h) ;

				// update neighboring vertices
				for(int i=0; i<4; ++i) {
					Map::Halfedge *cir = to_update[i]->halfedge() ;
					cost[to_update[i]] = flip_cost(cir) ;
					to_swap[to_update[i]] = cir ;
					cir = cir->next_around_vertex() ;

					while(cir!=to_update[i]->halfedge()) {
						double cost_cir = flip_cost(cir) ; 
						if(cost_cir < cost[to_update[i]]) {
							cost[to_update[i]] = cost_cir ;
							to_swap[to_update[i]] = cir ;
						}
						cir = cir->next_around_vertex() ;
					} ;

					if(cost[to_update[i]] < 0 && !Q.contains(to_update[i]) ) {
						Q.push(to_update[i]) ;
					}

					if(Q.contains(to_update[i])) {
						Q.update_cost(to_update[i]) ;
					}
				}
			}
		}

		cost.unbind() ;
		to_swap.unbind() ;
	}
}

