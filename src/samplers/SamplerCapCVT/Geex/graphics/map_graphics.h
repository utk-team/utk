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
 
#ifndef __GEEX_GRAPHICS_MAP_GRAPHICS__
#define __GEEX_GRAPHICS_MAP_GRAPHICS__

#include <Geex/combinatorics/map.h>
#include <Geex/combinatorics/map_properties.h>
#include <Geex/graphics/opengl.h>
#include <Geex/graphics/color.h>

namespace Geex {

    class GEEX_API MapGraphics {
    public:

        MapGraphics(Map* map = nil) ;
        Map* map() const { return map_ ; }
        void set_map(Map* map) ;

        void draw() ;
        void recompute_clip_values() ;

        const vec4& vertex_color() const { return vertex_color_ ; }
        void set_vertex_color(const vec4& c) { vertex_color_ = c ; }

        const vec4& edge_color() const { return edge_color_ ; }
        void set_edge_color(const vec4& c) { edge_color_ = c ; }

        const vec4& facet_color() const { return facet_color_ ; }
        void set_facet_color(const vec4& c) { facet_color_ = c ; }

        bool vertices_visible() const { return vertices_visible_ ; }
        void set_vertices_visible(bool x) { vertices_visible_ = x ; }

        bool edges_visible() const { return edges_visible_ ; }
        void set_edges_visible(bool x) { edges_visible_ = x ; }

        bool facets_visible() const { return facets_visible_ ; }
        void set_facets_visible(bool x) { facets_visible_ = x ; }

        bool border_visible() const { return border_visible_ ; }
        void set_border_visible(bool x) { border_visible_ = x ; }

        unsigned int border_width() const { return border_width_ ; }
        void set_border_width(unsigned int x) { border_width_ = x ; }

        const vec4& border_color() const { return border_color_ ; }
        void set_border_color(const vec4& x) { border_color_ = x ; }

        bool shrink_facets() const { return shrink_facets_ ; }
        void set_shrink_facets(bool x) { shrink_facets_ = x ; }

        bool smooth() const { return smooth_ ; }
        void set_smooth(bool x) { smooth_ = x ; }

        void set_vertex_property(const std::string& prop_name) ;
        void unset_vertex_property() { vertex_property_.unbind() ; vertex_tex_coord_.unbind() ; }
        bool vertex_property_is_visible() const { return vertex_property_.is_bound() ; }
        
        void set_facet_property(const std::string& prop_name) ;
        void unset_facet_property() { facet_property_.unbind() ;  }
        bool facet_property_is_visible() const { return facet_property_.is_bound() ; }

        void set_use_tstrips(bool x) { use_tstrips_ = x ; }
        bool use_tstrips() const { return use_tstrips_ ; }

        void set_normals_visible(bool x) { normals_visible_ = x ; }
        bool normals_visible() const { return normals_visible_ ; }

    protected:
        void set_vertex_color(const Map::Vertex* v) { 
            if(vertex_property_.is_bound()) {
                glColor(property_color_.color(vertex_property_[v])) ; 
            } else if(vertex_tex_coord_.is_bound()) {
                glTexCoord(5.0f * vertex_tex_coord_[v]) ;
            }
        }
        void set_facet_color(const Map::Facet* f) { 
            if(facet_property_.is_bound()) {
                glColor(property_color_.color(facet_property_[f])) ; 
            }
        }
        void draw_vertices() ;
        void draw_edges() ;
        void draw_facets() ;
        void draw_border() ;
        void draw_normals() ;

    private:
        Map* map_ ;
        vec4 vertex_color_ ;
        vec4 edge_color_ ;
        vec4 facet_color_ ;
        bool vertices_visible_ ;
        bool edges_visible_ ;
        bool facets_visible_ ;
        bool border_visible_ ;
        int border_width_ ;
        vec4 border_color_ ;
        bool shrink_facets_ ;
        bool smooth_ ;
        ColorRamp property_color_ ;
        MapVertexProperty<real> vertex_property_ ;
        MapVertexProperty<vec2> vertex_tex_coord_ ;
        MapFacetProperty<real> facet_property_ ;
        GLuint texture_ ;
        bool use_tstrips_ ;
        bool normals_visible_ ;
    } ;

}

#endif


