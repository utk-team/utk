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

#include <Geex/combinatorics/map_editor.h>
#include <Geex/combinatorics/map_geometry.h>

// TODO: for all the functions creating cells, use 'copy-creation'
// functions when applicable (to copy the properties)

namespace Geex {

    MapEditor::MapEditor(Map* target) : MapMutator(target) {
        if(target != nil) {
            set_target(target) ;
        }
    }

    void MapEditor::set_target(Map* target) {
        MapMutator::set_target(target) ;
        if(is_locked_.is_bound()) {
            is_locked_.unbind() ;
        }
        is_locked_.bind(target) ;
    }

//__________ CGAL interface __________________________________

    MapEditor::Halfedge* MapEditor::make_polygon(int nb) {
        Halfedge* first = nil ;
        Halfedge* cur = nil ;
        for(int i=0; i<nb; i++) {
            if(first == nil) {
                first = new_edge() ;
                cur = first ;
                make_facet_key(cur, new_facet()) ;
            } else {
                link(cur, new_edge(), 1) ;
                link(cur-> next()-> opposite(), cur-> opposite(), 1) ;
                set_halfedge_facet(cur-> next(), cur-> facet()) ;
                set_halfedge_vertex(
                    cur-> next()-> opposite(), cur-> vertex()
                ) ;
                cur = cur-> next() ;
            }
            make_vertex_key(cur, new_vertex()) ;
        }
        link(cur, first, 1) ;
        link(first-> opposite(), cur-> opposite(), 1) ;
        set_halfedge_vertex(first-> opposite(), cur-> vertex()) ;
        return first ;
    }
    
    MapEditor::Halfedge* MapEditor::make_triangle() {
        return make_polygon(3) ;
    }
    
    MapEditor::Halfedge* MapEditor::make_triangle(
        const vec3& p1, const vec3& p2, const vec3& p3
    ) {
        Halfedge* result = make_triangle() ;
        result-> vertex()-> set_point(p1) ;
        result-> next()-> vertex()-> set_point(p2) ;
        result-> next()-> next()-> vertex()-> set_point(p3) ;
        return result ;
    }

    void MapEditor::make_hole(Halfedge* h) {

        gx_assert(!h-> is_border()) ;

        // Note: I have the feeling that this code should be
        //  much simpler ...

        std::vector<Halfedge*> edges_to_delete ;

        delete_facet(h-> facet()) ;
        Halfedge* end = h ;
        do {
            set_halfedge_facet(h, nil) ;
            Halfedge* g = h-> next() ;
            bool h_opp_on_border = h-> opposite()-> is_border() ;
            bool g_opp_on_border = g-> opposite()-> is_border() ;
            if(h_opp_on_border) {
                // remove vertex X if it looks like that : 
                //
                //   ............
                //   ======X=====
                //
                // but not like that :
                //
                //     .....|
                //     .....|
                //     -----X-----
                //          |..... 
                //          |.....

                if(g_opp_on_border) {
                    if(g-> opposite()-> next() == h-> opposite()) {
                        delete_vertex(h-> vertex()) ;
                    } else {
                        make_vertex_key(h-> opposite()-> prev()) ;
                        Halfedge* z1 = h-> opposite()-> prev() ;
                        Halfedge* z2 = g-> opposite()-> next() ;
                        link(z1, z2, 1) ;
                    }
                }
                edges_to_delete.push_back(h) ;
            } else {
                if(h-> next()-> opposite()-> is_border()) {
                    Halfedge* next = 
                        h-> next()-> opposite()-> next() ;
                    link(h, next, 1) ;
                    make_vertex_key(h) ;
                }
                if(h-> prev()-> opposite()-> is_border()) {
                    Halfedge* prev = 
                        h-> prev()-> opposite()-> prev() ;
                    link(prev, h, 1) ;
                    make_vertex_key(prev) ;
                }
            }
            h = g ;
        } while(h != end) ;
        
        for(
            std::vector<Halfedge*>::iterator 
                it = edges_to_delete.begin() ; 
            it != edges_to_delete.end() ; it++
        ) {
            delete_edge(*it) ;
        }
    }
  
    MapEditor::Halfedge* MapEditor::fill_hole(Halfedge* h) {
        gx_assert(h-> is_border()) ;
        Facet* facet = new_facet() ;
        set_facet_on_orbit(h, facet) ;
        make_facet_key(h) ;
        if(border_facet_.is_bound()) {
            border_facet_[facet] = true ;
        }
        return h ;
    }
    
    void MapEditor::fill_holes(int max_size) {
        std::vector<Halfedge*> to_fill ;
        Property<Halfedge, bool> is_marked(
            target()->halfedge_property_manager()
        ) ;
        {FOR_EACH_HALFEDGE(Map, target(), it) {
            if(it->is_border() && !is_marked[it]) {
                int size = 0 ;
                Halfedge* h = it ;
                do {
                    is_marked[h] = true ;
                    h = h->next() ;
                    size++ ;
                } while(h != it) ;
                if(max_size == -1 || size <= max_size) {
                    to_fill.push_back(it) ;
                }
            }
        }}
        for(unsigned int i=0; i<to_fill.size(); i++) {
            fill_hole(to_fill[i]) ;
        }
    }

