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
 
#include <Geex/graphics/map_graphics.h>
#include <Geex/combinatorics/map_geometry.h>
#include <Geex/combinatorics/map_tstrip.h>

namespace Geex {

    MapGraphics::MapGraphics(
        Map* map
    ) : map_(map), vertex_color_(1.0f,1.0f,1.0f,1.0f), edge_color_(0.1f,0.1f,0.2f,1.0f), 
        facet_color_(1.0f,1.0f,1.0f,1.0f),
        vertices_visible_(false), edges_visible_(true), facets_visible_(true), 
        border_visible_(false), border_width_(4), border_color_(0.0f, 0.0f, 0.0f,1.0f),
        shrink_facets_(false), smooth_(true), texture_(0),
        use_tstrips_(false), normals_visible_(false) {
    }

    void MapGraphics::set_map(Map* map) {
        if(vertex_property_.is_bound()) {
            vertex_property_.unbind() ;
        }
        map_ = map ;
    }

    void MapGraphics::recompute_clip_values() {
        if(vertex_property_.is_bound()) {
            property_color_.begin_range() ;
	    FOR_EACH_VERTEX(Map, map_, it) {
                property_color_.add_value_to_range(vertex_property_[it]) ;
            }
            property_color_.end_range() ;
        } else if(facet_property_.is_bound()) {
            property_color_.begin_range() ;
	    FOR_EACH_FACET(Map, map_, it) {
                property_color_.add_value_to_range(facet_property_[it]) ;
            }
            property_color_.end_range() ;
        }
    }
    
    void MapGraphics::set_vertex_property(const std::string& prop_name) {
        vertex_property_.unbind() ;
        vertex_tex_coord_.unbind() ;
        facet_property_.unbind() ;
        if(vertex_property_.is_defined(map_, prop_name)) {
            vertex_property_.bind(map_, prop_name) ;
            recompute_clip_values() ;
        } else if(vertex_tex_coord_.is_defined(map_, prop_name)) {
            vertex_tex_coord_.bind(map_, prop_name) ;
        }
    }

    void MapGraphics::set_facet_property(const std::string& prop_name) {
        vertex_property_.unbind() ;
        vertex_tex_coord_.unbind() ;
        facet_property_.unbind() ;
        if(facet_property_.is_defined(map_, prop_name)) {
            facet_property_.bind(map_, prop_name) ;
            recompute_clip_values() ;
        } 
    }

    void MapGraphics::draw() {
        if(vertices_visible_) {
            draw_vertices() ;
        }
        if(edges_visible_) {
            draw_edges() ;
        }
        if(facets_visible_) {
            draw_facets() ;
        }
        if(border_visible_) {
            draw_border() ;
        }
        if(normals_visible_) {
            draw_normals() ;
        }
    }

    void MapGraphics::draw_vertices() {
        glDisable(GL_LIGHTING) ;
        glColor(vertex_color_) ;
        glPointSize(10) ;
        begin_spheres() ;
        FOR_EACH_VERTEX(Map, map_, it) {
            set_vertex_color(it) ;
            glVertex(it->point()) ;
        }
        end_spheres() ;
    }
    
    void MapGraphics::draw_edges() {
        glDisable(GL_LIGHTING) ;
        glColor(edge_color_) ;
        glLineWidth(1) ;
        glBegin(GL_LINES) ;
        FOR_EACH_HALFEDGE(Map, map_, it) {
            if(it->is_edge_key()){
                glVertex(it->vertex()->point()) ;
                glVertex(it->opposite()->vertex()->point()) ;
            }
        }
        glEnd() ;
    }
    
    void MapGraphics::draw_normals() {
        glDisable(GL_LIGHTING) ;
        glColor(edge_color_) ;
        glLineWidth(1) ;
        glBegin(GL_LINES) ;
        FOR_EACH_VERTEX(Map, map_, it) {
            glVertex(it->point()) ;
            glVertex(it->point() + Geom::vertex_normal(it)) ;
        }
        glEnd() ;
    }

