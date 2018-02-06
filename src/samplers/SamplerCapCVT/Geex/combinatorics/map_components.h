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
 

#ifndef ___MAP_COMPONENTS__
#define ___MAP_COMPONENTS__

//#include <Geex/combinatorics/common/common.h>
#include <Geex/combinatorics/map.h>
#include <Geex/combinatorics/map_properties.h>
#include <Geex/basics/counted.h>
#include <Geex/basics/smart_pointer.h>
#include <Geex/basics/pointer_iterator.h>

#include <vector>

namespace Geex {

//_________________________________________________________

    /**
     * represents a connected component of a Map.
     * A MapComponent stores three lists of pointers to its
     * vertices, halfedges and facets it contains.
     */
    class GEEX_API MapComponent : public Counted {
    public:

        MapComponent(Map* map) : map_(map) { }

        typedef Map::Vertex Vertex ;
        typedef Map::Halfedge Halfedge ;
        typedef Map::Facet Facet ;
        typedef Map::TexVertex TexVertex ;

        typedef PointerIterator<std::vector<Vertex*>,Vertex> 
           Vertex_iterator ;

        typedef PointerIterator<std::vector<Halfedge*>,Halfedge> 
           Halfedge_iterator ;

        typedef PointerIterator<std::vector<Facet*>,Facet> 
           Facet_iterator ;

        typedef PointerConstIterator<std::vector<Vertex*>,Vertex>
           Vertex_const_iterator ;

        typedef PointerConstIterator<std::vector<Halfedge*>,Halfedge>
           Halfedge_const_iterator;

        typedef PointerConstIterator<std::vector<Facet*>,Facet> 
           Facet_const_iterator ;

        Vertex_iterator vertices_begin() { 
            return Vertex_iterator(vertices_.begin()); 
        }
        
        Vertex_iterator vertices_end() { 
            return Vertex_iterator(vertices_.end()); 
        }
        
        Halfedge_iterator halfedges_begin() { 
            return Halfedge_iterator(halfedges_.begin()); 
        }
        
        Halfedge_iterator halfedges_end() { 
            return Halfedge_iterator(halfedges_.end()); 
        }
        
        Facet_iterator facets_begin() { 
            return Facet_iterator(facets_.begin()); 
        }
        
        Facet_iterator facets_end() { 
            return Facet_iterator(facets_.end()); 
        }

        Vertex_const_iterator vertices_begin() const { 
            return Vertex_const_iterator(vertices_.begin()); 
        }

        Vertex_const_iterator vertices_end() const { 
            return Vertex_const_iterator(vertices_.end()); 
        }

        Halfedge_const_iterator halfedges_begin() const { 
            return Halfedge_const_iterator(halfedges_.begin()); 
        }

        Halfedge_const_iterator halfedges_end() const { 
            return Halfedge_const_iterator(halfedges_.end()); 
        }

        Facet_const_iterator facets_begin() const { 
            return Facet_const_iterator(facets_.begin()); 
        }

        Facet_const_iterator facets_end() const { 
            return Facet_const_iterator(facets_.end()); 
        }

        unsigned int size_of_vertices() const {
            return (unsigned int) vertices_.size() ;
        }
        
        unsigned int size_of_halfedges() const {
            return (unsigned int) halfedges_.size() ;
        }
        
        unsigned int size_of_facets() const {
            return (unsigned int) facets_.size() ;
        }

        Map* map() const { return map_ ; }

    protected:
        friend class MapComponentMutator ;
        std::vector<Vertex*>& vertices() { return vertices_ ; }
        std::vector<Halfedge*>& halfedges() { return halfedges_ ; }
        std::vector<Facet*>& facets() { return facets_ ; }

    private:
        Map* map_ ;
        std::vector<Vertex*> vertices_ ;
        std::vector<Halfedge*> halfedges_ ;
        std::vector<Facet*> facets_ ;
    } ;

    typedef SmartPointer<MapComponent> MapComponent_var ;
    typedef std::vector<MapComponent_var> MapComponentList ;

//_________________________________________________________

    /**
     * Base class for objects allowed to modify a MapComponent.
     * It gives direct access to the lists stored by a component.
     */
    class GEEX_API MapComponentMutator {
    public:

        typedef MapComponent::Vertex Vertex ;
        typedef MapComponent::Halfedge Halfedge ;
        typedef MapComponent::Facet Facet ;
        typedef MapComponent::TexVertex TexVertex ;


        MapComponentMutator(MapComponent* target = nil) : target_(target) { }
        ~MapComponentMutator() { target_ = nil ;  }
        MapComponent* target() { return target_ ; }
        void set_target(MapComponent* m) { target_ = m ; }

    protected:
        std::vector<Vertex*>& vertices() { return target_->vertices(); }
        std::vector<Halfedge*>& halfedges() { return target_->halfedges(); }
        std::vector<Facet*>& facets() { return target_->facets(); }

    private:
        MapComponent* target_ ;
    } ;

//_________________________________________________________

    /**
     * creates the list of the connected components of a Map.
     */
    class GEEX_API MapComponentsExtractor : public MapComponentMutator {
    public:
        MapComponentList extract_components(Map* map) ;
        MapComponentList extract_components(Map* map, MapFacetProperty<int>& component_id) ;
        MapComponent* extract_component(Map* map, Map::Facet* from) ;
    } ;

//_________________________________________________________

    namespace Geom {

        double GEEX_API component_area(const MapComponent* comp) ;

        double GEEX_API component_signed_area2d(const MapComponent* comp) ;
        inline double component_area2d(const MapComponent* comp) {
            return ::fabs(component_signed_area2d(comp)) ;
        }

        double GEEX_API component_border_length(const MapComponent* comp) ;

        Box3d GEEX_API component_bbox(const MapComponent* mapc) ;
        Box2d GEEX_API component_bbox2d(const MapComponent* mapc) ;

        void GEEX_API translate_component(MapComponent* comp, const vec3& v) ;
        void GEEX_API translate_component2d(MapComponent* comp, const vec2& v) ;

        void GEEX_API normalize_component_tex_coords(MapComponent* comp) ;

    } ;

}
#endif

