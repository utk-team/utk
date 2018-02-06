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

#ifndef __GEEX_COMBINATORICS_MAP__
#define __GEEX_COMBINATORICS_MAP__

#include <Geex/combinatorics/map_combels.h>
#include <Geex/combinatorics/iterators.h>
#include <Geex/basics/dlist.h>
#include <Geex/properties/property.h>
#include <vector>

namespace Geex {
    
//_________________________________________________________

    class MapMutator ;


    class Map ;

    /**
     * Has its members add() and remove() called
     * whenever a vertex is added (respectively
     * removed) from the structure. This class
     * is meant to be subclassed by client code.
     */
    template <class CELL> class MapCombelObserver {
    public:
        MapCombelObserver(Map* m) : map_(m) { }
        virtual ~MapCombelObserver() { }
        virtual void add(CELL* c) { }
        virtual void remove(CELL* c) { }

    protected:
        Map* map() { return map_; }
        
    private:
        Map* map_ ;
    } ;


    /**
     * Map implements a topological map. It is highly inspired 
     * by CGAL's Polyhedron class, with the difference that
     * each halfedge is allocated separately. All the elements
     * are allocated in a high-performance chunk-based allocator.
     * Map is nearly source-level compatible with CGAL polyhedron.
     * The operations modifying the Map are implemented in the
     * MapEditor class.
     */

    class GEEX_API Map {
    public:

        // __________________ types ___________________

        typedef MapCombels::Vertex Vertex ;
        typedef MapCombels::Halfedge Halfedge ;
        typedef MapCombels::Facet Facet ;
        typedef MapCombels::TexVertex TexVertex ;
        typedef MapCombels::TexVertex_var TexVertex_var ;

        typedef DList<Vertex>::iterator Vertex_iterator ;
        typedef DList<Halfedge>::iterator Halfedge_iterator ;
        typedef DList<Facet>::iterator Facet_iterator ;

        typedef DList<Vertex>::const_iterator Vertex_const_iterator ;
        typedef DList<Halfedge>::const_iterator Halfedge_const_iterator ;
        typedef DList<Facet>::const_iterator Facet_const_iterator ;

        typedef GenericPropertyManager<Vertex> VertexPropertyManager ;
        typedef GenericPropertyManager<Halfedge> HalfedgePropertyManager ;
        typedef GenericPropertyManager<Facet> FacetPropertyManager ;
		typedef GenericPropertyManager<TexVertex> TexVertexPropertyManager ;

        Map() ;
        virtual ~Map() ;

        // __________________ access ___________________

        // __________________ access ___________________

        Vertex_iterator vertices_begin()    { return vertices_.begin() ;  }
        Vertex_iterator vertices_end()      { return vertices_.end() ;    }
        Halfedge_iterator halfedges_begin() { return halfedges_.begin() ; }
        Halfedge_iterator halfedges_end()   { return halfedges_.end() ;   }
        Facet_iterator facets_begin()       { return facets_.begin() ;    }
        Facet_iterator facets_end()         { return facets_.end() ;      } 

        Vertex_const_iterator vertices_begin() const { 
            return vertices_.begin() ;  
        }

        Vertex_const_iterator vertices_end() const { 
            return vertices_.end() ;    
        }
        
        Halfedge_const_iterator halfedges_begin() const { 
            return halfedges_.begin() ; 
        }
        Halfedge_const_iterator halfedges_end() const { 
            return halfedges_.end() ;   
        }
        Facet_const_iterator facets_begin() const { 
            return facets_.begin() ;    
        }
        Facet_const_iterator facets_end() const { 
            return facets_.end() ;      
        } 

        size_t nb_vertices() const  { return vertices_.size() ;  }
        size_t nb_halfedges() const { return halfedges_.size() ; }
        size_t nb_facets() const    { return facets_.size() ;    }

        // ___________________ properties _______________________
        
