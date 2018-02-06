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

#ifndef __GEEX_MATHEMATICS_GLSL_LINEAR__
#define __GEEX_MATHEMATICS_GLSL_LINEAR__

#include <Geex/mathematics/vecg.h>
#include <Geex/mathematics/matrix.h>
#include <Geex/mathematics/box.h>

namespace Geex {

    typedef vec2g<real> vec2 ;
    typedef vec3g<real> vec3 ;
    typedef vec4g<real> vec4 ;
    
    typedef Matrix<real, 3> mat3 ;
    typedef Matrix<real, 4> mat4 ;

    typedef GenericBox2d<real> Box2d ;
    typedef GenericBox3d<real> Box3d ;

    namespace Numeric {

        inline bool has_nan(const vec2& v) {
            return is_nan(v.x) || is_nan(v.y) ;
        }

        inline bool has_nan(const vec3& v) {
            return is_nan(v.x) || is_nan(v.y) || is_nan(v.z) ;
        }

        inline bool has_nan(const vec4& v) {
            return is_nan(v.x) || is_nan(v.y) || is_nan(v.z) || is_nan(v.w) ;
        }

    }

}


#endif

