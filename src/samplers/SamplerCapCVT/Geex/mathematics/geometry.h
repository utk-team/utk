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
 
#ifndef __GEEX_MATHEMATICS_GEOMETRY__
#define __GEEX_MATHEMATICS_GEOMETRY__

#include <Geex/mathematics/glsl_linear.h>

namespace Geex {

    namespace Geom {

       //----------------------------------------------------------------------------------------------------------------
       
        inline double triangle_area(
            const vec3& v1, const vec3& v2, const vec3& v3
        ) {
            return 0.5f * length(cross(v2-v1, v3-v1)) ;
        }

        inline real tetra_signed_area(
            const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4
        ) {
            return 0.5f * (dot( cross(p2 - p1,p3 - p1), p4 - p1)) ;
        }

        inline vec2 barycenter(const vec2& v1, const vec2& v2) {
            return mix(v1, v2, 0.5f) ;
        }

        inline vec3 barycenter(const vec3& v1, const vec3& v2) {
            return mix(v1, v2, 0.5f) ;
        }

        /**
         * Wrapper for shewchuk's robust orientation predicate.
         */
        Sign GEEX_API orient(
            const vec2& p1, const vec2& p2, const vec2& p3
        ) ;

        //----------------------------------------------------------------------------------------------------------------
    }

}

#endif

