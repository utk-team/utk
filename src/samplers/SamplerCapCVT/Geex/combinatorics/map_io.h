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
 
 
#ifndef __GEEX_COMBINATORICS_MAP_IO__
#define __GEEX_COMBINATORICS_MAP_IO__

#include <Geex/basics/types.h>
#include <iostream>

namespace Geex {
    
    class Map ;

    void GEEX_API load_map(std::istream& in, Map* map, bool load_tex_coords = true, bool flip_coords = false) ;
    void GEEX_API save_map(std::ostream& out, Map* map, bool save_tex_coords = true) ;
    void GEEX_API load_map(const std::string& filename, Map* map, bool load_tex_coords = true, bool flip = false) ;
    void GEEX_API save_map(const std::string& filename, Map* map, bool save_tex_coords = true) ;

} 

#endif
