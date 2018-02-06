/*
 *  OGF/Graphite: Geometry and Graphics Programming Library + Utilities
 *  Copyright (C) 2000 Bruno Levy
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
 *  Contact: Bruno Levy
 *
 *     levy@loria.fr
 *
 *     ISA Project
 *     LORIA, INRIA Lorraine, 
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX 
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 */
 

#include <Geex/combinatorics/enumerate.h>
#include <Geex/properties/property.h>
#include <Geex/basics/assert.h>

#include <stack>

namespace Geex {

//_________________________________________________________

    int enumerate_vertices(
        Map* map, Property<Map::Vertex, int>& id, int start, int step
    ) {
        gx_assert(start >= 0) ;
        gx_assert(step > 0) ;
        gx_assert(
            id.property_manager() == map->vertex_property_manager()
        ) ;
        int cur_id = start ;
        FOR_EACH_VERTEX(Map,map, it) {
            id[it] = cur_id ;
            cur_id += step ;
        }
        return (cur_id - start) / step ;
    }

    int enumerate_halfedges(
        Map* map, Property<Map::Halfedge, int>& id, int start, int step
    ) {
        gx_assert(start >= 0) ;
        gx_assert(step > 0) ;
        gx_assert(
            id.property_manager() == map->halfedge_property_manager()
        ) ;
        int cur_id = start ;
        FOR_EACH_HALFEDGE(Map,map, it) {
            id[it] = cur_id ;
            cur_id += step ;
        }
        return (cur_id - start) / step ;
     }

    int enumerate_facets(
        Map* map, Property<Map::Facet, int>& id, int start, int step
    ) {
        gx_assert(start >= 0) ;
        gx_assert(step > 0) ;
        gx_assert(
            id.property_manager() == map->facet_property_manager()
        ) ;
        int cur_id = start ;
        FOR_EACH_FACET(Map, map, it) {
            id[it] = cur_id ;
            cur_id += step ;
        }
        return (cur_id - start) / step ;
    }

    //int enumerate_tex_vertices(
    //    Map* map, Property<Map::TexVertex, int>& id, int start, int step
    //) {
    //    gx_assert(start >= 0) ;
    //    gx_assert(step > 0) ;
    //    gx_assert(
    //        id.property_manager() == map->tex_vertex_property_manager()
    //    ) ;

    //    { FOR_EACH_HALFEDGE(Map, map, it) {
    //        id[it->tex_vertex()] = -1 ;
    //    }}
    //    int cur_id = start ;
    //    { FOR_EACH_HALFEDGE(Map, map, it) {
    //        if(id[it->tex_vertex()] == -1) {
    //            id[it->tex_vertex()] = cur_id ;
    //            cur_id += step ;
    //        }
    //    }}
    //    return (cur_id - start) / step ;
    //}


    static void propagate_connected_component(
        Map* map, Property<Map::Vertex, int>& id,
        Map::Vertex* v, int cur_id
    ) {
        std::stack<Map::Vertex*> stack ;
        stack.push(v) ;
        
        while(!stack.empty()) {
            Map::Vertex* top = stack.top() ;
            stack.pop() ;
            if(id[top] == -1) {
                id[top] = cur_id ;
                Map::Halfedge* it = top->halfedge() ;
                do {
                    Map::Vertex* v = it-> opposite()-> vertex() ;
                    if(id[v] == -1) {
                        stack.push(v) ;
                    }
                    it = it->next_around_vertex() ;
                } while(it != top->halfedge()) ;
            }
        }
    }



    int enumerate_connected_components(
        Map* map, Property<Map::Vertex, int>& id
    ) {
        gx_assert(
            id.property_manager() == map->vertex_property_manager()
        ) ;
        { FOR_EACH_VERTEX(Map,map,it) {
            id[it] = -1 ;
        }}
        int cur_id = 0 ;
        { FOR_EACH_VERTEX(Map,map,it) {
            if(id[it] == -1) {
                propagate_connected_component(map, id, it, cur_id) ;
                cur_id++ ;
            }
        }}
        return cur_id ;
    }


    static void propagate_connected_component(
        Map* map, Property<Map::Facet, int>& id,
        Map::Facet* f, int cur_id
    ) {
        std::stack<Map::Facet*> stack ;
        stack.push(f) ;
        
        while(!stack.empty()) {
            Map::Facet* top = stack.top() ;
            stack.pop() ;
            if(id[top] == -1) {
                id[top] = cur_id ;
                Map::Halfedge* it = top->halfedge() ;
                do {
                    Map::Facet* f = it-> opposite()-> facet() ;
                    if(f != nil && id[f] == -1) {
                        stack.push(f) ;
                    }
                    it = it->next() ;
                } while(it != top->halfedge()) ;
            }
        }
    }


    int enumerate_connected_components(
        Map* map, Property<Map::Facet, int>& id
    ) {
        gx_assert(
            id.property_manager() == map->facet_property_manager()
        ) ;
        { FOR_EACH_FACET(Map,map,it) {
            id[it] = -1 ;
        }}
        int cur_id = 0 ;
        { FOR_EACH_FACET(Map,map,it) {
            if(id[it] == -1) {
                propagate_connected_component(map, id, it, cur_id) ;
                cur_id++ ;
            }
        }}
        return cur_id ;
    }
        
    
//_________________________________________________________

}

