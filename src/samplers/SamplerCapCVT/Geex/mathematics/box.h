
/*
 *  GEEX/Graphite: Geometry and Graphics Programming Library + Utilities
 *  Copyright (C) 2000 Bruno Levy
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
 *  Contact: Bruno Levy
 *
 *     levy@loria.fr
 *
 *     ISA Project
 *     LORIA, INRIA Lorraine, 
 *     Campus Scientifique, BP 239
 *     54506 VANDOEUVRE LES NANCY CEDEX 
 *     FRANCE
 *
 *  Note that the GNU General Public License does not permit incorporating
 *  the Software into proprietary programs. 
 */

#ifndef __GEEX_MATH_GEOMETRY_BOX__
#define __GEEX_MATH_GEOMETRY_BOX__

#include <Geex/basics/types.h>
#include <Geex/mathematics/vecg.h>
#include <Geex/basics/assert.h>

namespace Geex {
    
    template <class T> class GenericBox2d {
    public:
        GenericBox2d() : initialized_(false), x_min_(1e30), y_min_(1e30), x_max_(-1e30), y_max_(-1e30) { }
        bool initialized() const { return initialized_ ; }
        void clear() { initialized_ = false ; }
        T x_min() const { gx_assert(initialized_); return x_min_ ; }
        T y_min() const { gx_assert(initialized_); return y_min_ ; }
        T x_max() const { gx_assert(initialized_); return x_max_ ; }
        T y_max() const { gx_assert(initialized_); return y_max_ ; }
        T min(unsigned axis) const { return (axis==0)?x_min_:y_min_; }
        T max(unsigned axis) const { return (axis==0)?x_max_:y_max_; }
        T width()  const { return x_max() - x_min() ; }
        T height() const { return y_max() - y_min() ; }
        vec2g<T> center() const {
            return vec2g<T>((x_max() + x_min())/2, (y_max() + y_min())/2) ;
        }
        double radius() const {
            return 0.5 * ::sqrt( gx_sqr(x_max()-x_min()) + gx_sqr(y_max()-y_min()) ) ;
        }
        void add_point(const vec2g<T>& p) {
            if(!initialized_) {
                x_min_ = p.x ;
                y_min_ = p.y ;
                x_max_ = p.x ;
                y_max_ = p.y ;
                initialized_ = true ;
            } else {
                x_min_ = gx_min(x_min_, p.x) ;
                y_min_ = gx_min(y_min_, p.y) ;
                x_max_ = gx_max(x_max_, p.x) ;
                y_max_ = gx_max(y_max_, p.y) ;
            }
        }
        void add_box(const GenericBox2d<T>& b) {
            if(b.initialized()) {
                add_point(vec2g<T>(b.x_min(), b.y_min())) ;
                add_point(vec2g<T>(b.x_max(), b.y_max())) ;
            }
        }

		void scale(const T s) {
			vec2g<T> c = center() ;
			x_min_ = c[0] + s*(x_min_-c[0]) ;
			y_min_ = c[1] + s*(y_min_-c[1]) ;
			x_max_ = c[0] + s*(x_max_-c[0]) ;
			y_max_ = c[1] + s*(y_max_-c[1]) ;
		}
		void scale(const T sx, const T sy) {
			vec2g<T> c = center() ;
			x_min_ = c[0] + sx*(x_min_-c[0]) ;
			x_max_ = c[0] + sx*(x_max_-c[0]) ;
			y_min_ = c[1] + sy*(y_min_-c[1]) ;			
			y_max_ = c[1] + sy*(y_max_-c[1]) ;
		}

    private:
        bool initialized_ ;
        T x_min_ ;
        T y_min_ ;
        T x_max_ ;
        T y_max_ ;
    } ;

//_________________________________________________________________________

    template <class T> class GenericBox3d {
    public:
        GenericBox3d() : initialized_(false), 
                         x_min_(1e30), y_min_(1e30), z_min_(1e30), 
                         x_max_(-1e30), y_max_(-1e30), z_max_(-1e30) { 
        }
        bool initialized() const { return initialized_ ; }
        void clear() { initialized_ = false ; }
        T x_min() const { gx_assert(initialized_); return x_min_ ; }
        T y_min() const { gx_assert(initialized_); return y_min_ ; }
        T z_min() const { gx_assert(initialized_); return z_min_ ; }
        T x_max() const { gx_assert(initialized_); return x_max_ ; }
        T y_max() const { gx_assert(initialized_); return y_max_ ; }
        T z_max() const { gx_assert(initialized_); return z_max_ ; }
        T min(unsigned axis) const { return (axis==0)?x_min_:((axis==1)?y_min_:z_min_); }
        T max(unsigned axis) const { return (axis==0)?x_max_:((axis==1)?y_max_:z_max_); }
        T width()  const { return x_max() - x_min() ; }
        T height() const { return y_max() - y_min() ; }
        T depth()  const { return z_max() - z_min() ; }
        vec3g<T> center() const {
            return vec3g<T>(
                0.5*(x_max() + x_min()), 
                0.5*(y_max() + y_min()),
                0.5*(z_max() + z_min())
            ) ;
        }
        double radius() const {
            return 0.5 * ::sqrt( 
                gx_sqr(x_max()-x_min()) + gx_sqr(y_max()-y_min()) + gx_sqr(z_max()-z_min()) 
            ) ;
        }
        void add_point(const vec3g<T>& p) {
            if(!initialized_) {
                x_min_ = p.x ;
                y_min_ = p.y ;
                z_min_ = p.z ;
                x_max_ = p.x ;
                y_max_ = p.y ;
                z_max_ = p.z ;
                initialized_ = true ;
            } else {
                x_min_ = gx_min(x_min_, p.x) ;
                y_min_ = gx_min(y_min_, p.y) ;
                z_min_ = gx_min(z_min_, p.z) ;
                x_max_ = gx_max(x_max_, p.x) ;
                y_max_ = gx_max(y_max_, p.y) ;
                z_max_ = gx_max(z_max_, p.z) ;
            }
        }
        void add_box(const GenericBox3d<T>& b) {
            if(b.initialized()) {
                add_point(vec3g<T>(b.x_min(), b.y_min(), b.z_min())) ;
                add_point(vec3g<T>(b.x_max(), b.y_max(), b.z_max())) ;
            }
        }

		void scale(const T s) {
			vec3g<T> c = center() ;
			x_min_ = c[0] + s*(x_min_-c[0]) ;
			y_min_ = c[1] + s*(y_min_-c[1]) ;
			z_min_ = c[2] + s*(z_min_-c[2]) ;
			x_max_ = c[0] + s*(x_max_-c[0]) ;
			y_max_ = c[1] + s*(y_max_-c[1]) ;
			z_max_ = c[2] + s*(z_max_-c[2]) ;
		}

    private:
        bool initialized_ ;
        T x_min_ ;
        T y_min_ ;
        T z_min_ ;
        T x_max_ ;
        T y_max_ ;
        T z_max_ ;
    } ;

//_________________________________________________________________________

}

#endif
