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
 
#include <Geex/combinatorics/map_io.h>
#include <Geex/combinatorics/map.h>
#include <Geex/combinatorics/map_builder.h>
#include <Geex/combinatorics/map_properties.h>
#include <Geex/properties/generic_properties_io.h>
#include <Geex/basics/line_stream.h>
#include <Geex/basics/file_system.h>

#include <gzstream/gzstream.h>

namespace Geex {

    static Map::Halfedge* find_halfedge_between(Map::Vertex* v1, Map::Vertex* v2) {
        Map::Halfedge* h = v2->halfedge() ;
        do {
            if(h->opposite()->vertex() == v1) {
                return h ;
            }
            h = h->next_around_vertex() ;
        } while(h != v2->halfedge()) ;
        return nil ;
    }


    void load_map(
        std::istream& input, Map* map, bool load_tex_coords, bool flip_coords
    ) {

        std::cerr << "EOBJ loader: Loading map..." << std::endl ;

        MapBuilder builder(map) ;

        std::vector< SerializedProperty<Map::Vertex>   > v_properties ;
        std::vector< SerializedProperty<Map::Halfedge> > h_properties ;
        std::vector< SerializedProperty<Map::Facet>    > f_properties ;

        std::vector<Map::Facet*> facets ;
        
        bool surface_terminated = false ;
        
        LineInputStream in(input) ;

        builder.begin_surface() ;
        while(!in.eof()) {
            in.get_line() ;
            std::string keyword ;

            in >> keyword ;
            
            if(keyword == "v") {
                vec3 p ;
                in >> p ;
                if(flip_coords) {
                    builder.add_vertex(vec3(p.x, p.z, p.y)) ;
                } else {
                    builder.add_vertex(vec3(p)) ;		    
                }
            } else if(keyword == "vt") {
                if(load_tex_coords) {
                    vec2 q ;
                    in >> q ;
                    builder.add_tex_vertex(q) ;
                }
            } else if(keyword == "f") {
                builder.begin_facet() ;
                while(!in.eol()) {
                    std::string s ;
                    in >> s ;
                    if(s.length() > 0) {
                        std::istringstream v_input(s) ;
                        int index ;
                        v_input >> index ;
                        builder.add_vertex_to_facet(index - 1) ;
                        char c ;
                        v_input >> c ;
                        if(c == '/') {
                            v_input >> index ;
                            if(load_tex_coords) {
                                builder.set_corner_tex_vertex(index - 1) ;
                            }
                        }
                    }
                }
                builder.end_facet() ;
                facets.push_back(builder.current_facet()) ;
            } else if(keyword == "#") {
                std::string second_keyword ;
                in >> second_keyword ;

                if(second_keyword == "attribute") {
                    if(!surface_terminated) {
                        // Quick and dirty fix to ensure that
                        // border edges exist before we put
                        // properties on them !!!
                        builder.terminate_surface() ;
                        surface_terminated = true ;
                    }
                    std::string name ;
                    std::string localisation ;
                    std::string type ;
                    in >> name >> localisation >> type ;
                    std::cerr << "Property " << name << " on " << localisation << " : " << type << std::endl ;
                    if(localisation == "vertex") {
                        v_properties.push_back(SerializedProperty<Map::Vertex>()) ;
                        v_properties.rbegin()->bind(map->vertex_property_manager(), name, type) ;
                    } else if(localisation == "halfedge") {
                        h_properties.push_back(SerializedProperty<Map::Halfedge>()) ;
                        h_properties.rbegin()->bind(map->halfedge_property_manager(), name, type) ;
                    } else if(localisation == "facet") {
                        f_properties.push_back(SerializedProperty<Map::Facet>()) ;
                        f_properties.rbegin()->bind(map->facet_property_manager(), name, type) ;
                    } else {
                        std::cerr << "Invalid property localisation:" 
                                  << localisation << std::endl ; 
                    }
                } else if(second_keyword == "attrs") {
                    
                    std::string localisation ;
                    in >> localisation ;
                    if(localisation == "v") {
                        int id ;
                        in >> id ;
                        id-- ;
                        Map::Vertex* v = builder.vertex(id) ;
                        serialize_read_properties(in.line(), v, v_properties) ;
                    } else if(localisation == "h") {
                        int id1, id2 ;
                        in >> id1 >> id2 ;
                        id1-- ; id2-- ;
                        Map::Vertex* v1 = builder.vertex(id1) ;
                        Map::Vertex* v2 = builder.vertex(id2) ;
                        Map::Halfedge* h = find_halfedge_between(v1,v2) ;
                        if(h == nil) {
                            std::cerr << "Halfedge does not exist" << std::endl ;
                        } else {
                            serialize_read_properties(in.line(), h, h_properties) ;
                        }
                    } else if(localisation == "f") {
                        int id ;
                        in >> id ;
                        id-- ;
                        Map::Facet* f = facets[id] ;
                        serialize_read_properties(in.line(), f, f_properties) ;
                    } else {
                        std::cerr << "Invalid property localisation:" 
                                  << localisation << std::endl ; 
                    }
                } else if(second_keyword == "anchor") {
                    int index ;
                    in >> index ;
                    builder.lock_vertex(index - 1) ;
                } else if(second_keyword == "END") {
                    break ;
                }
            } 
        }
        
        builder.end_surface() ;
    }