    void MapGraphics::draw_border() {
        glDisable(GL_LIGHTING) ;
        glColor(border_color_) ;
        glLineWidth(GLfloat(border_width_)) ;
        glBegin(GL_LINES) ;
        FOR_EACH_HALFEDGE(Map, map_, it) {
            if(it->facet() == nil){
                glVertex(it->vertex()->point()) ;
                glVertex(it->opposite()->vertex()->point()) ;
            }
        }
        glEnd() ;
    }


static char const * checkerboard_xpm[] = {
"32 32 2 1",
" 	c #000000000000",
".	c #FFFFFFFFFFFF",
"        ................        ",
"        ................        ",
"        ................        ",
"        ................        ",
"        ................        ",
"        ................        ",
"        ................        ",
"        ................        ",
"........                ........",
"........                ........",
"........                ........",
"........                ........",
"........                ........",
"........                ........",
"........                ........",
"........                ........",
"........                ........",
"........                ........",
"........                ........",
"........                ........",
"........                ........",
"........                ........",
"........                ........",
"........                ........",
"        ................        ",
"        ................        ",
"        ................        ",
"        ................        ",
"        ................        ",
"        ................        ",
"        ................        ",
"        ................        "};

    void MapGraphics::draw_facets() {

        if(vertex_tex_coord_.is_bound()) {
            if(texture_ == 0) {
                texture_ = create_texture_from_xpm_data(checkerboard_xpm) ;
            }
            glEnable(GL_TEXTURE_2D) ;
            glBindTexture(GL_TEXTURE_2D, texture_) ;
        }

        glEnable(GL_LIGHTING) ;
        glEnable(GL_COLOR_MATERIAL) ;
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE) ; 

        glColor(facet_color_) ;

        if(use_tstrips_ && map_->is_triangulated()) {
            MapTStripGenerator gen(map_) ;
            MapTStripSet tstrips ;
            gen.generate_tstrips(tstrips) ;
            std::cerr << "generated " << tstrips.size() << " tstrips" << std::endl ;
            std::cerr << "avg tstrip len = " << gen.average_tstrip_length() << std::endl ;
            for(size_t i=0; i<tstrips.size(); i++) {
                MapTStrip tstrip = tstrips[i] ;
                glBegin(GL_TRIANGLE_STRIP) ;
                const Map::Halfedge* h = tstrip.start ;
                 bool even = tstrip.even ;
                 if(even) {
                     glNormal(Geom::vertex_normal(h->prev()->vertex())) ;
                     glVertex(h->prev()->vertex()->point()) ;
                     glNormal(Geom::vertex_normal(h->next()->vertex())) ;
                     glVertex(h->next()->vertex()->point()) ;
                 } else {
                     glNormal(Geom::vertex_normal(h->next()->vertex())) ;
                     glVertex(h->next()->vertex()->point()) ;
                     glNormal(Geom::vertex_normal(h->prev()->vertex())) ;
                     glVertex(h->prev()->vertex()->point()) ;
                 }
                 while(h != tstrip.finish) {
                     glNormal(Geom::vertex_normal(h->vertex())) ;
                     glVertex(h->vertex()->point()) ;
                     if(even) {
                         h = h->next()->opposite()->next() ;
                     } else {
                         h = h->opposite()->next() ;
                     }
                     even = !even ;
                 } 
                 glNormal(Geom::vertex_normal(h->vertex())) ;
                 glVertex(h->vertex()->point()) ;
                 glEnd() ;
            }
        } else {
	    FOR_EACH_FACET(Map, map_, it) {
                if(!smooth_) {
                    glNormal(Geom::facet_normal(it)) ;
                }
                const Map::Facet* f = it ;
                const Map::Halfedge* h = f->halfedge() ;
                set_facet_color(f) ;
                if(shrink_facets_) {
                    vec3 g = Geom::facet_vertices_barycenter(f) ;
                    glBegin(GL_POLYGON) ;
                    do {
                        if(smooth_) {
                            glNormal(Geom::vertex_normal(h->vertex())) ;
                        }
                        set_vertex_color(h->vertex()) ;
                        const vec3& p = h->vertex()->point() ;
                        glVertex(g + 0.6f * (p - g)) ;
                        h = h->next() ;
                    } while(h != f->halfedge()) ;
                    glEnd() ;
                } else {
                    glBegin(GL_POLYGON) ;
                    do {
                        if(smooth_) {
                            glNormal(Geom::vertex_normal(h->vertex())) ;
                        }
                        set_vertex_color(h->vertex()) ;
                        glVertex(h->vertex()->point()) ;
                        h = h->next() ;
                    } while(h != f->halfedge()) ;
                    glEnd() ;
                }
            }
        }
        glDisable(GL_TEXTURE_2D) ;        
    }

}
