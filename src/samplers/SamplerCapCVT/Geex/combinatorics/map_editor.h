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

#ifndef __GEEX_COMBINATORICS_MAP_EDITOR__
#define __GEEX_COMBINATORICS_MAP_EDITOR__

#include <Geex/combinatorics/map.h>
#include <Geex/combinatorics/map_properties.h>

namespace Geex {

    enum SplitMode {
        split_barycentric = 0, 
        split_quad = 1, 
        split_faces = 2, 
        split_dual = 3, 
        split_cvfe = 4,
        split_corners = 5, 
        split_triangulate = 6, 
        split_triangulate_center = 7,
        split_catmull_clark  = 8,
		split_cloop          = 9,
		split_quadtri        = 10,
		split_sqrt3          = 11
    } ;

//_________________________________________________________

    class GEEX_API MapEditor : public MapMutator {
    public:
    
        typedef ::Geex::SplitMode SplitMode ;
    
        MapEditor(Map* target = nil) ; 
        virtual void set_target(Map* target) ;

        // _________________ CGAL interface __________

        Halfedge* make_polygon(int nb) ;
        Halfedge* make_triangle() ;
        Halfedge* make_triangle(
            const vec3& p1, const vec3& p2, const vec3& p3
        ) ;

        /** 
         * destroys the facet referred two by h.
         * precondition: !h-> is_border() 
         */
        void make_hole(Halfedge* h) ;

        /**
         * fills a hole (i.e. a border).
         * precondition: h-> is_border()
         */
        Halfedge* fill_hole(Halfedge* h) ;
        void fill_holes(int max_size = -1) ;

        Halfedge* add_vertex_and_facet_to_border(
            Halfedge* h, Halfedge* g
        ) ;
        Halfedge* add_facet_to_border(
            Halfedge* h, Halfedge* g
        ) ;

        bool can_split_facet(Halfedge* h, Halfedge* g) ;
        bool split_facet(Halfedge* h, Halfedge* g) ;
        
        bool can_join_facets(Halfedge* h) ;
        bool join_facets(Halfedge* h) ;
        
        Vertex* create_center_vertex(Facet* f) ;
        bool erase_center_vertex(Vertex* v) ;

        /**
         * Insert a new vertex in the edge referred to by h.
         * If triangulate is set to true, triangulate the affected facets.
         */
        Vertex* split_edge(Halfedge* h, bool triangulate = false) ;
        bool can_join_edges(Vertex* v) ;
        bool join_edges(Vertex* v) ;

        void erase_facet(Halfedge* h) ;
        void erase_connected_component(Halfedge* h) ;

        // _________________ Additional functions ____      

        /**
         * Checks wether the two specified half-edges can be glued,
         * from a topological point of view. h0 and h1 should point 
         * in reverse direction, and should be on the
         * border. TODO: check if it can work for polygons.
         */
        bool can_glue(Halfedge* h0, Halfedge* h1) ;

        /**
         * h0 and h1 should point in reversed direction, and 
         * should be on the border. 
         * @param do_merge_vertices if set to false, the operation does not
         *   affect the TexVertices attached to the concerned Vertices.
         */
        bool glue(
            Halfedge* h0, Halfedge* h1
        ) ;


        /**
         * It is not allowed to collapse an edge of a
         *  triangle that has its two other edges on the border.
         */
        // TODO: check if it works for polygons.
        bool can_collapse_edge(Halfedge* h) ;

        bool collapse_edge(Halfedge* h) ;

        /**
         * An edge on the border cannot be unglued. One of the
         * extremities of the edge should be on the border.
         */
        bool can_unglue(Halfedge* h) ;

        /**
         * Separates the two halfedges indicated by h, and
         * generates new vertices as needed.
         * If h is already on the border, does nothing.
         */
          
        bool unglue(Halfedge* h, bool check = true) ;

        /** Checks whether a Vertex has several TexVertices */
        bool vertex_is_split(const Vertex* v) ;

        void split_surface(SplitMode mode) ;

        /** 
         * Triangulates a facet by creating triangles
         * radiating from start-> vertex()
         */
        void triangulate_facet(Halfedge* start) ;

        /**
         * inverts the orientation of all the facets.
         */
        void inside_out() ;
        
        void copy_properties(Vertex* to, Vertex* from) ;
        void copy_properties(Halfedge* to, Halfedge* from) ;
        void copy_properties(Facet* to, Facet* from) ;

    protected:

        //_________________ utilities ____________________

        /**
         * Removes faces having only two edges.
         * @param f0 is an interior halfedge of the face to be removed.
         */
        void remove_null_face(Halfedge* f0) ;

        /**
         * Checks wheter the vertices pointed by h0 and h1 can be
         * merged. It is called twice by can_glue(), once per 
         * orientation of the edges.
         */
        bool can_merge_vertices(Halfedge* h0, Halfedge* h1) ;
        
        /**
         * To be explained by Nico.
         * Note: should be called with both (h0,h1) and with (h1,h0)
         */
        bool orbits_are_compatible(
            Halfedge* h0, Halfedge* h1
        ) ;

        /**
         * Checks the existence of an half_edge e such that
         * e-> vertex() = v1 and e-> opposite()-> vertex() = v2
         */
        bool halfedge_exists_between_vertices(
            Vertex* v1, Vertex* v2
        ) ;

        bool halfedges_on_same_vertex(
            Halfedge* h1, Halfedge* h2
        ) ;

        bool halfedges_on_same_facet(
            Halfedge* h1, Halfedge* h2
        ) ;

        // _______________ splitting ________________________

        // These functions are used by the implementation of
        // split_surface(SplitMode). All splitting operations
        // are expressed as a subset of the barycentric 
        // triangulation. The unused edges are then removed.
        // These functions require the vertex_id_ property
        // to be bound to the surface before operating.

        /**
         * creates a new vertex at the center of each edge, 
         * and sets their ids as specified.
         * The vertex_id_ property should be bound.
         */
        void split_all_edges(int id = 1) ;

        /**
         * creates a new vertex at the center of each facet,
         * connected to the vertices of the facet. The ids
         * of the created vertices are set to id. The vertex_id_
         * property should be bound.
         */
        void split_all_facets(int id = 1) ;

        /**
         * removes all the halfedges connecting a vertex of id id1
         * to a vertex of id id2. The vertex_id_ property should
         * be bound.
         */
        void remove_edges(int id1, int id2) ;

        /**
         * removes all the vertices having their id equal to id.
         * The vertex_id_ property should be bound.
         */
        void remove_vertices(int id) ;

        /**
         * creates new edges in the specified facet, by cutting
         * the corners corresponding to a vertex of id id.
         * The vertex_id_ property should be bound.
         */
        void insert_edges_in_facet(Facet* f, int id) ;

        /**
         * calls insert_edges_in_facet() for all facets.
         */
        void insert_edges_in_all_facets(int id) ;

        /**
         * triangulates all the facets, by radiating around one of
         * the vertices of each facet. Note that the facets should be
         * convex.
         */
        void triangulate_all_facets() ;

        // _______________ ids __________________________

        void clear_all_vertex_ids(int value = 0) ;

    protected:
        void reorient_facet(Map::Halfedge* first) ;

    private:
        MapVertexProperty<int> vertex_id_ ;
        MapFacetProperty<bool> border_facet_ ;  
        MapVertexLock is_locked_ ;
    } ;


//_________________________________________________________

}
#endif

