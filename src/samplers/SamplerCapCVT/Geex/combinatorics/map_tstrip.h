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
 
#ifndef __GEEX_COMBINATORICS_MAP_TSTRIP__
#define __GEEX_COMBINATORICS_MAP_TSTRIP__

#include <Geex/combinatorics/map.h>
#include <Geex/combinatorics/map_properties.h>

#include <vector>

namespace Geex {

   /**
    * Describes a triangle strip.
    * TODO: add an iterator to do that
    * Example of traversal:
    * \code
    * glBegin(GL_TRIANGLE_STRIP) ;
    * Map::Halfedge* h = tstrip.start ;
    * bool even = tstrip.even ;
    * if(even) {
    *    glVertex(h->prev()->vertex()->point()) ;
    *    glVertex(h->next()->vertex()->point()) ;
    *  } else {
    *    glVertex(h->next()->vertex()->point()) ;
    *    glVertex(h->prev()->vertex()->point()) ;
    *  }
    *  while(h != tstrip.finish) {
    *    glVertex(h->vertex()->point()) ;
    *    if(even) {
    *      h = h->next()->opposite()->next() ;
    *    } else {
    *      h = h->opposite()->next() ;
    *    }
    *    even = !even ;
    *  } 
    *  glVertex(h->vertex()->point()) ;
    *  glEnd() ;
    * \endcode
    */
    struct MapTStrip  {
        MapTStrip (Map::Halfedge* start_in,  Map::Halfedge* finish_in, bool even_in)
	    : start(start_in), finish(finish_in), even(even_in) { 
        }
        MapTStrip () : start(nil), finish(nil), even(true) { }
        const Map::Halfedge* start ;
        const Map::Halfedge* finish ;
        bool even ;
    } ;

    typedef std::vector<MapTStrip> MapTStripSet ;

   /**
    * Generates a triangle strip set covering a given map.
    * The map needs to be triangulated.
    */
    class GEEX_API MapTStripGenerator {
    public:
        MapTStripGenerator (Map* map_in) : map_(map_in) {  }
        void generate_tstrips (MapTStripSet& tstrips) ;
        size_t average_tstrip_length() const { return avg_tstrip_length_ ; }

    protected:
        /** returns the length of the STrip */
        size_t get_tstrip (const Map::Halfedge* h, MapTStrip& tstrip) ;
        void mark_tstrip (MapTStrip& tstrip) ;

    private:
        Map* map_ ;
        MapFacetProperty<bool> is_marked_ ;
        size_t avg_tstrip_length_ ;
    } ;
} 

#endif 
