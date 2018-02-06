/*
 *  gx/Graphite: Geometry and Graphics Programming Library + Utilities
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

#ifndef __COMBINATORICS_MAP_CELL_HEAP__
#define __COMBINATORICS_MAP_CELL_HEAP__

//#include <gx/cells/common/common.h>
#include <Geex/combinatorics/map.h>
#include <Geex/combinatorics/map_properties.h>
//#include <gx/basic/Propertys/Property.h>

#include <set>

namespace Geex {

//_________________________________________________________
    
    template<class CELL>
    class MapCellHeap : public MapCombelObserver<CELL> {
    public :
        
        MapCellHeap(
            Map *s,                      // surface to iterate on
            Property<CELL,double> &cost // how to sort
        ) : MapCombelObserver<CELL>(s) {
            cost_ = cost;
        } 
    
        ~MapCellHeap() { }    
    
        // top() is not defined since top may be indirectly changed by surface
        CELL* pop(){
            gx_assert(!heap_.empty()) ;
            CELL* result = heap_[0];
            remove(heap_[0]) ; 
            return result;
        }


        bool empty()      { return heap_.empty();  }
        unsigned int size() { return heap_.size(); }

        // _________________ modify _____________________________

        // modify cost....
        void update_cost(CELL *v) {
            gx_assert(contains(v));
            place_up_in_heap(v);
            place_down_in_heap(v);
        }
        
        bool contains(CELL *v) { 
            return (v != nil && pos_[v] >= 0 && pos_[v] < int(heap_.size()) && heap_[pos_[v]] == v) ; 
        }

        void push(CELL* v) {
            gx_assert(!contains(v)) ;
            pos_[v] = int(heap_.size());
            heap_.push_back(v);
            place_up_in_heap(v);
        }
        
        // ________________ MapCombelObserver overloads ______________

        // only called from surface, does nothing.

        virtual void add(CELL* v) {  }

        virtual void remove(CELL* v) {
            if(contains(v)) {
                int pos = pos_[v]; 
                if (pos != int(heap_.size())-1) {
                    switch_elements(pos,int(heap_.size())-1);
                }
                heap_.pop_back();
                if (pos < int(heap_.size())) {
                    update_cost(heap_[pos]);
                }
            }
        }
        
    private:    
        void switch_elements(int i,int j) {
            CELL* tmp;
            tmp = heap_[i];
            heap_[i] =  heap_[j];
            heap_[j] =  tmp;
            pos_[heap_[i] ]=i;
            pos_[heap_[j] ]=j;
        }

        inline long int father(long int i) const {
            return  (i+1)/2 - 1 ;
        }
        
        inline long int child0(long int i) const {
            return 2*(i+1)-1;
        }
        
        inline long int child1(long int i) const {
            return 2*(i+1)+1-1;
        }
    
        void place_up_in_heap(CELL *v) {
            if (pos_[v]!=0 && cost_[v] < cost_[heap_[father(pos_[v])] ]) {
                switch_elements(pos_[v],father(pos_[v]));
                place_up_in_heap(v);
            }
        }

        void place_down_in_heap(CELL *v) {
            gx_assert(pos_[v] < int(heap_.size()));
            gx_assert(pos_[v] >=0 );
            if (
                (child0(pos_[v]) < int(heap_.size())
                    && cost_[heap_[child0(pos_[v])]] < cost_[v])
                || 
                (child1(pos_[v]) < int(heap_.size())
                    && cost_[heap_[child1(pos_[v])]] < cost_[v])
            ) {
                if (child1(pos_[v]) == int(heap_.size())
                    || 
                    cost_[heap_[child0(pos_[v])]] < 
                    cost_[heap_[child1(pos_[v])]]
                ) {
                    switch_elements(pos_[v],child0(pos_[v]));
                    place_down_in_heap(v);
                } else {
                    switch_elements(pos_[v],child1(pos_[v]));
                    place_down_in_heap(v);
                }
            }
        }
        
    private: // protect against copy
        
        MapCellHeap(const MapCellHeap& rhs) ;
        MapCellHeap& operator=(const MapCellHeap& rhs) ;
        
    protected :
        Property<CELL,int> pos_;      // -1 means not in heap
        Property<CELL,double> cost_;
        std::vector<CELL*> heap_;
    } ;


    //---------------------------------------------------------------------

    class MapVertexHeap : 
        public MapCellHeap<Map::Vertex> {
    public :
        MapVertexHeap(
            Map* s,
            Property<Map::Vertex,double>& cost
        ) : MapCellHeap<Map::Vertex>(s,cost){
            pos_.bind(s->vertex_property_manager());
			FOR_EACH_VERTEX(Map, map(), vi) {
                pos_[vi] = -1;
            }
        }  

        void init_with_all_surface_vertices() {
            FOR_EACH_VERTEX(Map, map(), vi) {
                push(vi);
            }
        }
    } ;

    //---------------------------------------------------------------------

    class MapHalfedgeHeap : 
        public MapCellHeap<Map::Halfedge> {
    public :
        MapHalfedgeHeap(
            Map* s,
            Property<Map::Halfedge,double>& cost
        ) : MapCellHeap<Map::Halfedge>(s,cost){
            pos_.bind(s->halfedge_property_manager());
            FOR_EACH_HALFEDGE(Map, map(), hi) {
                pos_[hi] = -1;
            }
        }  

        void init_with_all_surface_halfedges(){
            FOR_EACH_HALFEDGE(Map, map(), hi) {
                push(hi) ;
            }      
        }
    } ;

    //---------------------------------------------------------------------

    class MapFacetHeap :
        public MapCellHeap<Map::Facet> {
    public :
        MapFacetHeap(
            Map* s,
            Property<Map::Facet,double>& cost
        ) : MapCellHeap<Map::Facet>(s,cost){
            pos_.bind(s->facet_property_manager());
			FOR_EACH_FACET(Map, map(), fi) {
                pos_[fi] = -1;
            }
        }

        void init_with_all_surface_facets(){
            FOR_EACH_FACET(Map, map(), fi) {
                push(fi) ;
            }
        }
    } ;

//_________________________________________________________
}

#endif