    MapEditor::Halfedge* MapEditor::add_vertex_and_facet_to_border(
        Halfedge* h, Halfedge* g
    ) {
        gx_assert(h-> is_border()) ;
        gx_assert(g-> is_border()) ;
        gx_assert(h != g) ;

        gx_assert(halfedges_on_same_facet(h,g)) ;

        Halfedge* h2 = h-> next() ;
        Halfedge* g2 = g-> next() ;

        Halfedge* n1 = new_edge() ;
        Halfedge* n2 = new_edge() ;
        
        link(n1, n2, 1) ;
        link(n2-> opposite(), n1-> opposite(), 1) ;

        make_vertex_key(n1, new_vertex()) ;
        set_halfedge_vertex(n2-> opposite(), n1-> vertex()) ;

        link(n2, h2, 1) ;
        set_halfedge_vertex(n2, h-> vertex()) ;

        link(g, n1, 1) ;
        set_halfedge_vertex(n1-> opposite(), g-> vertex()) ;

        link(h, n2-> opposite(), 1) ;
        link(n1-> opposite(), g2, 1) ;

        set_facet_on_orbit(n1, new_facet()) ;
        make_facet_key(n1) ;

        n1->vertex()-> set_point(
            Geom::barycenter(h-> vertex()-> point(), g-> vertex()-> point())
        ) ;

        return n1 ;
    }
    
    MapEditor::Halfedge* MapEditor::add_facet_to_border(
        Halfedge* h, Halfedge* g
    ) {

        gx_assert(h-> is_border()) ;
        gx_assert(g-> is_border()) ;
        gx_assert(h != g) ;
        gx_assert(halfedges_on_same_facet(h,g)) ;
        
        Halfedge* h2 = h-> next() ;
        Halfedge* g2 = g-> next() ;

        Halfedge* n = new_edge() ;
        
        link(n, h2, 1) ;
        set_halfedge_vertex(n, h-> vertex()) ;

        link(g, n, 1) ;
        set_halfedge_vertex(n-> opposite(), g-> vertex()) ;

        link(h, n-> opposite(), 1) ;
        link(n-> opposite(), g2, 1) ;

        set_facet_on_orbit(n, new_facet()) ;
        make_facet_key(n) ;
        
        return n ;
    }
    
    bool MapEditor::can_split_facet(
        Halfedge* h, Halfedge* g
    ) {
        if(h == g) {
            return false ;
        }
        if(!halfedges_on_same_facet(h,g)) {
            return false ;
        }
        if(h-> next() == g || g-> next() == h) {
            return false ;
        }
        return true ;
    }
    
    bool MapEditor::split_facet(
        Halfedge* h, Halfedge* g
    ) {
        if(!can_split_facet(h,g)) {
            return false ;
        }

        Map::Facet* f = h->facet() ;

        Halfedge* result = new_edge() ;

        link(result-> opposite(), g-> next(), 1) ;
        link(result, h-> next(), 1) ;
        link(g, result, 1) ;
        link(h, result-> opposite(), 1) ;

        set_halfedge_vertex(result, h-> vertex()) ;
        set_halfedge_vertex(result-> opposite(), g-> vertex()) ;
        
        make_facet_key(result-> opposite(), h-> facet()) ;

        set_facet_on_orbit(result, new_facet(f)) ;
        make_facet_key(result) ;

        return true ;
    }

    bool MapEditor::can_join_facets(Halfedge* h) {
        if(h-> is_border_edge()) {
            return false ; 
        }
        if(h-> vertex()-> degree() < 3) {
            return false ;
        }
        if(h-> opposite()-> vertex()-> degree() < 3) {
            return false ;
        }
        if(h-> facet() == h-> opposite()-> facet()) {
            return false ;
        }
        return true ;
    }

    bool MapEditor::join_facets(Halfedge* h) {


        if(!can_join_facets(h)) {
            return false ;
        }

        Halfedge* result = h-> next() ;

        link(h-> opposite()-> prev(), h-> next(), 1) ;
        link(h-> prev(), h-> opposite()-> next(), 1) ;

        make_vertex_key(h-> prev()) ;
        make_vertex_key(h-> opposite()-> prev()) ;

        delete_facet(h-> opposite()-> facet()) ;
        
        make_facet_key(result) ;
        set_facet_on_orbit(result, result-> facet()) ;

        delete_edge(h) ;

        return true ;
    }