    // Note: we cannot use MapSerializer_obj::serialize_write,
    // since it saves anchors using special keywords. In our
    // case, anchors are handled by the generic mechanism.

    void save_map(
        std::ostream& output, Map* map, bool save_tex_coords
    ) {
        Property<Map::Vertex,int> vertex_id(
            map->vertex_property_manager()
        ) ;
        MapVertexLock is_locked(map) ;
        
        // Obj files numbering starts with 1 (instead of 0)
        // Output Vertices
        int cur_id = 1 ;
        FOR_EACH_VERTEX(Map, map, it) {
            output << "v " << it->point() << std::endl ;
            vertex_id[it] = cur_id ;
            cur_id++ ;
        } 

        // Output facets
        FOR_EACH_FACET(Map, map, it) {
            Map::Halfedge* jt = it->halfedge() ;
            output << "f " ;
            do {
                output << vertex_id[jt-> vertex()]
                       << " " ;
                jt = jt->next() ;
            } while(jt != it->halfedge()) ;
            output << std::endl ;
        }


        // Output vertex properties
        {
            std::vector<SerializedProperty<Map::Vertex> > properties ;
            if(get_serializable_properties(map->vertex_property_manager(), properties, output, "vertex")) {
                int vid = 1 ;
	        FOR_EACH_VERTEX(Map, map, it) {
                    Map::Vertex* v = it ;
                    output << "# attrs v " << vid << " " ;
                    serialize_write_properties(output, v, properties) ;
                    output << std::endl ;
                    vid++ ;
                }
            }
        }

        // Output halfedge properties
        {
            std::vector<SerializedProperty<Map::Halfedge> > properties ;
            if(get_serializable_properties(map->halfedge_property_manager(), properties, output, "halfedge")) {
	        FOR_EACH_HALFEDGE(Map, map, it) {
                    Map::Halfedge* h = it ;
                    output << "# attrs h " 
                           << vertex_id[h->opposite()->vertex()] << " " << vertex_id[h->vertex()] << " " ;
                    serialize_write_properties(output, h, properties) ;
                    output << std::endl ;
                }
            }
        }

        // Output facet properties
        {
            std::vector<SerializedProperty<Map::Facet> > properties ;
            if(get_serializable_properties(map->facet_property_manager(), properties, output, "facet")) {
                int fid = 1 ;
	        FOR_EACH_FACET(Map, map, it) {
                    Map::Facet* f = it ;
                    output << "# attrs f " << fid << " " ;
                    serialize_write_properties(output, f, properties) ;
                    output << std::endl ;
                    fid++ ;
                }
            }
        }
        output << "# END" << std::endl ;
    }


    static void load_compressed_map(const std::string& file_name_in, Map* map, bool load_tex_coords, bool flip) {
        std::string extension = FileSystem::extension(file_name_in) ;
        gx_assert(extension == "gz") ;
        std::string file_name = 
            FileSystem::dir_name(file_name_in) + "/" +
            FileSystem::base_name(file_name_in) ;
        extension = FileSystem::extension(file_name) ;

        if(extension != "obj" && extension != "eobj") {
            std::cerr << "Invalid file extension:" << extension << std::endl ;
            return ;
        }
        
        igzstream in(file_name_in.c_str()) ;
        if(!in) {
            std::cerr << "could not open file \'"
                      << file_name_in 
                      << "\'" << std::endl ;
            return ;
        }
        load_map(in, map, load_tex_coords, flip) ;
    }


    void load_map(const std::string& filename, Map* map, bool load_tex_coords, bool flip) {

        if(!FileSystem::is_file(filename)) {
            std::cerr << "Cannot open file: " << filename << std::endl ;
        }

        std::string extension = FileSystem::extension(filename) ;
        if(extension == "gz") {
            load_compressed_map(filename, map, load_tex_coords, flip) ;
        } else if(extension == "obj" || extension == "eobj") {
            std::ifstream in(filename.c_str()) ;
            if(!in) {
                std::cerr << "could not open file \'"
                          << filename
                          << "\'" << std::endl ;
                return ;
            }
            load_map(in, map, load_tex_coords, flip) ;
        }
    }


	void GEEX_API save_map(const std::string& filename, Map* map, bool save_tex_coords) {
		std::ofstream out(filename.c_str()) ;
		save_map(out, map, save_tex_coords) ;
		out.close() ;
	}
    
}
