
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
 

#ifndef __GEEX_COMBINATORICS_ITERATORS__
#define __GEEX_COMBINATORICS_ITERATORS__

// Note: I dont't like macros, but iterators are really too 
//  annoying to use.

#define FOR_EACH_VERTEX(T,map,it)                         \
    for(                                                  \
        T::Vertex_iterator it=(map)->vertices_begin();    \
        it!=(map)->vertices_end(); it++                   \
    )

#define FOR_EACH_HALFEDGE(T,map,it)                          \
    for(                                                     \
        T::Halfedge_iterator it=(map)->halfedges_begin();    \
        it!=(map)->halfedges_end(); it++                     \
    )

#define FOR_EACH_EDGE(T,map,it)                              \
    for(                                                     \
        T::Halfedge_iterator it=(map)->halfedges_begin();    \
        it!=(map)->halfedges_end(); it++                     \
    ) if(it->is_edge_key())                                  \


#define FOR_EACH_FACET(T,map,it)                       \
    for(                                               \
        T::Facet_iterator it=(map)->facets_begin();    \
        it!=(map)->facets_end(); it++                  \
    )


#define FOR_EACH_CELL(T,map,it)                        \
    for(                                               \
        T::Cell_iterator it=(map)->cells_begin();      \
        it!=(map)->cells_end(); it++                   \
    )

#define FOR_EACH_VERTEX_CONST(T,map,it)                       \
    for(                                                      \
        T::Vertex_const_iterator it=map->vertices_begin();    \
        it!=map->vertices_end(); it++                         \
    )

#define FOR_EACH_HALFEDGE_CONST(T,map,it)                        \
    for(                                                         \
        T::Halfedge_const_iterator it=map->halfedges_begin();    \
        it!=map->halfedges_end(); it++                           \
    )

#define FOR_EACH_EDGE_CONST(T,map,it)                           \
    for(                                                        \
        T::Halfedge_const_iterator it=(map)->halfedges_begin(); \
        it!=(map)->halfedges_end(); it++                        \
    ) if(it->is_edge_key())                                     \


#define FOR_EACH_FACET_CONST(T,map,it)                     \
    for(                                                   \
        T::Facet_const_iterator it=map->facets_begin();    \
        it!=map->facets_end(); it++                        \
    )

#define FOR_EACH_CELL_CONST(T,map,it)                   \
    for(                                                \
        T::Cell_const_iterator it=(map)->cells_begin(); \
        it!=(map)->cells_end(); it++                    \
    )


// This stupid C++ compiler requires the 'typename' keyword !!
// This means we cannot use the classic FOR_EACH_FACET macro.

#define FOR_EACH_FACET_GENERIC(MAPTYPE, MAP, IT)                  \
for( typename MAPTYPE::Facet_iterator IT = MAP->facets_begin();   \
     IT != MAP->facets_end(); IT++ )                              \

#define FOR_EACH_HALFEDGE_GENERIC(MAPTYPE, MAP, IT)                  \
for( typename MAPTYPE::Halfedge_iterator IT = MAP->halfedges_begin();   \
     IT != MAP->halfedges_end(); IT++ )                              \

#define FOR_EACH_VERTEX_GENERIC(MAPTYPE, MAP, IT)                  \
for( typename MAPTYPE::Vertex_iterator IT = MAP->vertices_begin(); \
     IT != MAP->vertices_end(); IT++ )                             \


#endif