    MapEditor::Vertex* MapEditor::create_center_vertex(Facet* f) {
        Halfedge* h = f-> halfedge() ;
        Vertex* v = new_vertex() ;

        // Interpolate properties
        {
            int degree = f->degree() ;
            double w = 1.0 / double(degree) ;
            target()->vertex_property_manager()->interpolate_record_begin(v) ;
            Map::Halfedge* h = f->halfedge() ;
            do {
                target()->vertex_property_manager()->interpolate_record_add(w, h->vertex()) ;
                h = h->next() ;
            } while(h != f->halfedge()) ;
            target()->vertex_property_manager()->interpolate_record_end() ;
        }

        vec3 p = Geom::facet_barycenter(f) ;
        
        bool first = true ;
        Halfedge* it = h ;
        do {
            Halfedge* jt = it-> next() ;

            Halfedge* z = new_edge() ;
            link(it, z, 1) ;
            link(z-> opposite(), jt, 1) ;
            set_halfedge_vertex(z, v) ;
            set_halfedge_vertex(z-> opposite(), it-> vertex()) ;
            if(first) {
                first = false ;
                make_vertex_key(z) ;
            } else {
                link(z, it-> prev(), 1) ;
                set_facet_on_orbit(it, new_facet(f)) ;
                make_facet_key(it) ;
            }

            it = jt ;
        } while(it != h) ;

        link(h-> next(), h-> prev(), 1) ;
        set_facet_on_orbit(h, new_facet(f)) ;
        make_facet_key(h) ;

        v-> set_point(p) ;

        delete_facet(f) ;

        return v ;
    }
    

    bool MapEditor::erase_center_vertex(Vertex* v) {

        // Preprocessing: for a vertex on the border, add a facet
        {
            Halfedge* b = nil ;
            Halfedge* it = v->halfedge() ;
            do {
                if(it-> is_border()) {
                    b = it ;
                    break ;
                }
                it = it->next_around_vertex() ;
            } while(it != v->halfedge()) ;
            if(b != nil) {
                if(b-> prev() == b-> next()) {
                    return false ;
                }
                add_facet_to_border(b-> prev(), b-> next()) ;
            }
        }

        Halfedge* h = v-> halfedge()-> prev() ;
        Halfedge* it = h ;
        do {
            Halfedge* jt = it-> next() ;
            if(jt-> vertex() == v) {
                jt = jt-> opposite()-> next() ;
                delete_edge(it-> next()) ;
                delete_facet(it-> facet()) ;
                link(it, jt, 1) ;
                make_vertex_key(it) ;
            } 
            it = jt ;
        } while(it != h) ;
        set_facet_on_orbit(h, new_facet()) ;
        make_facet_key(h) ;

        delete_vertex(v) ;
        return true ;
    }
    

    MapEditor::Vertex* MapEditor::split_edge(
        Halfedge* h, bool triangulate
    ) {

        Vertex* ov1 = h->vertex() ;
        Vertex* ov2 = h->opposite()->vertex() ;

        vec3 p = Geom::edge_barycenter(h)  ;

        Vertex* v = new_vertex() ;

        Halfedge* z1 = h-> next() ;
        Halfedge* z2 = h-> opposite()-> prev() ;
        Halfedge* r = new_edge(h) ;
        link(h, r, 1) ;
        link(r-> opposite(), h-> opposite(), 1) ;
        link(r, z1, 1) ;
        link(z2, r-> opposite(), 1) ;
        
        make_vertex_key(r, h-> vertex()) ;
        make_vertex_key(h, v) ;
        set_halfedge_vertex(r-> opposite(), v) ;
        
        set_halfedge_facet(r, h-> facet()) ;
        set_halfedge_facet(r-> opposite(), h-> opposite()-> facet()) ;

        v->set_point(p) ;

        target()->vertex_property_manager()->interpolate_record_2(v, 0.5, ov1, 0.5, ov2) ;

        if(triangulate) {
            Halfedge* first = v-> halfedge() ;
            Halfedge* cur   = first ;
            Halfedge* next  = cur-> next()-> opposite() ;
            do {
                if(!cur-> is_border()) {
                    triangulate_facet(cur) ;
                }
                cur = next ;
                next = next-> next()-> opposite() ;
            } while(cur != first) ;
        }

        return v ;
    }

    bool MapEditor::can_join_edges(Vertex* v) {
        if(v-> degree() != 2) {
            return false ;
        }
        Halfedge* h = v-> halfedge() ;
        if(!h-> is_border() && h-> facet()-> nb_edges() < 4) {
            return false ;
        }
        h = h-> opposite() ;
        if(!h-> is_border() && h-> facet()-> nb_edges() < 4) {
            return false ;
        }
        return true ;
    }
    
    bool MapEditor::join_edges(Vertex* v) {

        // TODO: TexVertices

        if(!can_join_edges(v)) {
            return false ;
        }

        Halfedge* h = v-> halfedge() ;
        Halfedge* r = h-> next() ;
        Halfedge* z1 = r-> next() ;
        Halfedge* z2 = r-> opposite()-> prev() ;

        link(h, z1, 1) ;
        link(z2, h-> opposite(), 1) ;
        if(!h-> is_border()) {
            make_facet_key(h) ;
        }
        if(!h-> opposite()-> is_border()) {
            make_facet_key(h-> opposite()) ;
        }
        make_vertex_key(h) ;
     
        delete_vertex(h-> vertex()) ;
        make_vertex_key(h, r-> vertex()) ;
        delete_edge(r) ;


        return true ;
    }
    

