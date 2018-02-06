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

#ifndef __GEEX_MATHEMATICS_VECG__
#define __GEEX_MATHEMATICS_VECG__

#include <Geex/basics/types.h>
#include <Geex/basics/assert.h>
#include <iostream>
#include <cfloat>

namespace Geex {

    //--------------------------- VEC2 ------------------------------------------------------------------------------------------

    template <class T>
    struct vec2g {
        typedef vec2g<T> thisclass ;
        
        vec2g() : x(0), y(0) { }
        vec2g(T x_in, T y_in) : x(x_in), y(y_in) {  }
            
        inline T length2() const { return x*x+y*y ; }
        inline T length() const { return sqrt(x*x+y*y) ; }
            
        // operators
        inline thisclass& operator+=(const thisclass& v) { x += v.x ; y += v.y ; return *this ; }
        inline thisclass& operator-=(const thisclass& v) { x -= v.x ; y -= v.y ; return *this ; }
        inline thisclass& operator*=(const thisclass& v) { x *= v.x ; y *= v.y ; return *this ; }
        inline thisclass& operator/=(const thisclass& v) { x /= v.x ; y /= v.y ; return *this ; }
        template <class T2> inline thisclass& operator*=(T2 s) { x *= T(s) ; y *= T(s) ; return *this ; }
        template <class T2> inline thisclass& operator/=(T2 s) { x /= T(s) ; y /= T(s) ; return *this ; }

        inline thisclass operator+ (const thisclass& v) const {return thisclass(x+v.x, y+v.y); }
        inline thisclass operator- (const thisclass& v) const {return thisclass(x-v.x, y-v.y); }
        template <class T2> inline thisclass operator* (T2 s) const {return thisclass(x*T(s), y*T(s)); }
        template <class T2> inline thisclass operator/ (T2 s) const {return thisclass(x/T(s), y/T(s)); }
        inline thisclass operator- () const {return thisclass(-x, -y);}

        inline T& operator[](int idx) {
            switch(idx) {
            case 0: return x ; break ;
            case 1: return y ; break ;
            default: gx_assert_not_reached ;
            }
            return x ;
        }

        inline const T& operator[](int idx) const {
            switch(idx) {
            case 0: return x ; break ;
            case 1: return y ; break ;
            default: gx_assert_not_reached ;
            }
            return x ;
        }
        
        T x ;
        T y ;
    } ;
        
    template <class T> inline T dot(const vec2g<T>& v1, const vec2g<T>& v2) {  return v1.x*v2.x + v1.y*v2.y ;  }

    template <class T> inline  T det(const vec2g<T>& v1, const vec2g<T>& v2) {
        return v1.x*v2.y - v1.y*v2.x ;
    }
    

    template <class T> inline vec2g<T> operator-(const vec2g<T>& v1) { return vec2g<T>(-v1.x, -v1.y) ; }
    template <class T2, class T> inline vec2g<T> operator*(T2 s, const vec2g<T>& v) { return vec2g<T>(T(s)*v.x, T(s)*v.y) ;   }

    template <class T> inline vec2g<T> operator+(const vec2g<T>& v1, const vec2g<T>& v2) { 
        return vec2g<T>(v1.x+v2.x, v1.y+v2.y) ; 
    }

    template <class T> inline vec2g<T> operator-(const vec2g<T>& v1, const vec2g<T>& v2) { 
        return vec2g<T>(v1.x - v2.x, v1.y - v2.y) ; 
    }

    // Compatibility with GLSL
    template <class T> inline T length(const vec2g<T>& v) { return v.length() ; }
    template <class T> inline T length2(const vec2g<T>& v) { return v.length2() ; }
    template <class T> inline T distance2(const vec2g<T>& v1, const vec2g<T>& v2) { return length2(v2 - v1) ; }
    template <class T> inline T distance(const vec2g<T>& v1, const vec2g<T>& v2) { return length(v2 - v1) ; }
    template <class T> inline vec2g<T> normalize(const vec2g<T>& v) { return (T(1) / length(v)) * v ; }
    template <class T> inline vec2g<T> mix(const vec2g<T>& v1, const vec2g<T>& v2, real s) { return (T(1) - s) * v1 + s * v2 ; }

    //--------------------------- VEC3 ------------------------------------------------------------------------------------------

    template <class T>
    struct vec3g {
        typedef vec3g<T> thisclass ;
        
        vec3g() : x(0), y(0), z(0) { }
        vec3g(T x_in, T y_in, T z_in) : x(x_in), y(y_in), z(z_in) {  }
            
        inline T length2() const { return x*x+y*y+z*z ; }
        inline T length() const { return sqrt(x*x+y*y+z*z) ; }
            
