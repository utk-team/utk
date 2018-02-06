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
 
 
 
 
 
 
 
 
#ifndef __GEEX_BASICS_ASSERT__
#define __GEEX_BASICS_ASSERT__

#include <Geex/basics/types.h>
#include <string>

#ifndef NDEBUG
#define GX_DEBUG
#endif

namespace Geex {

    void GEEX_API gx_assertion_failed(
        const std::string& condition_string,
        const std::string& file, int line
    ) ;
    
    void GEEX_API gx_range_assertion_failed(
        double value, double min_value, double max_value, 
        const std::string& file, int line
    ) ;

    void GEEX_API gx_should_not_have_reached(
        const std::string& file, int line
    ) ;

}


// Three levels of assert:
// use gx_assert() and gx_range_assert()               for non-expensive asserts
// use gx_debug_assert() and gx_debug_range_assert()   for expensive asserts
// use gx_parano_assert() and gx_parano_range_assert() for very exensive asserts

#define gx_assert(x) {                                        \
    if(!(x)) {                                                 \
        ::Geex::gx_assertion_failed(#x,__FILE__, __LINE__) ;   \
    }                                                          \
} 

#define gx_range_assert(x,min_val,max_val) {                  \
    if(((x) < (min_val)) || ((x) > (max_val))) {               \
        ::Geex::gx_range_assertion_failed(x, min_val, max_val, \
            __FILE__, __LINE__                                 \
        ) ;                                                    \
    }                                                          \
}

#define gx_assert_not_reached {                               \
    ::Geex::gx_should_not_have_reached(__FILE__, __LINE__) ;   \
}

#ifdef GX_DEBUG
#define gx_debug_assert(x) gx_assert(x)
#define gx_debug_range_assert(x,min_val,max_val) gx_range_assert(x,min_val,max_val)
#else
#define gx_debug_assert(x) 
#define gx_debug_range_assert(x,min_val,max_val) 
#endif

#ifdef GX_PARANOID
#define gx_parano_assert(x) gx_assert(x)
#define gx_parano_range_assert(x,min_val,max_val) gx_range_assert(x,min_val,max_val)
#else
#define gx_parano_assert(x) 
#define gx_parano_range_assert(x,min_val,max_val) 
#endif

#endif