    void MapEditor::erase_facet(Halfedge* h) {
        make_hole(h) ;
    }
    
    void MapEditor::erase_connected_component(Halfedge* h) {

        // Note: I compute a "vertices" connected component, in order
        //  to take non-manifold vertices into account. 

        std::vector<Vertex*> vertices ;
        target()-> get_connected_component(h-> vertex(), vertices) ;

        // Transferring into list of halfedges (note: I could
        //   do that on the vertices directly, but my initial
        //   code did that with halfedges ...

        std::vector<Halfedge*> halfedges ;
        {for(
            std::vector<Vertex*>::iterator it = vertices.begin() ;
            it != vertices.end() ; it++
        ) {
            Halfedge* jt = (*it)->halfedge() ;
            do {
                halfedges.push_back(jt) ;
                jt = jt->next_around_vertex() ;
            } while(jt != (*it)->halfedge()) ;
        }}


        // Step 1 : delete vertices and facets.
        {for(
            std::vector<Halfedge*>::iterator 
                it = halfedges.begin() ; it != halfedges.end() ; it++
        ) {
            Halfedge* cur = *it ;
            if(cur-> vertex() != nil && cur-> vertex()-> halfedge() == cur) {
                delete_vertex(cur-> vertex()) ;
                set_vertex_on_orbit(cur, nil) ;
            }
            if(cur-> facet() != nil && cur-> facet()-> halfedge() == cur) {
                delete_facet(cur-> facet()) ; 
                set_facet_on_orbit(cur, nil) ;
            }
        }}

        // Step 2 : delete halfedges
        {for(
            std::vector<Halfedge*>::iterator 
                it = halfedges.begin() ; it != halfedges.end() ; it++
        ) {
            Halfedge* cur = *it ;
            delete_halfedge(cur) ;
        }}
    }

//_____________________ additional functions __________________
    
    bool MapEditor::can_glue(Halfedge* h0, Halfedge* h1) {

        // Checks that both Halfedges are on the border.
        if(!h0-> is_border() || !h1-> is_border()) {
            return false ;
        }
        
        // don't glue two halfedges on a same face
        if (  
            h0->opposite()->facet() == h1-> opposite()-> facet()
	) { 
            return false ;
        }
    
        // don't merge two vertices on a same halfedge
        if(
            halfedge_exists_between_vertices(
                h0-> vertex(), h1-> opposite()-> vertex()
            ) ||
            halfedge_exists_between_vertices(
                h1-> vertex(), h0-> opposite()-> vertex()
            ) 
        ) {
            return false ;
        }

        if (
            !can_merge_vertices(h0,h1->opposite()) ||
            !can_merge_vertices(h1,h0->opposite())
        ) {
            return false ;
        }
        
        return true;
    }
    

    
    bool MapEditor::glue(Halfedge* h0, Halfedge* h1) {

        if (!can_glue(h0,h1)) {
            return false ;
        }

        vec3 new_p0 = Geom::barycenter(
            h0-> vertex()-> point(), h1-> opposite()-> vertex()-> point() 
        ) ;
        
        vec3 new_p1 = Geom::barycenter(
            h1-> vertex()-> point(), h0-> opposite()-> vertex()-> point() 
        ) ;
        

        // merge vertices if necessary
        
        Vertex* dest0 = h0->vertex() ;
        Vertex* dest1 = h1->vertex() ;
        
        Vertex* org0  = h0->opposite()->vertex() ;
        Vertex* org1  = h1->opposite()->vertex() ;
        
        if(is_locked_[dest1]) {
            is_locked_[org0] = true ;
        }

        if(is_locked_[dest0]) {
            is_locked_[org1] = true ;
        }

        if(org0 != dest1) {
            set_vertex_on_orbit(h1, org0) ;
            delete_vertex(dest1) ;
        }
        
        if (org1 != dest0) {
            set_vertex_on_orbit(h0, org1) ;
            delete_vertex (dest0);
        }

        // set halfedge connections

        link(h1-> prev(), h0-> next(), 1) ;
        link(h0-> prev(), h1-> next(), 1) ;
        link(h0-> opposite(), h1-> opposite(), 2) ;
        make_vertex_key(h0-> opposite()) ;
        make_vertex_key(h1-> opposite()) ;

        org1-> set_point(new_p0) ;
        org0-> set_point(new_p1) ;
        
        delete_halfedge(h0);
        delete_halfedge(h1);

        return true;
    }
    
