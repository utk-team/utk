/*
 *  Geex/Graphite: Geometry and Graphics Programming Library + Utilities
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
 

#include <Geex/combinatorics/map_components.h>
#include <Geex/combinatorics/enumerate.h> 
#include <Geex/combinatorics/map_geometry.h>
#include <set>

namespace Geex {

//_________________________________________________________

    MapComponentList MapComponentsExtractor::extract_components(
        Map* map
    ) {
        Property<Map::Vertex,int> component_id(
            map->vertex_property_manager()
        ) ;
        int nb_components = enumerate_connected_components(map, component_id) ;
        MapComponentList result ;
        for(int i=0; i<nb_components; i++) {
            result.push_back(new MapComponent(map)) ;
        }
        
        FOR_EACH_VERTEX(Map, map, it) {
            set_target(result[component_id[it]]) ;
            vertices().push_back(it) ;
        }

        FOR_EACH_HALFEDGE(Map, map, it) {
            set_target(result[component_id[it->vertex()]]) ;
            halfedges().push_back(it) ;
        }

        FOR_EACH_FACET(Map, map, it) {
            set_target(result[component_id[it->halfedge()->vertex()]]) ;
            facets().push_back(it) ;
        }

        return result ;
    }

    MapComponentList MapComponentsExtractor::extract_components(
        Map* map, MapFacetProperty<int>& component_id
    ) {
        int nb_components = 0 ;
        FOR_EACH_FACET(Map, map, it) {
            nb_components = gx_max(nb_components, component_id[it]) ;
        }
        nb_components++ ;
        MapComponentList result ;
        for(int i=0; i<nb_components; i++) {
            result.push_back(new MapComponent(map)) ;
        }
        
        FOR_EACH_VERTEX(Map, map, it) {
            Map::Halfedge* h = it->halfedge() ;
            int comp_id = (h->facet() != nil) ? component_id[h->facet()] : component_id[h->opposite()->facet()] ;
            set_target(result[comp_id]) ;
            vertices().push_back(it) ;
        }

        FOR_EACH_HALFEDGE(Map, map, it) {
            int comp_id = (it->facet() != nil) ? component_id[it->facet()] : component_id[it->opposite()->facet()] ;
            set_target(result[comp_id]) ;
            halfedges().push_back(it) ;
        }

        FOR_EACH_FACET(Map, map, it) {
            set_target(result[component_id[it]]) ;
            facets().push_back(it) ;
        }
        return result ;
    }

    MapComponent* MapComponentsExtractor::extract_component(
        Map* map, Map::Facet* from
    ) {
        Property<Map::Vertex,int> component_id(
            map->vertex_property_manager()
        ) ;
        enumerate_connected_components(map, component_id) ;
        MapComponent* result = new MapComponent(map) ;
        set_target(result) ;

        int comp_id = component_id[from->halfedge()->vertex()] ;

        {FOR_EACH_VERTEX(Map, map, it) {
            if(component_id[it] == comp_id) {
                vertices().push_back(it) ;
            }
        }}

        {FOR_EACH_HALFEDGE(Map, map, it) {
            if(component_id[it->vertex()] == comp_id) {
                halfedges().push_back(it) ;
            }
        }}

        {FOR_EACH_FACET(Map, map, it) {
            if(component_id[it->halfedge()->vertex()] == comp_id) {
                facets().push_back(it) ;
            }
        }}

        return result ;
    }

//_______ Geometry ________________________________________________________

    namespace Geom {

        double component_area(const MapComponent* comp) {
            double result = 0 ;
            FOR_EACH_FACET_CONST(MapComponent, comp, it) {
                result += facet_area(it) ;
            }
            return result ;
        }        

/*        double component_signed_area2d(const MapComponent* comp) {
            double result = 0 ;
            FOR_EACH_FACET_CONST(MapComponent, comp, it) {
                result += facet_signed_area2d(it) ;
            }
            return result ;
        } */       

        double component_border_length(const MapComponent* comp) {
            double result = 0.0 ;
            FOR_EACH_HALFEDGE_CONST(MapComponent, comp, it) {
                if(it->is_border()) {
                    result += edge_length(it) ;
                }
            }
            
            return result ;
        }


        Box3d component_bbox(const MapComponent* mapc) {
            Box3d result ;
            FOR_EACH_VERTEX_CONST(MapComponent, mapc, it) {
                result.add_point(it->point()) ;
            }
            return result ;
        }
        
        //Box2d component_bbox2d(const MapComponent* mapc) {
        //    Box2d result ;
        //    FOR_EACH_HALFEDGE_CONST(MapComponent, mapc, it) {
        //        result.add_point(it->tex_vertex()->tex_coord()) ;
        //    }
        //    return result ;
        //}

        void translate_component(MapComponent* comp, const vec3& v) {
            FOR_EACH_VERTEX(MapComponent, comp, it) {
                vec3 new_p = it->point() + v ;
                it->set_point(new_p) ;
            }
        }
        
    //    void translate_component2d(MapComponent* comp, const vec2& v) {
    //        FOR_EACH_VERTEX(MapComponent, comp, it) {
    //            vec2 new_p = it->halfedge()->tex_coord() + v ;
    //            it->halfedge()->set_tex_coord(new_p) ;
    //        }
    //    }

    //    void normalize_component_tex_coords(MapComponent* comp) {
    //        if(comp->size_of_vertices() == 0) {
    //            return ;
    //        }
    //        Box2d B = component_bbox2d(comp) ;
    //        double sx = (B.width() > 0) ? 1.0/B.width() : 0.0 ;
    //        double sy = (B.height() > 0) ? 1.0/B.height() : 0.0 ;
    //        std::set<Map::TexVertex*> tex_vertices ;
    //        FOR_EACH_HALFEDGE(MapComponent, comp, it) {
    //            tex_vertices.insert(it->tex_vertex()) ;
    //        }
    //        for(std::set<Map::TexVertex*>::iterator it = tex_vertices.begin(); it != tex_vertices.end(); it++) {
    //            Map::TexVertex* cur = *it ;
    //            double x = cur->tex_coord().x ;
    //            double y = cur->tex_coord().y ;
    //            cur->set_tex_coord(
    //                vec2(
    //                    sx * (x - B.x_min()),
    //                    sy * (y - B.y_min())
    //                )
    //            ) ;
    //        }
    //    }

    } // end of namespace Geom

} // end of namespace Geex

