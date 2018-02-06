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
 
#ifndef __GEEX_GRAPHICS_COLOR__
#define __GEEX_GRAPHICS_COLOR__

#include "../mathematics/glsl_linear.h"
#include "../basics/assert.h"


namespace Geex {

    class ColorRamp {
    public:
        ColorRamp(
            const vec4& c1 = vec4(0,0,1,1), const vec4& c2 = vec4(1,0,0,1)
        ) : c1_(c1), c2_(c2), xmin_(0.0), xmax_(1.0), scal_(1.0) { 
        }
        vec4 color(real x) {
            real val = (x - xmin_)*scal_ ;
            return mix(c1_, c2_, val) ;
        }
        void set_range(real xmin, real xmax) {
            xmin_ = xmin ; xmax_ = xmax ; 
            end_range() ;
        }
        void set_colors(const vec4& c1, const vec4& c2) {
            c1_ = c1 ; c2_ = c2 ;
        }
        void begin_range() { xmin_ = 1e30f; xmax_ = -1e30f; scal_ = 1.0f ; }
        void add_value_to_range(real x) { xmin_ = gx_min(xmin_, x) ; xmax_ = gx_max(xmax_, x) ; }
        void end_range() { if(xmax_ == xmin_) { xmax_ += 1e-10f ; } scal_ = 1.0f / (xmax_ - xmin_) ; }
    private:
        vec4 c1_ ;
        vec4 c2_ ;
        real xmin_ ;
        real xmax_ ;
        real scal_ ;
    } ;

}

#endif

