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
 
 
 
 
 
 
 

#ifndef __GEEX_COMBINATORICS_MAP_BUILDER__
#define __GEEX_COMBINATORICS_MAP_BUILDER__

#include <Geex/combinatorics/map.h>
#include <Geex/combinatorics/map_properties.h>
#include <Geex/combinatorics/map_io.h>
#include <Geex/image/types.h>
#include <vector>
#include <string>
#include <set>

namespace Geex {

//_________________________________________________________

    class GEEX_API MapBuilder : public MapMutator {
    private:
        enum state {initial, surface, facet, final} ;

    public:
        MapBuilder(Map* target = nil) : MapMutator(target), state_(initial) {
            quiet_ = false ;
        }

        virtual void begin_surface() ;
        virtual void end_surface() ;
        virtual void reset() ;

        virtual void add_vertex(const vec3& p) ;
        virtual void add_vertex(unsigned int id, const vec3& p) ;

        virtual void add_tex_vertex(const vec2& p) ;
        virtual void add_tex_vertex(unsigned int id, const vec2& p) ;

        virtual void begin_facet() ;
        virtual void end_facet() ;
        virtual void add_vertex_to_facet(int i) ;
        virtual void set_corner_tex_vertex(int i) ;

        virtual void lock_vertex(int i) ;
        virtual void unlock_vertex(int i) ;

		
		virtual void create_vertices(unsigned int nb_vertices, bool with_colors = false) ;
		virtual void reserve_facets(unsigned int nb_facets) ;
		virtual void set_vertex(unsigned int v, const vec3& p) ;
		virtual void set_vertex_color(unsigned int v, const Color& c) ;

        Map::Vertex* current_vertex() ;
        Map::Vertex* vertex(index_t i) ;
        size_t nb_vertices() ;
        Map::Facet* current_facet() ;
		Map::TexVertex* current_tex_vertex() ;        
		Map::TexVertex* tex_vertex(int i) ;
        void set_quiet(bool quiet) { quiet_ = quiet ; }

    protected:

        //   Important note: in this class, all the Stars correspond to the
        // set of halfedges radiating FROM a vertex (i.e. h->vertex() != v
        // if h belongs to Star(v) ).

        void add_vertex_internal(unsigned int id, const vec3& p) ;
        void add_vertex_internal(const vec3& p) ;

        void add_tex_vertex_internal(const vec2& p) ;
        void add_tex_vertex_internal(unsigned int id, const vec2& p) ;


        void begin_facet_internal() ;
        void end_facet_internal() ;
        void add_vertex_to_facet_internal(Vertex* v) ;
        void set_corner_tex_vertex_internal(Map::TexVertex* tv) ;
        Vertex* copy_vertex(Vertex* from_vertex) ;

        std::vector<Vertex*>    facet_vertex_ ;
        std::vector<Map::TexVertex*> facet_tex_vertex_ ;

    private:

        Map::Halfedge* new_halfedge_between(Vertex* from, Vertex* to) ;
        Map::Halfedge* find_halfedge_between(Vertex* from, Vertex* to) ;

        /**
         * Checks whether a vertex is manifold, by
         * checking that the star_ property and the
         * computed star have the same number of Halfedges.
         * Note: this test is valid only if the borders
         * have been constructed.
         */
        bool vertex_is_manifold(Vertex* v) ;

        /**
         * splits a non-manifold vertex into several vertices, as
         * needed. returns true if the vertex was non-manifold.
         */
        bool split_non_manifold_vertex(Vertex* v) ;

        /**
         * used by split_non_manifold_vertex:
         * sets v's halfedge pointer,
         * sets the halfedge vertex pointers to v, 
         * removes the concerned halfedges from star,
         * updates the star of the concerned vertex,
         * if borders are found, connects them
         */
        void disconnect_vertex(
            Map::Halfedge* start, Map::Vertex* v, 
            std::set<Map::Halfedge*>& star
        ) ;

        void terminate_surface() ;
        friend void GEEX_API load_map(std::istream& input, Map* map, bool load_tex_coord, bool flip) ;

        void transition(state from, state to) ;
        void check_state(state s) ;
        std::string state_to_string(state s) ;

        state state_ ;
        std::vector<Map::Vertex*> vertex_ ;
        std::vector<Map::TexVertex_var> tex_vertex_ ;

        MapHalfedgeProperty<Map::TexVertex_var> halfedge_tex_vertex_ ;

        Map::Facet* current_facet_ ;
        Vertex* current_vertex_ ;
        Vertex* first_vertex_in_facet_ ;
        Map::TexVertex* first_tex_vertex_in_facet_ ;
        Map::Halfedge* first_halfedge_in_facet_ ;
        Map::Halfedge* current_halfedge_ ;

        typedef std::vector<Map::Halfedge*> Star ;
        MapVertexProperty<Star> star_ ;
		MapVertexProperty<Color> color_ ;
        MapVertexLock is_locked_ ;

        int nb_isolated_v_ ;
        int nb_non_manifold_v_ ;
        int nb_duplicate_e_ ;
        bool quiet_ ;
    } ;

//_________________________________________________________

}
#endif