    bool MapEditor::can_unglue(Halfedge* h) {
        if(h-> is_border_edge()) {
            return false ;
        }
        return (
            h->vertex()->is_on_border() ||
            h->opposite()->vertex()->is_on_border()
        ) ;
    }

    
    bool MapEditor::unglue(Halfedge* h0, bool check) {

        // TODO: tex vertices : if already dissociated,
        // do not create a new Texvertex.


        if(check && !can_unglue(h0)) {
            return false ;
        }

        if(h0-> is_border_edge()) {
            return false ;
        }

        Halfedge* h1 = h0-> opposite() ;
        Vertex* v0 = h0-> vertex() ;
        Vertex* v1 = h1-> vertex() ;        
        
        bool v0_on_border = v0->is_on_border() ;
        bool v1_on_border = v1->is_on_border() ;
        
        gx_assert(!check || (v0_on_border || v1_on_border)) ;
        
        Halfedge* n0 = new_halfedge(h0) ;
        Halfedge* n1 = new_halfedge(h1) ;
        link(n0, n1, 2) ;
        set_halfedge_next(n0,n1) ;
        set_halfedge_prev(n0,n1) ;
        set_halfedge_next(n1,n0) ;
        set_halfedge_prev(n1,n0) ;

        link(h0, n0, 2) ;
        link(h1, n1, 2) ;

        // If v1 is on the border, find the predecessor and
        // the successor of the newly created edges, and
        // split v1 into two vertices. 
        if(v1_on_border) {
            Halfedge* next0 = h0-> prev()-> opposite() ;
            while(!next0-> is_border()) {
                next0 = next0-> prev()-> opposite() ;
            }
            gx_assert(next0 != h0) ;
            Halfedge* prev1 = h1-> next()-> opposite() ;
            while(!prev1-> is_border()) {
                prev1 = prev1-> next()-> opposite() ;
            }
            gx_assert(prev1 != h1) ;
            gx_assert(prev1-> vertex() == v1) ;
            gx_assert(prev1-> next() == next0) ;
            link(n0, next0, 1) ;
            link(prev1, n1, 1) ;
            set_vertex_on_orbit(n0, new_vertex(v1)) ;
            
            make_vertex_key(n0) ;
            make_vertex_key(h1) ;
        } else {
            set_halfedge_vertex(n0, v1) ;
        }

        // If v0 is on the border, find the predecessor and
        // the successor of the newly created edges, and
        // split v0 into two vertices. 
        if(v0_on_border) {
            Halfedge* prev0 = h0-> next()-> opposite() ;
            while(!prev0-> is_border()) {
                prev0 = prev0-> next()-> opposite() ;
            }
            gx_assert(prev0 != h0) ;
            Halfedge* next1 = h1-> prev()-> opposite() ;
            while(!next1-> is_border()) {
                next1 = next1-> prev()-> opposite() ;
            }
            gx_assert(next1 != h1) ;
            gx_assert(prev0-> next() == next1) ;
            link(prev0, n0, 1) ;
            link(n1, next1, 1) ;
            set_vertex_on_orbit(n1, new_vertex(v0)) ;

            make_vertex_key(n1) ;
            make_vertex_key(h0) ;

        } else {
            set_halfedge_vertex(n1, v0) ;
        }

        return true ;
    }


    bool MapEditor::can_collapse_edge(Halfedge* h) {

        {// disable glueing problems

            if ((!h->is_border() &&
                    h->next()->opposite()->facet() 
                    == h->prev()->opposite()->facet())
                || (!h->opposite()->is_border() &&
                    h->opposite()->next()->opposite()->facet() 
                    == h->opposite()->prev()->opposite()->facet())
	    )
                return false;
        }
        {// don't remove more than one vertex
            if (// it's a triangle
                h->next()->next()->next()==h            
                // the vertex is alone on border
                && h->next()->opposite()->is_border()  
                && h->prev()->opposite()->is_border()   
            )
                return false ;

            // the same on the other face
            if (// it's a triangle
                h->opposite()->next()->next()->next()==h            
                // the vertex is alone on border
                && h->opposite()->next()->opposite()->is_border()
                && h->opposite()->prev()->opposite()->is_border()
            )
                return false ;
        }

        // don't do stupid holes
        {
            if (
                (h->is_border()  && h->next()->next()->next()==h) ||
                (
                    h->opposite()->is_border() && 
                    h->opposite()->next()->next()->next()==h->opposite()
                )
	    ) { 
                return false;
            }
        }

        // don't merge holes (i.e. don't split surface)
        {
            if (
                !h->is_border() &&
                !h->opposite()->is_border() &&
                h->vertex()->is_on_border() &&
                h->opposite()->vertex()->is_on_border()
            ) {
                return false;
            }
        }

        // be carefull of the toblerone case (don't remove volumes)
        {
            Halfedge* cir = h ;
            int nb_twice=0;

            std::set<Vertex*> marked ;

            // do { 
            //    cir->opposite()->vertex()->set_id(0);
            //    cir = cir->next_around_vertex();
            // } while ( cir != h);

            cir = h->opposite();
            do { 
                marked.insert(cir->opposite()->vertex()) ;
                cir = cir->next_around_vertex();
            } while ( cir != h->opposite());
            
            cir = h;
            do {
                if (
                    marked.find(cir->opposite()->vertex()) != marked.end()
                ) {
                    nb_twice++;
                }
                marked.insert(cir->opposite()->vertex()) ;
                cir = cir->next_around_vertex();
            }while ( cir != h);
            
            if (h->next()->next()->next()==h)  {
                nb_twice--;
            }
            if (h->opposite()->next()->next()->next()==h->opposite()) { 
                nb_twice--;
            }
            
            if (nb_twice > 0) {
                //std::cerr<<" \nbe carefull of the toblerone case";
                return false;
            }
        }
        return true ;


/*

if(
!h->next()->opposite()->is_border() ||
!h->prev()->opposite()->is_border()
) {
return false ;
}
if(
!h->opposite()->next()->opposite()->is_border() || 
!h->opposite()->prev()->opposite()->is_border()
) {
return false ;
}
return true ;
*/
    }

