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
 

#ifndef __GEEX_TRIANGLES_MAP__
#define __GEEX_TRIANGLES_MAP__

//#include <OGF/GoodMesh/common/common.h>
#include <Geex/combinatorics/index.h>
#include <Geex/combinatorics/map_builder.h>
#include <Geex/mathematics/glsl_linear.h>
#include <vector>
#include <map>

namespace Geex {

    /**
     * Reconstructs a map given a set of (possibly unoriented) triangles.
     * Non-manifold edges are segmented.
     */
    class GEEX_API TrianglesMap {
    public:
	TrianglesMap(
	    std::vector<vec3>& vertices_in,
	    std::vector<trindex>& triangles_in
	) ;

	TrianglesMap(
	    unsigned int nb_vertices_in,
	    std::vector<trindex>& triangles_in
	) ;

	~TrianglesMap() {}

	static void remove_duplicates(std::vector<trindex>& triangles) ;
	static bool triangle_is_degenerate(const trindex& T) {
	    return (
		T.indices[0] == T.indices[1] || 
		T.indices[0] == T.indices[2] || 
		T.indices[1] == T.indices[2]
	    ) ;
	}

	void build(
	    MapBuilder& builder, bool keep_vertex_index = false
	) ;


	void build(std::vector<trindex>& out, std::vector<unsigned int>& vertex_index) ;

        void build_fast(
            std::vector<trindex>& out, std::vector<unsigned int>& vertex_index
	) ;

        void set_fast_mode(bool x) { fast_mode_ = x ; }
       
    protected:

	static int index(const trindex& T, int v) {
	    for(unsigned int iv=0; iv<3; iv++) {
		if(T.indices[iv] == v) { return iv ; }
	    }
	    return -1 ;
	}

	void connect(unsigned int t1, unsigned int t2) {
	    gx_debug_assert(t1 < triangles_.size()) ;
	    gx_debug_assert(t2 < triangles_.size()) ;
	    const trindex& T1 = triangles_[t1] ;
	    const trindex& T2 = triangles_[t2] ;
	    if(index(T2,T1.indices[0]) == -1) {
		triangles_adjacent_[t1].indices[0] = t2 ;
	    } else if(index(T2, T1.indices[1]) == -1) {
		triangles_adjacent_[t1].indices[1] = t2 ;
	    } else {
		gx_debug_assert(index(T2, T1.indices[2]) == -1) ;
		triangles_adjacent_[t1].indices[2] = t2 ;
	    }
	}

	void move_to_next_around_vertex(int& t, unsigned int& c, int v) {
	    t = triangles_adjacent_[t].indices[trgl_e_v[c][0]] ;
	    if(t != -1) { c = index(triangles_[t], v) ; }
	}

	void move_to_prev_around_vertex(int& t, unsigned int& c, int v) {
	    t = triangles_adjacent_[t].indices[trgl_e_v[c][1]] ;
	    if(t != -1) { c = index(triangles_[t], v) ; }
	}

	struct EdgeInfo {
	    EdgeInfo() : non_manifold_(false) { 
		t[0] = -1 ; t[1] = -1 ;
	    }
	    void add_incident(int t_in) {
		if(non_manifold_) { return ; }
		if(t[0] == -1) { t[0] = t_in ; return ; }
		if(t[1] == -1) { t[1] = t_in ; return ; }
		non_manifold_ = true ; t[0] = -1 ; t[1] = -1 ; 
	    }
	    unsigned int nb_incident() const {
		if(t[1] != -1) { return 2 ; }
		if(t[0] != -1) { return 1 ; }
		return 0 ;
	    }
	    unsigned int incident(unsigned int idx) const {
		gx_debug_assert(idx < nb_incident()) ;
		return t[idx] ;
	    }
	    bool non_manifold() const { return non_manifold_ ; }
	private:
	    bool non_manifold_ ;
	    int t[2] ;
	} ;

	bindex edge(unsigned int t, unsigned int e) {
	    return bindex(
		triangles_[t].indices[trgl_e_v[e][0]], 
		triangles_[t].indices[trgl_e_v[e][1]],
		KeepOrder
	    ) ;
	}

	bindex edge_key(unsigned int t, unsigned int e) {
	    return bindex(
		triangles_[t].indices[trgl_e_v[e][0]], 
		triangles_[t].indices[trgl_e_v[e][1]]
	    ) ;
	}

	void init_edge_info() {
	    for(unsigned int t=0; t<triangles_.size(); t++) {
		for(unsigned int e=0; e<3; e++) {
		    edges_[edge_key(t,e)].add_incident(t) ;
		}
	    }
	}

	unsigned int nb_adjacent(unsigned int t) {
	    unsigned int result = 0 ;
	    for(unsigned int e=0; e<3; e++) {
		if(edges_.find(edge_key(t,e))->second.nb_incident() == 2) {
		    result++ ;
		}
	    }
	    return result ;
	}

    protected:
	std::vector<vec3>* vertices_ ;
	unsigned int nb_vertices_ ;
	std::vector<trindex>& triangles_ ;
	std::vector<trindex>  triangles_adjacent_ ;
	typedef std::map<bindex, EdgeInfo> EdgeMap ;
	EdgeMap edges_ ;
	std::vector<int> vertex_index_ ;
        bool fast_mode_ ;
	static unsigned int trgl_e_v[3][2] ;
    } ;

    inline bool triangle_has_edge(const trindex& T, const bindex& E) {
	unsigned int e0 = E.indices[0] ;
	unsigned int e1 = E.indices[1] ;
	return ( 
	    (T.indices[0] == e0 && T.indices[1] == e1) ||
	    (T.indices[1] == e0 && T.indices[2] == e1) ||
	    (T.indices[2] == e0 && T.indices[0] == e1)
	)  ;
    }

    /**
     * Uses a TrianglesMap to construct the manifold components of a map
     * into a target MapBuilder.
     */
    class GEEX_API ManifoldComponentsMapBuilder : public MapBuilder {
      public:
        ManifoldComponentsMapBuilder(MapBuilder& builder) ;
	void set_colocate(bool x, double tolerance = 1e-30) {
	    colocate_ = x ; colocate_tolerance_ = tolerance ;
	}
	virtual void begin_surface() ;
	virtual void end_surface() ;
	virtual void begin_facet() ;
	virtual void end_facet() ;
	virtual void add_vertex_to_facet(int v) ;
	virtual void create_vertices(unsigned int nb_v, bool with_colors) ;
	virtual void add_vertex(const vec3& p) ;
	virtual void reserve_facets(unsigned int nb_f) ;
	virtual void set_vertex(unsigned int v, const vec3& p) ;
//	virtual void set_vertex_color(unsigned int v, const Color& c) ;
	virtual void reset() ;
	virtual void add_tex_vertex(const vec2& v) ;
	virtual void set_corner_tex_vertex(int v) ;
	virtual void lock_vertex(int v) ;
	virtual void unlock_vertex(int v) ;
	virtual void add_vertex(unsigned int id, const vec3& p) ;
	virtual void add_tex_vertex(unsigned int id, const vec2& p) ;

      protected:
	bool colocate_ ;
	double colocate_tolerance_ ;
	MapBuilder& builder_ ;
	std::vector<vec3> vertices_ ;
	std::vector<trindex> triangles_ ;
	int v1_ ;
	int v2_ ;
    } ;


}

#endif