        VertexPropertyManager* vertex_property_manager() const {
            return const_cast<VertexPropertyManager*>(
                &vertex_property_manager_ 
            ) ;
        }

        HalfedgePropertyManager* halfedge_property_manager() const {
            return const_cast<HalfedgePropertyManager*>(
                &halfedge_property_manager_ 
            ) ;
        }
        
        FacetPropertyManager* facet_property_manager() const {
            return const_cast<FacetPropertyManager*>(
                &facet_property_manager_ 
            ) ;
        }         
        
		TexVertexPropertyManager* tex_vertex_property_manager() const {
			return const_cast<TexVertexPropertyManager*>(
				&tex_vertex_property_manager_ 
				) ;
		}

        // ___________________ observers ________________________

        void add_vertex_observer(MapCombelObserver<Vertex>* obs) ;
        void remove_vertex_observer(MapCombelObserver<Vertex>* obs) ;
        void add_halfedge_observer(MapCombelObserver<Halfedge>* obs) ;
        void remove_halfedge_observer(MapCombelObserver<Halfedge>* obs) ;
        void add_facet_observer(MapCombelObserver<Facet>* obs) ;
        void remove_facet_observer(MapCombelObserver<Facet>* obs) ;

        // __________________ modification ______________________

        void clear() ;
        void erase_all() ;
        void clear_inactive_items() ;

        // __________________ predicates ________________________

        bool is_triangulated() const ;

        // __________________ low level (for experts only) ______

        /**
         * checks the validity of the combinatorial structure.
         */
        bool is_valid() const ;

        /**
         * checks the validity of the combinatorial structure,
         * and stops in an assertion failure if an error is detected.
         * This function can be used to debug low-level operations,
         * such as those in the MapBuilder and the MapEditor classes.
         */
        void assert_is_valid() const ;

        /**
         * fills the list l with all the vertices connected to v,
         * directly or indirectly.
         */
        void get_connected_component(
            Vertex* v, std::vector<Vertex*>& l
        ) ;

    protected:

        /**
         * allocates a pair of halfedges connected with next(), prev(),
         * and opposite() links.
         */
        Halfedge* new_edge() ;

        /**
         * deallocates a pair of halfedges, connected with an opposite() link.
         */
        void delete_edge(Halfedge* h) ;

        Vertex* new_vertex() ;
		TexVertex* new_tex_vertex() ;
        Halfedge* new_halfedge() ;
        Facet* new_facet() ;

        /** copies geometry and properties from rhs. */
        Vertex* new_vertex(const Vertex* rhs) ;
		/** copies geometry and attributes from rhs. */
		TexVertex* new_tex_vertex(const TexVertex* rhs) ;        
        /** copies properties from rhs. */
        Halfedge* new_halfedge(const Halfedge* rhs) ;
        /** copies properties from rhs. */
        Facet* new_facet(const Facet* rhs) ;

        void delete_vertex(Vertex* v) ;
        void delete_halfedge(Halfedge* h) ;
        void delete_facet(Facet* f) ;
		void delete_tex_vertex(TexVertex* tv) ;

        void activate_vertex(Vertex* v) ;
        void activate_halfedge(Halfedge* h) ;
        void activate_facet(Facet* f) ;
        
        void deactivate_vertex(Vertex* v) ;
        void deactivate_halfedge(Halfedge* h) ;
        void deactivate_facet(Facet* f) ;

        
        friend class ::Geex::MapMutator ;
        
    protected:
        void reorient_facet( Halfedge* first) ;
        

    protected:

        // MapCombelObservers notification
        void notify_add_vertex(Vertex* v) ;
        void notify_remove_vertex(Vertex* v) ;
        void notify_add_halfedge(Halfedge* h) ;
        void notify_remove_halfedge(Halfedge* h) ;
        void notify_add_facet(Facet* f) ;
        void notify_remove_facet(Facet* f) ;

