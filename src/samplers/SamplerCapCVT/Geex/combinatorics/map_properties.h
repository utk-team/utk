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


#ifndef ___GEEX_COMBINATORICS_MAP_PROPERTIES__
#define ___GEEX_COMBINATORICS_MAP_PROPERTIES__

#include <Geex/combinatorics/map.h>
#include <Geex/properties/property.h>

namespace Geex {

//_________________________________________________________

	template <class T> 
	class MapTexVertexProperty : public Property<Map::TexVertex, T> {
	public:
		typedef Property<Map::TexVertex, T> superclass ;

		void bind(Map* map, const std::string& name) {
			superclass::bind(
				map->tex_vertex_property_manager(), name
				) ;
		}

		void bind(Map* map) { 
			superclass::bind(map->tex_vertex_property_manager()) ; 
		}

		bool bind_if_defined(Map* map, const std::string& name) {
			return superclass::bind_if_defined(map->tex_vertex_property_manager(), name) ;
		}

		MapTexVertexProperty() { }

		MapTexVertexProperty(Map* map) {
			bind(map) ;
		}

		MapTexVertexProperty(Map* map, const std::string& name) { 
			bind(map, name) ; 
		}

		static bool is_defined(Map* map, const std::string& name) {
			return superclass::is_defined(
				map->tex_vertex_property_manager(), name
				) ;
		}
	} ;

//_________________________________________________________

    template <class T> 
    class MapVertexProperty : public Property<Map::Vertex, T> {
    public:
        typedef Property<Map::Vertex, T> superclass ;

        void bind(Map* map, const std::string& name) {
            superclass::bind(
                map->vertex_property_manager(), name 
            ) ;
        }

        void bind(Map* map) { 
            superclass::bind(map->vertex_property_manager()) ; 
        }

        void bind_if_defined(Map* map, const std::string& name) {
            superclass::bind_if_defined(
                map->vertex_property_manager(), name 
            ) ;
        }
        
        MapVertexProperty() { }

        MapVertexProperty(Map* map) {
            bind(map) ;
        }

        MapVertexProperty(Map* map, const std::string& name) { 
            bind(map, name) ;
        }
        
        static bool is_defined(Map* map, const std::string& name) {
            return superclass::is_defined(
                map->vertex_property_manager(), name
            ) ;
        }
    } ;

//_________________________________________________________

    template <class T> 
    class MapHalfedgeProperty : public Property<Map::Halfedge, T> {
    public:
        typedef Property<Map::Halfedge, T> superclass ;

        void bind(Map* map, const std::string& name) {
            superclass::bind(
                map->halfedge_property_manager(), name
            ) ;
        }

        void bind(Map* map) { 
            superclass::bind(map->halfedge_property_manager()) ; 
        }

        void bind_if_defined(Map* map, const std::string& name) {
            superclass::bind_if_defined(
                map->halfedge_property_manager(), name 
            ) ;
        }
        
        MapHalfedgeProperty() { }

        MapHalfedgeProperty(Map* map) {
            bind(map) ;
        }

        MapHalfedgeProperty(Map* map, const std::string& name) { 
            bind(map, name) ; 
        }
        
        static bool is_defined(Map* map, const std::string& name) {
            return superclass::is_defined(
                map->halfedge_property_manager(), name
            ) ;
        }
    } ;

//_________________________________________________________

    template <class T> 
    class MapFacetProperty : public Property<Map::Facet, T> {
    public:
        typedef Property<Map::Facet, T> superclass ;

        void bind(Map* map, const std::string& name) {
            superclass::bind(
                map->facet_property_manager(), name
            ) ;
        }

        void bind(Map* map) { 
            superclass::bind(map->facet_property_manager()) ; 
        }

        void bind_if_defined(Map* map, const std::string& name) {
            superclass::bind_if_defined(
                map->facet_property_manager(), name 
            ) ;
        }
        
        MapFacetProperty() { }

        MapFacetProperty(Map* map) {
            bind(map) ;
        }

        MapFacetProperty(Map* map, const std::string& name) { 
            bind(map, name) ; 
        }
        
        static bool is_defined(Map* map, const std::string& name) {
            return superclass::is_defined(
                map->facet_property_manager(), name
            ) ;
        }
    } ;

	//=====================================================================
	//
	//                    standard map attributes
	//
	//=====================================================================

	class MapTexVertexNormal : public MapTexVertexProperty<vec3> {
	public:
		typedef MapTexVertexProperty<vec3> superclass ;
		MapTexVertexNormal() { }
		MapTexVertexNormal(Map* map) : superclass(map, "normal") { }
		void bind(Map* map) { superclass::bind(map, "normal");  }
		static bool is_defined(Map* map) {
			return superclass::is_defined(map,"normal") ;
		}
	} ;

//_________________________________________________________

    class MapVertexLock : public MapVertexProperty<bool> {
    public:
        typedef MapVertexProperty<bool> superclass ;
        MapVertexLock() { }
        MapVertexLock(Map* map) : superclass(map, "lock") { }
        void bind(Map* map) { superclass::bind(map, "lock") ; }
        static bool is_defined(Map* map) {
            return superclass::is_defined(map,"lock") ;
        }
    } ;

    //_________________________________________________________


}

#endif