        // operators
        inline thisclass& operator+=(const thisclass& v) { x += v.x ; y += v.y ; z += v.z ; return *this ; }
        inline thisclass& operator-=(const thisclass& v) { x -= v.x ; y -= v.y ; z -= v.z ; return *this ; }
        inline thisclass& operator*=(const thisclass& v) { x *= v.x ; y *= v.y ; z *= v.z ; return *this ; }
        inline thisclass& operator/=(const thisclass& v) { x /= v.x ; y /= v.y ; z /= v.z ; return *this ; }
        template <class T2> inline thisclass& operator*=(T2 s) { x *= T(s) ; y *= T(s) ; z *= T(s) ; return *this ; }
        template <class T2> inline thisclass& operator/=(T2 s) { x /= T(s) ; y /= T(s) ; z /= T(s) ; return *this ; }

    
        inline thisclass operator+ (const thisclass& v) const {return thisclass(x+v.x, y+v.y, z+v.z); }
        inline thisclass operator- (const thisclass& v) const {return thisclass(x-v.x, y-v.y, z-v.z); }
        template <class T2> inline thisclass operator* (T2 s) const {return thisclass(x*T(s), y*T(s), z*T(s)); }
        template <class T2> inline thisclass operator/ (T2 s) const {return thisclass(x/T(s), y/T(s), z/T(s)); }

        inline thisclass operator- () const {return thisclass(-x, -y, -z);}

        inline T& operator[](int idx) {
            switch(idx) {
            case 0: return x ; break ;
            case 1: return y ; break ;
            case 2: return z ; break ;
            default: gx_assert_not_reached ;
            }
            return x ;
        }

        inline const T& operator[](int idx) const {
            switch(idx) {
            case 0: return x ; break ;
            case 1: return y ; break ;
            case 2: return z ; break ;
            default: gx_assert_not_reached ;
            }
            return x ;
        }
        
        T x ;
        T y ;
        T z ;
    } ;
        
    template <class T> inline T dot(const vec3g<T>& v1, const vec3g<T>& v2) {  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;  }

    template <class T> inline  vec3g<T> cross(const vec3g<T>& v1, const vec3g<T>& v2) {
        return vec3g<T>(
            v1.y*v2.z - v1.z*v2.y,
            v1.z*v2.x - v1.x*v2.z,
            v1.x*v2.y - v1.y*v2.x
        ) ;
    }
        
    template <class T> inline vec3g<T> operator-(const vec3g<T>& v1) { return vec3g<T>(-v1.x, -v1.y, -v1.z) ; }
    template <class T2, class T> inline vec3g<T> operator*(T2 s, const vec3g<T>& v) { 
        return vec3g<T>(T(s)*v.x, T(s)*v.y, T(s)*v.z) ;   
    }

    template <class T> inline vec3g<T> operator+(const vec3g<T>& v1, const vec3g<T>& v2) { 
        return vec3g<T>(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z) ; 
    }

    template <class T> inline vec3g<T> operator-(const vec3g<T>& v1, const vec3g<T>& v2) { 
        return vec3g<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z) ; 
    }

    // Compatibility with GLSL
    template <class T> inline T length(const vec3g<T>& v) { return v.length() ; }
    template <class T> inline T length2(const vec3g<T>& v) { return v.length2() ; }
    template <class T> inline T distance2(const vec3g<T>& v1, const vec3g<T>& v2) { return length2(v2 - v1) ; }
    template <class T> inline T distance(const vec3g<T>& v1, const vec3g<T>& v2) { return length(v2 - v1) ; }
    template <class T> inline vec3g<T> normalize(const vec3g<T>& v) { return (T(1) / length(v)) * v ;   }
    template <class T> inline vec3g<T> mix(const vec3g<T>& v1, const vec3g<T>& v2, real s) { return (T(1) - s) * v1 + s * v2 ; }