    private:
        DList<Vertex> vertices_ ;
        DList<Halfedge> halfedges_ ;
        DList<Facet> facets_ ;

        VertexPropertyManager vertex_property_manager_ ;
        HalfedgePropertyManager halfedge_property_manager_ ;
        FacetPropertyManager facet_property_manager_ ;
		TexVertexPropertyManager tex_vertex_property_manager_ ;

        std::vector< MapCombelObserver<Vertex>* > vertex_observers_ ;
        std::vector< MapCombelObserver<Halfedge>* > halfedge_observers_ ;
        std::vector< MapCombelObserver<Facet>* > facet_observers_ ;

        friend class MapVerticesItr ;
        friend class MapHalfedgesItr ;
        friend class MapFacetsItr ;
    } ;

    //______________________________________________________________

    /**
     * MapMutator is the base class for the classes
     * that can modify the topology of a map.
     */
    
    class GEEX_API MapMutator {
    public:

        // _________________ Types ____________________

        typedef Map::Vertex Vertex ;
		typedef Map::TexVertex TexVertex ; 
        typedef Map::Halfedge Halfedge ;
        typedef Map::Facet Facet ;

        typedef Map::Vertex_iterator Vertex_iterator ;
        typedef Map::Halfedge_iterator Halfedge_iterator ;
        typedef Map::Facet_iterator Facet_iterator ;

        typedef Map::Vertex_const_iterator Vertex_const_iterator ;
        typedef Map::Halfedge_const_iterator Halfedge_const_iterator ;
        typedef Map::Facet_const_iterator Facet_const_iterator ;

        MapMutator(Map* target = nil) : target_(target) { }
        virtual ~MapMutator() ; 
        Map* target() { return target_ ; }
        virtual void set_target(Map* m) ; 
        
    protected:
        
        void link(Halfedge* h1, Halfedge* h2, int dim) ;
        void set_vertex_on_orbit(Halfedge* h, Vertex* v) ;
		void set_tex_vertex_on_orbit(Halfedge* h, TexVertex* tv) ;
        void set_facet_on_orbit(Halfedge* h, Facet* f) ;
        
        void make_vertex_key(Halfedge* h) { 
            h-> vertex()-> set_halfedge(h) ; 
        }

        void make_vertex_key(Halfedge* h, Vertex* v) { 
            v-> set_halfedge(h) ;
            h-> set_vertex(v) ;
        }

        void make_facet_key(Halfedge* h) {
            h-> facet()-> set_halfedge(h) ;
        }

        void make_facet_key(Halfedge* h, Facet* f) {
            f-> set_halfedge(h) ;
            h-> set_facet(f) ;
        }
        
        // _________ create / destroy (see Map) _________
        
        Halfedge* new_edge() { return target_-> new_edge() ; }
        void delete_edge(Halfedge* h) { target_-> delete_edge(h) ; }

        Vertex*    new_vertex()     { return target_-> new_vertex() ;     }
		TexVertex* new_tex_vertex() { return target_-> new_tex_vertex() ; }
        Halfedge*  new_halfedge()   { return target_-> new_halfedge() ;   }
        Facet*     new_facet()      { return target_-> new_facet() ;      }

        Vertex* new_vertex(const Map::Vertex* rhs) { 
            return target_-> new_vertex(rhs) ;     
        }
		TexVertex* new_tex_vertex(const Map::TexVertex* rhs) { 
			return target_-> new_tex_vertex(rhs) ; 
		}
        Halfedge* new_halfedge(const Map::Halfedge* rhs) { 
            return target_-> new_halfedge(rhs) ;
        }
        Halfedge* new_edge(Map::Halfedge* rhs) ;
        Facet* new_facet(const Map::Facet* rhs) { 
            return target_-> new_facet(rhs) ;      
        }


