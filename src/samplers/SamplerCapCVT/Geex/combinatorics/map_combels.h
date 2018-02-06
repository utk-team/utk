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

#ifndef __GEEX_COMBINATORICS_MAP_COMBELS__
#define __GEEX_COMBINATORICS_MAP_COMBELS__

#include <Geex/basics/types.h>
#include <Geex/basics/counted.h>
#include <Geex/basics/smart_pointer.h>
#include <Geex/mathematics/geometry.h>
#include <Geex/properties/record_id.h>
#include <Geex/properties/property_manager.h>
#include <string.h>

// Turns on assertion checks and
//  clearing destructors.

#define MAP_DEBUG

namespace Geex {

//_________________________________________________________

    class Map ;
    class MapMutator ;

    namespace MapCombels {

        class Vertex ;
        class Halfedge ;
        class Facet ;
        class TexVertex ;

        class MapMutator ;
        

        //___________________________________________________

        /**
         * Combinatorial Element. Combel is the base class for 
         * vertices, half-edges and polygons.
         */
        class GEEX_API Combel : public Record {
        public:
            Combel()  { }
            ~Combel() { }
        } ;

        //___________________________________________________

        /**
         * Texture coordinates can be shared between several
         * corners, or independant.
         */

        // Note: for the moment, each tex vertex keeps
        // a pointer to the tex vertex attribute manager,
        // to enable a correct management of the attributes.
        // This will be improved in the future.

        class GEEX_API TexVertex : public Counted, public Combel {
        public:
            TexVertex(GenericPropertyManager<TexVertex>* attr_mngr) :
                property_manager_(attr_mngr) { 
                property_manager_->new_record(this) ;
            }

            TexVertex(
                GenericPropertyManager<TexVertex>* attr_mngr, const TexVertex* rhs
            ) : tex_coord_(rhs->tex_coord()), property_manager_(attr_mngr) { 
                property_manager_->new_record(this, rhs) ;
            }

            ~TexVertex() { 
                property_manager_->delete_record(this) ;
                property_manager_ = nil ;
            }
            const vec2& tex_coord() const  { return tex_coord_ ; }
            vec2& tex_coord()              { return tex_coord_ ; }
            void set_tex_coord(const vec2& p) { tex_coord_ = p ; }

        private:
            vec2 tex_coord_ ;
            GenericPropertyManager<TexVertex>* property_manager_ ;
        } ;

        typedef SmartPointer<TexVertex> TexVertex_var ;

		//_________________________________________________________
        
		/**
         * A vertex of a Map. Each Vertex has a geometry (i.e. a vec3).
         */
        class GEEX_API Vertex : public Combel {
        public:
            Vertex() : halfedge_(nil) {  }
            Vertex(const vec3& p) : halfedge_(nil), point_(p) {  }
            ~Vertex() { halfedge_ = nil ; }
            
            const vec3& point() const     { return point_ ; }
            vec3& point()                 { return point_ ; }
            void set_point(const vec3& p) { point_ = p ;    }

            Halfedge* halfedge() const { return halfedge_ ; }

            void assert_is_valid() const ;

            size_t degree() const ;
            bool is_on_border() const ;

        protected:
            void set_halfedge(Halfedge* h) { halfedge_ = h ; }
            friend class ::Geex::Map ;
            friend class ::Geex::MapMutator ;

        private:
            Halfedge* halfedge_ ;
            vec3 point_ ;
        } ;

        //______________________________________________

        /**
         * Each edge of a Map is composed of two Halfedges.
         */
        class GEEX_API Halfedge : public Combel {
        public:
            Halfedge() : 
                opposite_(nil), next_(nil), 
                prev_(nil), facet_(nil), vertex_(nil) {
            }
            ~Halfedge() { 
                opposite_ = nil ; next_ = nil ;
                prev_ = nil ; facet_ = nil ; vertex_ = nil ;
            }

			TexVertex* tex_vertex() const { return tex_vertex_ ; }
			
			const vec2& tex_coord() const { 
				return tex_vertex()-> tex_coord() ; 
			}

			vec2& tex_coord() { 
				return tex_vertex()-> tex_coord() ; 
			}

			void set_tex_coord(const vec2& tex_coord_in) {
				tex_vertex()-> set_tex_coord(tex_coord_in) ; 
			}

            Halfedge* opposite() const { return opposite_ ; }
            Halfedge* next() const { return next_ ; }
            Halfedge* prev() const { return prev_ ; }
            Halfedge* next_around_vertex() const {
                return opposite()->prev() ;
            }
            Halfedge* prev_around_vertex() const {
                return next()->opposite() ;
            }

            Facet* facet() const { return facet_ ; }
            Vertex* vertex() const { return vertex_ ; }
            bool is_border() const { return facet_ == nil ; }
            bool is_border_edge() const { 
                return is_border() || opposite()-> is_border() ; 
            }

            /** One halfedge per facet exacly is the facet key. */
            bool is_facet_key() const ;

            /** One halfedge per vertex exacly is the vertex key. */
            bool is_vertex_key() const ;

            /** 
             * One halfedge per edge exacly is the edge key. 
             * Note: this can be used for loops, to traverse
             * one halfedge per edge exaclty (for instance, to
             * draw the mesh).
             */
            bool is_edge_key() const ;
            Halfedge* edge_key() const { 
                return is_edge_key() ? const_cast<Halfedge*>(this) : opposite() ; 
            }

            void assert_is_valid() const ;

        protected:
            void set_opposite(Halfedge* h) { opposite_ = h; }
            void set_next(Halfedge* h) { next_ = h; }
            void set_prev(Halfedge* h) { prev_ = h; }
            void set_facet(Facet* f) { facet_ = f ; }
            void set_vertex(Vertex* v) { vertex_ = v ; }
			void set_tex_vertex(TexVertex* t) { tex_vertex_ = t ; }

            friend class ::Geex::Map ;
            friend class ::Geex::MapMutator ;

        private:
            Halfedge* opposite_ ;
            Halfedge* next_ ;
            Halfedge* prev_ ;
            Facet* facet_ ;
            Vertex* vertex_ ;
			TexVertex_var tex_vertex_ ;
        } ;

        //______________________________________________

        
        /**
         * A Facet of a Map.
         */
        class GEEX_API Facet : public Combel {
        public:
            Facet() : halfedge_(nil) { }
            ~Facet() { halfedge_ = nil ; }
            Halfedge* halfedge() const { return halfedge_ ; }
            size_t degree() const ;
            size_t nb_edges() const { return degree() ; }
            size_t nb_vertices() const { return degree() ; }
            bool is_on_border() const ;
            bool is_triangle() const ;
            void assert_is_valid() const ;
			real area() const ;
			vec3 normal() const ;
            
        protected:
            void set_halfedge(Halfedge* h) { halfedge_ = h ; }
            friend class ::Geex::Map ;
            friend class ::Geex::MapMutator ;

        private:
            Halfedge* halfedge_ ;
        } ;


		//_________________________________________________________


        inline bool Halfedge::is_facet_key() const {
            return (facet_->halfedge() == this) ;
        }
        
        inline bool Halfedge::is_vertex_key() const {
            return (vertex_->halfedge() == this) ;
        }
        
        inline bool Halfedge::is_edge_key() const {
            return (this < opposite_) ;
        }

		//_________________________________________________________
    
    }

}
#endif

