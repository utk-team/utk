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
 
#include <Geex/combinatorics/triangles_map.h>
//#include <Geex/combinatorics/colocate.h>
#include <Geex/combinatorics/map_builder.h>
#include <stack>

namespace Geex {

    TrianglesMap::TrianglesMap(
	std::vector<vec3>& vertices_in,
	std::vector<trindex>& triangles_in
    ) : vertices_(&vertices_in), nb_vertices_(vertices_in.size()), triangles_(triangles_in), 
	vertex_index_(vertices_in.size()) {
	init_edge_info() ;
	fast_mode_ = false ;
    }

    TrianglesMap::TrianglesMap(
	unsigned int nb_vertices_in,
	std::vector<trindex>& triangles_in
    ) : vertices_(nil), nb_vertices_(nb_vertices_in), triangles_(triangles_in), 
	vertex_index_(nb_vertices_in) {
	init_edge_info() ;
	fast_mode_ = false ;	   
    }

    void TrianglesMap::remove_duplicates(std::vector<trindex>& triangles) {
	std::cerr << "Nb triangles before remove duplicates=" << triangles.size() << std::endl ;
	std::sort(triangles.begin(), triangles.end()) ;
	std::vector<trindex>::iterator new_end = std::unique(triangles.begin(), triangles.end()) ;
	triangles.erase(new_end, triangles.end()) ;
	std::cerr << "Nb triangles after remove duplicates=" << triangles.size() << std::endl ;

	unsigned int nb_degenerate_triangles = 0 ;
	for(unsigned int t=0; t<triangles.size(); t++) {
	    if(triangle_is_degenerate(triangles[t])) {
		nb_degenerate_triangles++ ;
	    }
	}
	if(nb_degenerate_triangles != 0) {
	    std::cerr << "Has " 
		      << nb_degenerate_triangles  
		      << " degenerate_trianges" << std::endl ;

	    std::vector<trindex> fixed_triangles ;
	    fixed_triangles.reserve(triangles.size()) ;
	    for(unsigned int t=0; t<triangles.size(); t++) {
		if(!triangle_is_degenerate(triangles[t])) {
		    fixed_triangles.push_back(triangles[t]) ;
		}
	    }
	    triangles = fixed_triangles ;
	}
    }


    inline bool is_visited(const trindex& T) {
	return T.indices[0] != -1 || T.indices[1] != -1 || T.indices[2] != -1 ;
    }

    inline void mark_as_visited(trindex& T) { T.indices[0] = -2 ; }

    void TrianglesMap::build(
	std::vector<trindex>& triangles_out, std::vector<unsigned int>& original_vertex_index
    ) {
       
        if(fast_mode_) {
	   return build_fast(triangles_out, original_vertex_index) ;
	}
       
	triangles_out.assign(triangles_.size(), trindex(-1,-1,-1,KeepOrder)) ;
	// 0.6 is probably a reasonable guess. This slightly improves performance.
	original_vertex_index.reserve(int(0.6*triangles_.size())) ;
	triangles_adjacent_.assign(triangles_.size(), trindex(-1,-1,-1,KeepOrder)) ;

	// Step 1: reorient
	for(unsigned int t=0; t<triangles_.size(); t++) {
	    if(!is_visited(triangles_out[t])) {
		std::stack<int> S ;
		S.push(t) ; 
		mark_as_visited(triangles_out[t]) ;

		while(!S.empty()) {
		    int t = S.top() ; S.pop() ;
		    
		    // Push neighbors
		    for(unsigned int ie=0; ie<3; ie++) {

			// Lookup edge info
			bindex EK = edge_key(t,ie) ;
			const EdgeInfo& EI = edges_.find(EK)->second ;

			if(EI.nb_incident() == 2) {
			    unsigned int t2 = EI.incident(0) ;
			    if(t2 == t) { t2 = EI.incident(1) ; }
			    if(!is_visited(triangles_out[t2])) {

				//    Check for orientation, and reorient T2 if 
				// the common edge is not in opposite orientation
				// with respect to t1.
				bindex E = edge(t,ie) ;
				trindex& T2 = triangles_[t2] ;
				if(triangle_has_edge(T2,E)) {
				    T2 = trindex::inverse(T2) ;
				} else {
				    gx_debug_assert(triangle_has_edge(T2, bindex::inverse(E))) ;
				}

				mark_as_visited(triangles_out[t2]) ;
				S.push(t2) ;
			    }
			}
		    }
		}
	    }
	}
	
	// Step 2: connect
	for(EdgeMap::iterator it=edges_.begin(); it != edges_.end(); it++) {
	    const EdgeInfo& E = it->second ;
	    if(E.nb_incident() == 2) {
		connect(E.incident(0), E.incident(1)) ;
		connect(E.incident(1), E.incident(0)) ;
	    }
	}

	// Step 3: generate vertices
	for(unsigned int t=0; t<triangles_.size(); t++) {
	    for(unsigned int iv=0; iv<3; iv++) {
		if(triangles_out[t].indices[iv] < 0) {
		    int v = triangles_[t].indices[iv] ;
		    int new_v = original_vertex_index.size() ;
		    original_vertex_index.push_back(v) ;
		    int cur_t = t ;
		    unsigned int cur_c = iv ;
		    do {
			triangles_out[cur_t].indices[cur_c] = new_v ;
			move_to_next_around_vertex(cur_t, cur_c, v) ;
		    } while(cur_t != t && cur_t != -1) ;
		    if(cur_t == -1) {
			cur_t = t ; cur_c = iv ;
			do {
			    triangles_out[cur_t].indices[cur_c] = new_v ;
			    move_to_prev_around_vertex(cur_t, cur_c, v) ;
			} while(cur_t != -1) ;
		    }
		}
	    }
	}
    }