        void delete_vertex(Vertex* v) { target_-> delete_vertex(v) ; }
		void delete_tex_vertex(TexVertex* tv) { 
			target_-> delete_tex_vertex(tv) ;
		}
        void delete_halfedge(Halfedge* h) { 
            target_-> delete_halfedge(h) ; 
        }
        void delete_facet(Facet* f) { target_-> delete_facet(f) ; }

        // _________ activate / deactivate (see Map) _________

        void activate_vertex(Vertex* v) { target_->activate_vertex(v); }
        void activate_halfedge(Halfedge* h) { target_->activate_halfedge(h); }
        void activate_facet(Facet* f) { target_->activate_facet(f); }

        void deactivate_vertex(Vertex* v) { 
            target_->deactivate_vertex(v); 
        }
        void deactivate_halfedge(Halfedge* h) { 
            target_->deactivate_halfedge(h); 
        }
        void deactivate_facet(Facet* f) { 
            target_->deactivate_facet(f); 
        }

        // _________ basic functions ____________________
        
        void set_vertex_halfedge(Vertex* v, Halfedge* h) { 
            v-> set_halfedge(h) ; 
        }
        
        void set_halfedge_opposite(Halfedge* h1, Halfedge* h2) { 
            h1-> set_opposite(h2) ;
        }
        
        void set_halfedge_next(Halfedge* h1, Halfedge* h2) { 
            h1-> set_next(h2) ;
        }
        
        void set_halfedge_prev(Halfedge* h1, Halfedge* h2) { 
            h1-> set_prev(h2) ;
        }
        
        void set_halfedge_facet(Halfedge* h, Facet* f) { 
            h-> set_facet(f) ;
        }
        
        void set_halfedge_vertex(Halfedge* h, Vertex* v) { 
            h-> set_vertex(v) ;
        }

        void set_facet_halfedge(Facet* f, Halfedge* h) { 
            f-> set_halfedge(h) ;
        }

		void set_halfedge_tex_vertex(Halfedge* h, TexVertex* t) {
			h-> set_tex_vertex(t) ;
		}
        // ________ Access _________________________________

        DList<Vertex>& vertices() { return target_->vertices_ ; }
        DList<Halfedge>& halfedges() { return target_->halfedges_ ; }
        DList<Facet>& facets() { return target_->facets_ ; }

    private:
        Map* target_ ;
    } ;

//_________________________________________________________

    /**
     * The constructor and destructor of this
     * specialization call map->add_vertex_observer(this)
     * and map->remove_vertex_observer(this) respectively.
     */
    template<> class GEEX_API MapCombelObserver<Map::Vertex> {
    public:
        MapCombelObserver(Map* m) ;
        virtual ~MapCombelObserver() ;
        virtual void add(Map::Vertex* c) { }
        virtual void remove(Map::Vertex* c) { }
        
    protected:
        Map* map() { return map_; }
        
    private:
        Map* map_ ;
    } ;
    
    /**
     * The constructor and destructor of this
     * specialization call map->add_halfedge_observer(this)
     * and map->remove_halfedge_observer(this) respectively.
     */
    template<> class GEEX_API MapCombelObserver<Map::Halfedge> {
    public:
        MapCombelObserver(Map* m) ;
        virtual ~MapCombelObserver() ;
        virtual void add(Map::Halfedge* c) { }
        virtual void remove(Map::Halfedge* c) { }
        
    protected:
        Map* map() { return map_; }
        
    private:
        Map* map_ ;
    } ;


    /**
     * The constructor and destructor of this
     * specialization call map->add_facet_observer(this)
     * and map->remove_facet_observer(this) respectively.
     */
    template<> class GEEX_API MapCombelObserver<Map::Facet> {
    public:
        MapCombelObserver(Map* m) ;
        virtual ~MapCombelObserver() ;
        virtual void add(Map::Facet* c) { }
        virtual void remove(Map::Facet* c) { }
        
    protected:
        Map* map() { return map_; }
        
    private:
        Map* map_ ;
    } ;

//_________________________________________________________

}
#endif

