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
 


#include <Geex/combinatorics/map_tstrip.h>

namespace Geex {


    void MapTStripGenerator::generate_tstrips (MapTStripSet& tstrips) {

        is_marked_.bind(map_) ;
        avg_tstrip_length_ = 0 ;

        tstrips.clear() ;
        FOR_EACH_FACET(Map, map_, it) {
            if(!is_marked_[it]) {
                is_marked_[it] = true ; // avoids cycles

                //   Tries the three different directions starting from the triangle
                // and uses the one that generates the longest TStrip.
                const Map::Halfedge* h = it->halfedge() ;
                MapTStrip tstrip ;
                size_t tstrip_len = get_tstrip(h, tstrip) ;

                h = h->next() ;
                MapTStrip new_tstrip ;
                size_t new_tstrip_len = get_tstrip(h, new_tstrip) ;
                if(new_tstrip_len > tstrip_len) {
                    tstrip = new_tstrip ;
                    tstrip_len = new_tstrip_len ;
                }

                h = h->next() ;
                new_tstrip_len = get_tstrip(h, new_tstrip) ;
                if(new_tstrip_len > tstrip_len) {
                    tstrip = new_tstrip ;
                    tstrip_len = new_tstrip_len ;
                }
                mark_tstrip(tstrip) ;
                tstrips.push_back(tstrip) ;
                avg_tstrip_length_ += tstrip_len ;
            }
        }
        is_marked_.unbind() ;
        if(tstrips.size() != 0) {
            avg_tstrip_length_ /= (size_t)tstrips.size() ;
        }
    }


    size_t MapTStripGenerator::get_tstrip (const Map::Halfedge* h, MapTStrip& tstrip) {
        size_t len = 1 ;

        const Map::Halfedge* start  = h ;
        const Map::Halfedge* finish = h ;

        // Forward TStrip marching
        finish = h ;
        bool even = true ;
        while(true) {
            Map::Halfedge* next = 
                even ? finish->next()->opposite()->next() : finish->opposite()->next() ;
            if(next->is_border() || is_marked_[next->facet()]) {
                break ;
            }
            len++ ;
            even = !even ;
            finish = next ;
        }

        // Backward TStrip marching
        start = h ;
        even = false ;
        const Map::Halfedge* prev_start = start ;
        while(true) {
            Map::Halfedge* prev = 
                even ? start->prev()->opposite()->prev() : start->prev()->opposite() ;
            if(prev->is_border() || is_marked_[prev->facet()]) {
                break ;
            }
            len++ ;
            even = !even ;
            prev_start = start ;
            start = prev ;
        }

        even = !even ;

        tstrip.even = even ;
        tstrip.start = start ;
        tstrip.finish = finish ;

        return len ;
    }

    void MapTStripGenerator::mark_tstrip (MapTStrip& tstrip) {
        const Map::Halfedge* h = tstrip.start ;
        bool even = tstrip.even ;
    
        while(h != tstrip.finish) {
            is_marked_[h->facet()] = true ;
            if(even) {
                h = h->next()->opposite()->next() ;
            } else {
                h = h->opposite()->next() ;
            }
            even = !even ;
        }
        is_marked_[h->facet()] = true ;
    }

} 