    void TrianglesMap::build_fast(
	std::vector<trindex>& triangles_out, std::vector<unsigned int>& original_vertex_index
    ) {
	unsigned int cur_v_index = 0 ;
	triangles_out.assign(triangles_.size(), trindex(-1,-1,-1,KeepOrder)) ;
	// 0.6 is probably a reasonable guess. This slightly improves performance.
	original_vertex_index.reserve(int(0.6*triangles_.size())) ;

	for(unsigned int t=0; t<triangles_.size(); t++) {
	    if(!is_visited(triangles_out[t])) {
		std::fill(vertex_index_.begin(), vertex_index_.end(), -1) ;

		std::stack<int> S ;
		S.push(t) ;

		// Create vertices (if not created already) and copy
		// vertex index to output.
		for(unsigned int iv=0; iv<3; iv++) {
		    unsigned int v = triangles_[t].indices[iv] ;
		    if(vertex_index_[v] == -1) {
			original_vertex_index.push_back(v) ;
			vertex_index_[v] = cur_v_index ;
			cur_v_index++ ;
		    }
		    triangles_out[t].indices[iv] = vertex_index_[v] ;
		}
		    

		while(!S.empty()) {
		    int t = S.top() ; S.pop() ;
		    
		    // Push neighbors
		    for(unsigned int ie=0; ie<3; ie++) {

			// Lookup edge info
			bindex EK = edge_key(t,ie) ;
			const EdgeInfo& EI = edges_.find(EK)->second ;

			if(EI.nb_incident() == 2) {
			    unsigned int t2 = EI.incident(0) ;
			    if(t2 == t) { t2 = EI.incident(1) ; }
			    if(!is_visited(triangles_out[t2])) {

				//    Check for orientation, and reorient T2 if 
				// the common edge is not in opposite orientation
				// with respect to t1.
				bindex E = edge(t,ie) ;
				trindex& T2 = triangles_[t2] ;
				if(triangle_has_edge(T2,E)) {
				    T2 = trindex::inverse(T2) ;
				} else {
				    gx_debug_assert(triangle_has_edge(T2, bindex::inverse(E))) ;
				}


				// Create vertices (if not created already) and copy
				// vertex index to output.
				for(unsigned int iv=0; iv<3; iv++) {
				    unsigned int v = triangles_[t2].indices[iv] ;
				    if(vertex_index_[v] == -1) {
					original_vertex_index.push_back(v) ;
					vertex_index_[v] = cur_v_index ;
					cur_v_index++ ;
				    }
				    triangles_out[t2].indices[iv] = vertex_index_[v] ;
				}

				S.push(t2) ;
			    }
			}
		    }
		}
	    }
	}
    }

    void TrianglesMap::build(
	MapBuilder& builder, bool keep_vertex_index
    ) {
	gx_assert(vertices_ != nil) ; // Did we use the right constructor ? sanity check.

	MapBuilder* concrete_map_builder = dynamic_cast<MapBuilder*>(&builder) ;

	//MapVertexAttribute<int> vertex_index_attr ;
	//if(keep_vertex_index) {
	//    gx_assert(concrete_map_builder != nil) ;
	//    vertex_index_attr.bind(concrete_map_builder->target(), "id") ;
	//}

	std::vector<trindex> triangles_out ;
	std::vector<unsigned int> original_vertex_index ;
	build(triangles_out, original_vertex_index) ;
	builder.begin_surface() ;
	builder.create_vertices(original_vertex_index.size()) ;
	for(unsigned int v=0; v<original_vertex_index.size(); v++) {
	    builder.set_vertex(v, (*vertices_)[original_vertex_index[v]]) ;
	 //   if(keep_vertex_index) {
		//vertex_index_attr[concrete_map_builder->vertex(v)] = original_vertex_index[v] ;
	 //   }
	}
	builder.reserve_facets(triangles_out.size()) ;
	for(unsigned int t=0; t<triangles_out.size(); t++) {
	    builder.begin_facet() ;
	    builder.add_vertex_to_facet(triangles_out[t].indices[0]) ;
	    builder.add_vertex_to_facet(triangles_out[t].indices[1]) ;
	    builder.add_vertex_to_facet(triangles_out[t].indices[2]) ;
	    builder.end_facet() ;
	}
	builder.end_surface() ;
    }