// -------------------------- vec4 -----------------------------------------------------------------------------------------------------------------------------------

    template <class T>
    struct vec4g {
        typedef vec4g<T> thisclass ;
        
        vec4g() : x(0), y(0), z(0), w(0) { }
        vec4g(T x_in, T y_in, T z_in, T w_in) : x(x_in), y(y_in), z(z_in), w(w_in) {  }
            
        inline T length2() const { return x*x+y*y+z*z+w*w ; }
        inline T length() const { return sqrt(x*x+y*y+z*z+w*w) ; }
            
        // operators
        inline thisclass& operator+=(const thisclass& v) { x += v.x ; y += v.y ; z += v.z ; w += v.w ; return *this ; }
        inline thisclass& operator-=(const thisclass& v) { x -= v.x ; y -= v.y ; z -= v.z ; w -= v.w ; return *this ; }
        inline thisclass& operator*=(const thisclass& v) { x *= v.x ; y *= v.y ; z *= v.z ; w *= v.w ; return *this ; }
        inline thisclass& operator/=(const thisclass& v) { x /= v.x ; y /= v.y ; z /= v.z ; w /= v.w ; return *this ; }
        template <class T2> inline thisclass& operator*=(T2 s) { x *= T(s) ; y *= T(s) ; z *= T(s) ; w *= T(s) ; return *this ; }
        template <class T2> inline thisclass& operator/=(T2 s) { x /= T(s) ; y /= T(s) ; z /= T(s) ; w /= T(s) ; return *this ; }
    
        inline thisclass operator+ (const thisclass& v) const {return thisclass(x+v.x, y+v.y, z+v.z, w+v.w); }
        inline thisclass operator- (const thisclass& v) const {return thisclass(x-v.x, y-v.y, z-v.z, w-v.w); }
        template <class T2> inline thisclass operator* (T2 s) const {return thisclass(x*T(s), y*T(s), z*T(s), w*T(s)); }
        template <class T2> inline thisclass operator/ (T2 s) const {return thisclass(x/T(s), y/T(s), z/T(s), w/T(s)); }
        inline thisclass operator- () const {return thisclass(-x, -y, -z, -w);}

        inline T& operator[](int idx) {
            switch(idx) {
            case 0: return x ; break ;
            case 1: return y ; break ;
            case 2: return z ; break ;
            case 3: return w ; break ;
            default: gx_assert_not_reached ;
            }
            return x ;
        }

        inline const T& operator[](int idx) const {
            switch(idx) {
            case 0: return x ; break ;
            case 1: return y ; break ;
            case 2: return z ; break ;
            case 3: return w ; break ;
            default: gx_assert_not_reached ;
            }
            return x ;
        }
        
        T x ;
        T y ;
        T z ;
        T w ; 
    } ;
        
    template <class T> inline T dot(const vec4g<T>& v1, const vec4g<T>& v2) {  
        return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;  
    }

    template <class T> inline vec4g<T> operator-(const vec4g<T>& v1) { return vec4g<T>(-v1.x, -v1.y, -v1.z, -v1.w) ; }
    template <class T2, class T> inline vec4g<T> operator*(T2 s, const vec4g<T>& v) { 
        return vec4g<T>(T(s)*v.x, T(s)*v.y, T(s)*v.z, T(s)*v.w) ;   
    }

    template <class T> inline vec4g<T> operator+(const vec4g<T>& v1, const vec4g<T>& v2) { 
        return vec4g<T>(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z, v1.w+v2.w) ; 
    }

    template <class T> inline vec4g<T> operator-(const vec4g<T>& v1, const vec4g<T>& v2) { 
        return vec4g<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w) ; 
    }

    // Compatibility with GLSL
    template <class T> inline T length(const vec4g<T>& v) { return v.length() ; }
    template <class T> inline T length2(const vec4g<T>& v) { return v.length2() ; }
    template <class T> inline T distance2(const vec4g<T>& v1, const vec4g<T>& v2) { return length2(v2 - v1) ; }
    template <class T> inline T distance(const vec4g<T>& v1, const vec4g<T>& v2) { return length(v2 - v1) ; }
    template <class T> inline vec4g<T> normalize(const vec4g<T>& v) { return (T(1) / length(v)) * v ;   }
    template <class T> inline vec4g<T> mix(const vec4g<T>& v1, const vec4g<T>& v2, real s) { return (T(1) - s) * v1 + s * v2 ; }


}

namespace Geex {

    template <class T> inline std::ostream& operator<<(std::ostream& out, const Geex::vec2g<T>& v) {
        return out << v.x << "  " << v.y ;
    }

    template <class T> inline std::istream& operator>>(std::istream& in, Geex::vec2g<T>& v) {
        return in >> v.x >> v.y ;
    }

    template <class T> inline std::ostream& operator<<(std::ostream& out, const Geex::vec3g<T>& v) {
        return out << v.x << "  " << v.y << "  " << v.z  ;
    }

    template <class T> inline std::istream& operator>>(std::istream& in, Geex::vec3g<T>& v) {
        return in >> v.x >> v.y >> v.z ;
    }

    template <class T> inline std::ostream& operator<<(std::ostream& out, const Geex::vec4g<T>& v) {
        return out << v.x << "  " << v.y << "  " << v.z  << " " << v.w ;
    }

    template <class T> inline std::istream& operator>>(std::istream& in, Geex::vec4g<T>& v) {
        return in >> v.x >> v.y >> v.z >> v.w ;
    }

}

#endif