    bool MapEditor::collapse_edge(Halfedge* h) {

        if(!can_collapse_edge(h)) {
            return false ;
        }
      
        Vertex* dest = h->vertex() ;

        // everyone has the same vertex
        {	
            Vertex* v = h->opposite()->vertex() ;
            Halfedge* i ;
            Halfedge* ref;
            i = ref = h->opposite();
            do {
                Halfedge* local = i ;
                set_halfedge_vertex(local, dest) ;
                i = i->next_around_vertex() ;
            } while (i != ref);
            delete_vertex(v);
        }


        // remove links to current edge (facet & vertex)
        Halfedge* hDle = h;
        if ( !h->is_border() ) {
            set_facet_halfedge( hDle->facet(), hDle->next() ) ;
        }

        if (!h->opposite()->is_border()) {
            set_facet_halfedge(
                hDle->opposite()->facet(), hDle->opposite()->next() 
            ) ;
        }
        set_vertex_halfedge( dest, hDle->next()->opposite() ) ;

        Halfedge* f0 = h->next() ;
        Halfedge* f1 = h->opposite()->prev() ;

        // update prev/next links
        { 
            Halfedge* e = h->next() ;      
            link(hDle-> prev(), e, 1) ;
            e = hDle->opposite()->next() ;
            link(hDle->opposite()->prev(), e, 1) ;
        }
      
        // remove null faces
        if (f0->next()->next() == f0) {
            remove_null_face(f0);
        }

        if (f1->next()->next() == f1) {
            remove_null_face(f1);
        }
        
        delete_edge(hDle);
        
        return true ;
    }


//_____________________ utilities _____________________________
    
    void MapEditor::remove_null_face(Halfedge* f0) {
        Halfedge* border = f0-> next()-> opposite() ;

        //remove facet
        delete_facet(f0-> facet());

        // set link (fake set_opposite) to others
        link(f0, border-> next(), 1) ;
        link(border-> prev(), f0, 1) ;
        set_halfedge_facet(f0, border-> facet()) ;
        
        // set links from others
        if (!f0->is_border()){
            make_facet_key(f0) ;
        }
        make_vertex_key(f0) ;
        make_vertex_key(f0-> opposite()) ;
        delete_edge(border); 
    }

    bool MapEditor::can_merge_vertices(
        Halfedge* h0, Halfedge* h1
    ) {
        // It's OK it they are already the same !
        if (h0->vertex() == h1->vertex()) {
            return true;
        }
        return (
            orbits_are_compatible(h0, h1) &&
            orbits_are_compatible(h1, h0) 
        ) ;
    } ;

    bool MapEditor::orbits_are_compatible(
        Halfedge* h0, Halfedge* h1
    ) {
        
        Halfedge* cir_h0 = h0 ;
        do {
            // Number of potential opposites half_edges
            // (should not be greater than 1)
            int nb_common = 0 ;
            Halfedge* hh0 = cir_h0-> opposite() ;
            Halfedge* cir_h1 = h1 ;
            do {
                Halfedge* hh1 = cir_h1-> opposite() ;
                if (
                    hh0->vertex() == hh1->vertex() || 
                    (
                        hh0->vertex() == h0->opposite()->vertex() && 
                        hh1->vertex() == h1->opposite()->vertex()
                    ) || (
                        hh0->vertex() == h1->opposite()->vertex() && 
                        hh1->vertex() == h0->opposite()->vertex()
                    )
                ) {
                    if ( 
                        hh0->opposite()->is_border() && hh1->is_border() || 
                        hh0->is_border() && hh1->opposite()->is_border()
                    ) {
                        // Found a potential opposite edge.
                        nb_common++ ; 
                    } else {
                        // Potential opposite edge not on the border.
                        return false ;
                    }
                }
                cir_h1 = cir_h1->next_around_vertex() ;
            } while(cir_h1 != h1) ;
            if (nb_common > 1) {
                return false ;
            }
            cir_h0 = cir_h0->next_around_vertex() ;
        } while(cir_h0 != h0) ;
        return true ;
    }
    