    // Vertices of the edges of a triangle (local indices)
    unsigned int TrianglesMap::trgl_e_v[3][2] = {
	{1,2},
	{2,0},
	{0,1}
    } ;

    //============================================================================================

    ManifoldComponentsMapBuilder::ManifoldComponentsMapBuilder(
	MapBuilder& builder
    ) : builder_(builder), colocate_(false), colocate_tolerance_(1e-6)  {
    }

    void ManifoldComponentsMapBuilder::begin_surface() {
    }
    
    void ManifoldComponentsMapBuilder::end_surface() {
	if(colocate_) {
	 //   std::vector<vec3> new_vertices ;
	 //   std::vector<int> old2new ;
	 //   Geom::colocate(vertices_, old2new, new_vertices, colocate_tolerance_) ;
	 //   std::cerr << "Nb vertices before colocate:" << vertices_.size() << std::endl ;
	 //   std::cerr << "Nb vertices after colocate:"  << new_vertices.size() << std::endl ;
	 //   for(unsigned int t=0; t<triangles_.size(); t++) {
		//int i = triangles_[t].indices[0] ;
		//int j = triangles_[t].indices[1] ;
		//int k = triangles_[t].indices[2] ;
		//triangles_[t] = trindex(old2new[i], old2new[j], old2new[k]) ;
	 //   }
	 //   TrianglesMap::remove_duplicates(triangles_) ;
	 //   TrianglesMap tmap(new_vertices, triangles_) ;
	 //   tmap.build(builder_) ;
	} else {
	    TrianglesMap::remove_duplicates(triangles_) ;
	    TrianglesMap tmap(vertices_, triangles_) ;
	    tmap.build(builder_) ;
	}
	vertices_.clear() ;
	triangles_.clear() ;
    }
    
    void ManifoldComponentsMapBuilder::begin_facet() {
	v1_ = -1 ; v2_ = -1 ;
    }
    
    void ManifoldComponentsMapBuilder::end_facet() {
    }
    
    void ManifoldComponentsMapBuilder::add_vertex_to_facet(int v) {
	if(v1_ == -1) { v1_ = v ; return ; }
	if(v2_ == -1) { v2_ = v ; return ; }
	triangles_.push_back(trindex(v1_, v2_, v)) ;
	v2_ = v ;
    }
    
    void ManifoldComponentsMapBuilder::create_vertices(unsigned int nb_v, bool with_colors) {
	vertices_.resize(nb_v) ;
    }
    
    void ManifoldComponentsMapBuilder::add_vertex(const vec3& p) {
	vertices_.push_back(p) ;
    }
    
    void ManifoldComponentsMapBuilder::reserve_facets(unsigned int nb_f) {
	triangles_.reserve(nb_f) ;
    }
    
    void ManifoldComponentsMapBuilder::set_vertex(unsigned int v, const vec3& p) {
	gx_debug_assert(v < vertices_.size()) ;
	vertices_[v] = p ;
    }
    
 //   void ManifoldComponentsMapBuilder::set_vertex_color(unsigned int v, const Color& c) {
	// Ignored.
 //   }
    
    void ManifoldComponentsMapBuilder::reset() {
    }
    
    void ManifoldComponentsMapBuilder::add_tex_vertex(const vec2& v) {
	// Ignored.
    }
    
    void ManifoldComponentsMapBuilder::set_corner_tex_vertex(int v) {
	// Ignored.
    }
    
    void ManifoldComponentsMapBuilder::lock_vertex(int v) {
	// Ignored.
    }
    
    void ManifoldComponentsMapBuilder::unlock_vertex(int v) {
	// Ignored.
    }
    
    void ManifoldComponentsMapBuilder::add_vertex(unsigned int id, const vec3& p) {
	if(vertices_.size() <= id) {
	    vertices_.resize(id+1) ;
	}
	vertices_[id] = p ;
    }
    
    void ManifoldComponentsMapBuilder::add_tex_vertex(unsigned int id, const vec2& p) {
	// Ignored.
    }
    
}

