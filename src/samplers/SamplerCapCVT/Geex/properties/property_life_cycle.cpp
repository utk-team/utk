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
 
 
 
 
 
 
 

#include <Geex/properties/property_life_cycle.h>

namespace Geex {

//_________________________________________________________

    void PropertyLifeCycle::virtual_construct(
        Memory::pointer addr
    ) {
        gx_assert(false) ; // should not be called.
    }
    
    void PropertyLifeCycle::virtual_destroy(
        Memory::pointer addr
    ) {
        gx_assert(false) ; // should not be called.
    }
    
    void PropertyLifeCycle::virtual_copy(
        Memory::pointer lhs, Memory::pointer rhs 
    ) {
        gx_assert(false) ; // should not be called.
    }
    
    void PropertyLifeCycle::virtual_copy_construct(
        Memory::pointer lhs, Memory::pointer rhs 
    ) {
        gx_assert(false) ; // should not be called.
    }
    
    void PropertyLifeCycle::notify_construct(
        Memory::pointer addr, Record* record
    ) {
        gx_assert(false) ; // should not be called.
    }
    
    void PropertyLifeCycle::notify_destroy(
        Memory::pointer addr, Record* record
    ) {
        gx_assert(false) ; // should not be called.
    }
    
    void PropertyLifeCycle::notify_copy(
        Memory::pointer lhs, Record* record_lhs,
        Memory::pointer rhs, const Record* record_rhs
    ) {
        gx_assert(false) ; // should not be called.
    }
    
    void PropertyLifeCycle::notify_copy_construct(
        Memory::pointer lhs, Record* record_lhs,
        Memory::pointer rhs, const Record* record_rhs
    ) {
        gx_assert(false) ; // should not be called.
    }

//_________________________________________________________

}