    bool MapEditor::halfedge_exists_between_vertices(
        Vertex* v1, Vertex* v2
    ) {
        Halfedge* cir = v1->halfedge() ;
        do {
            if(cir-> opposite()-> vertex() == v2) {
                return true ;
            }
            cir = cir->next_around_vertex() ;
        } while(cir != v1->halfedge()) ;
        return false ;
    }


    bool MapEditor::halfedges_on_same_vertex(
        Halfedge* h1, Halfedge* h2
    ) {
        Halfedge* it = h1 ;
        do {
            if(it == h2) {
                return true ;
            }
            it = it->next_around_vertex() ;
        } while(it != h1) ;
        return false ;
    }

    bool MapEditor::halfedges_on_same_facet(
        Halfedge* h1, Halfedge* h2
    ) {
        Halfedge* it = h1 ;
        do {
            if(it == h2) {
                return true ;
            }
            it = it->next() ;
        } while(it != h1) ;
        return false ;
    }


    void MapEditor::split_surface(SplitMode mode) {

        vertex_id_.bind(target()) ;
        clear_all_vertex_ids() ;
        
        switch(mode) {
        case split_barycentric:
            split_all_edges() ;
            split_all_facets() ;
            break ;
        case split_faces:
            split_all_facets() ;
            break ;
        case split_quad:
            split_all_edges(1) ;
            split_all_facets(2) ;
            remove_edges(0,2) ;
            break ;
        case split_dual: {
            border_facet_.bind(target()) ;
            fill_holes() ;
            split_all_edges(1) ;
            split_all_facets(2) ;
            remove_vertices(0) ;
            remove_vertices(1) ;
            std::vector<Halfedge*> to_remove ;
            { FOR_EACH_VERTEX(Map, target(), it) {
                if(vertex_id_[it] == -1) {
                    Map::Halfedge* h = it->halfedge() ;
                    do {
                        if(h->facet() != nil) {
                            to_remove.push_back(h) ;
                        }
                        h = h->next_around_vertex() ;
                    } while(h != it->halfedge()) ;
                }
            }}
            for(unsigned int i = 0; i < to_remove.size(); i++) {
                make_hole(to_remove[i]) ;
            }
            border_facet_.unbind() ;
        } break ;
        case split_cvfe:
            split_all_edges(1) ;
            split_all_facets(2) ;
            remove_vertices(0) ;
            break ;
        case split_corners:
            split_all_edges(1) ;
            insert_edges_in_all_facets(0) ;
            break ;
        case split_triangulate:
            triangulate_all_facets() ;
            break ;
        case split_triangulate_center: {
            std::vector<Facet*> facets ;
            {FOR_EACH_FACET(Map, target(), it) {
                if(!it->is_triangle()) {
                    facets.push_back(it) ;
                }
            }}
            {for(std::vector<Facet*>::iterator it = facets.begin() ;
                 it != facets.end(); it++
            ) {
                create_center_vertex(*it) ;
            }}
        } break ;
        case split_cloop:
            split_all_edges(1) ;
            insert_edges_in_all_facets(0) ;
			break ;
        }
        vertex_id_.unbind() ;
    }

//_________________________________________________________

    void MapEditor::split_all_facets(int id) {
        std::vector<Facet*> facets ;
        FOR_EACH_FACET(Map, target(), it) {
            facets.push_back(it) ;
        }
        for(std::vector<Facet*>::iterator it = facets.begin() ;
            it != facets.end(); it++
        ) {
            int cur_id = id ;
            if(border_facet_.is_bound() && border_facet_[*it]) {
                cur_id = -1 ;
            }
            Vertex* v = create_center_vertex(*it) ;
            vertex_id_[v] = cur_id ;
        }
    }

    void MapEditor::split_all_edges(int id) {
        std::vector<Halfedge*> edges ;
        FOR_EACH_HALFEDGE(Map, target(), it) {
            Halfedge* p_it = it ;
            Halfedge* p_opp = it-> opposite() ;
            if(p_it < p_opp) {
                edges.push_back(it) ;
            }
        }
        for(std::vector<Halfedge*>::iterator it = edges.begin() ;
            it != edges.end(); it++
        ) {
            Vertex* v = split_edge(*it) ;
            vertex_id_[v] = id ;
        }
    }

    void MapEditor::remove_edges(int id1, int id2) {
        std::vector<Halfedge*> edges ;
        {FOR_EACH_HALFEDGE(Map, target(),it) {
            if(
                !it-> is_border_edge() &&
                vertex_id_[it-> vertex()] == id1 &&
                vertex_id_[it-> opposite()-> vertex()] == id2
            ) {
                edges.push_back(it) ;
            }
        }}
        {for(std::vector<Halfedge*>::iterator it = edges.begin() ;
            it != edges.end(); it++
        ) {
            join_facets(*it) ;
        }}
    }


    void MapEditor::remove_vertices(int id) {
        std::vector<Vertex*> vertices ;
        {FOR_EACH_VERTEX(Map, target(),it) {
            if(vertex_id_[it] == id) {
                vertices.push_back(it) ;
            }
        }}
        {for(std::vector<Vertex*>::iterator it = vertices.begin() ;
            it != vertices.end(); it++
        ) {
            if(can_join_edges(*it)) {
                join_edges(*it) ;
            } else {
                if(!(*it)->is_on_border()) {
                    erase_center_vertex(*it) ;
                }
            }
        }}
    }

    void MapEditor::insert_edges_in_facet(Facet* f, int id) {
        Halfedge* start = f-> halfedge() ;
        do {
            start = start-> next() ;
        } while(
            vertex_id_[start-> next()-> vertex()] != id &&
            start != f-> halfedge() 
        ) ;
        
        gx_assert(vertex_id_[start-> next()-> vertex()] == id) ;

        Halfedge* cur1 = start ;
        Halfedge* cur2 = cur1-> next()-> next() ;
        Halfedge* next = cur2-> next()-> next() ;

        do {
            gx_assert(
                vertex_id_[cur1-> next()-> vertex()] == id
            ) ;
            split_facet(cur1, cur2) ;
            cur1 = cur2-> next()-> opposite() ;
            cur2 = next ;
            next = next-> next()-> next() ;
        } while(cur1-> vertex() != start-> vertex()) ;
    }
    
    void MapEditor::insert_edges_in_all_facets(int id) {
        std::vector<Facet*> facets ;
        {FOR_EACH_FACET(Map, target(),it) {
            facets.push_back(it) ;
        }}

        {for(
            std::vector<Facet*>::iterator it = facets.begin() ;
            it != facets.end() ; it++
        ) {
            insert_edges_in_facet(*it, id) ;
        }}
        
    }

    void MapEditor::triangulate_facet(Halfedge* start) {
        Halfedge* cur = start-> next()-> next() ;
        while(cur-> next() != start) {
            split_facet(start, cur) ;
            cur = cur-> next()-> opposite()-> next() ;
        }
    }

    void MapEditor::triangulate_all_facets() {
        std::vector<Facet*> facets ;
        {FOR_EACH_FACET(Map, target(),it) {
            facets.push_back(it) ;
        }}

        {for(
            std::vector<Facet*>::iterator it = facets.begin() ;
            it != facets.end() ; it++
        ) {
            triangulate_facet((*it)-> halfedge()) ;
        }}
        
    }


//_________________________________________________________

    void MapEditor::clear_all_vertex_ids(int value) {
        FOR_EACH_VERTEX(Map, target(), it) {
            vertex_id_[it] = value ;
        }
    }

//_________________________________________________________

    void MapEditor::reorient_facet( Map::Halfedge* first) {
        if ( first == nil) {
            return;
        }
        Map::Halfedge* last  = first;
        Map::Halfedge* prev  = first;
        Map::Halfedge* start = first;
        first = first->next();
        Map::Vertex* new_v = start-> vertex() ;
        // Note: a _var is used to prevent deallocation
        while (first != last) {
            Map::Vertex*  tmp_v = first-> vertex() ;
            set_halfedge_vertex(first, new_v) ;
            set_vertex_halfedge(first->vertex(), first) ;
            new_v = tmp_v;
            Map::Halfedge* next = first->next();
            set_halfedge_next(first, prev) ;
            set_halfedge_prev(first, next) ;
            prev  = first;
            first = next;
        }
        set_halfedge_vertex(start, new_v) ;
        set_vertex_halfedge(start->vertex(), start) ;
        Map::Halfedge* next = start->next();
        set_halfedge_next(start, prev) ;
        set_halfedge_prev(start, next) ;
    }


    void MapEditor::inside_out() {

        { FOR_EACH_FACET(Map, target(), it) {
            reorient_facet(it->halfedge()) ;
        }}

        // Note: A border edge is now parallel to its opposite edge.
        // We scan all border edges for this property. If it holds, we
        // reorient the associated hole and search again until no border
        // edge with that property exists any longer. Then, all holes are
        // reoriented.

        { FOR_EACH_HALFEDGE(Map, target(), it) {
            if ( 
                it->is_border() &&
                it->vertex() == it->opposite()->vertex()
            ) {
                reorient_facet(it);
            }
        }}

    }

    void MapEditor::copy_properties(Vertex* to, Vertex* from) {
        target()->vertex_property_manager()->copy_record(to, from) ;
    }
    
    void MapEditor::copy_properties(Halfedge* to, Halfedge* from) {
        target()->halfedge_property_manager()->copy_record(to, from) ;
    }
    
    void MapEditor::copy_properties(Facet* to, Facet* from) {
        target()->facet_property_manager()->copy_record(to, from) ;
    }
    
}

