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

#ifndef __GENERIC_GEOMETRY__
#define __GENERIC_GEOMETRY__

#include <Geex/symbolics/expression.h>
#include <Geex/mathematics/glsl_linear.h>

namespace Geex {

    template <class T> class Line {
    public:
        Line() { }

        Line(T a_in, T b_in, T c_in) :
            a(a_in), b(b_in), c(c_in) {
        }

        Line(
            const vec2g<T>& P, const vec2g<T>& N
        ) : a(N.x), b(N.y), c(-dot(P,N)) {
        }

        T side(const vec2g<T>& p) const { 
            T result = a*p.x + b*p.y + c ; 
            return result ;
        }

        vec3g<T> normal() const { return vec2(a,b) ; }

		void inverse() { a = -a ; b = -b ; c = -c ;}

        T a,b,c ;
    } ;

    template <class T> inline T det2x2(
        T a00, T a01,
        T a10, T a11
    ) {
        return a00*a11 - a10 * a01 ;
    }

    template <class T> inline T det3x3(
        T a00,  T a01,  T a02,
        T a10,  T a11,  T a12,
        T a20,  T a21,  T a22
    ) {
        T m01 = a00*a11 - a10*a01;
        T m02 = a00*a21 - a20*a01;
        T m12 = a10*a21 - a20*a11;
        return m01*a22 - m02*a12 + m12*a02;
    }

    template <class T> inline void comatrix2x2(
        T a00,  T a01,  
        T a10,  T a11,  
        T& b00,  T& b01, 
        T& b10,  T& b11 
    ) {
        b00 = a11 ;
        b01 = -a10 ;
        b10 = -a01 ;
        b11 = a00 ;
    }

    template <class T> inline void comatrix3x3(
        T a00,  T a01,  T a02,
        T a10,  T a11,  T a12,
        T a20,  T a21,  T a22,
        T& b00,  T& b01,  T& b02,
        T& b10,  T& b11,  T& b12,
        T& b20,  T& b21,  T& b22
    ) {
        b00 = a11*a22 - a12*a21 ;
        b01 = a12*a20 - a10*a22 ;
        b02 = a10*a21 - a11*a20 ;
        b10 = a02*a21 - a01*a22 ;
        b11 = a00*a22 - a02*a20 ;
        b12 = a01*a20 - a00*a21 ;
        b20 = a01*a12 - a02*a11 ;
        b21 = a02*a10 - a00*a12 ;
        b22 = a00*a11 - a01*a10 ;
    }

    template <class T> inline void invert3x3(
        T a00,  T a01,  T a02,
        T a10,  T a11,  T a12,
        T a20,  T a21,  T a22,
        T& b00,  T& b01,  T& b02,
        T& b10,  T& b11,  T& b12,
        T& b20,  T& b21,  T& b22
    ) {
        T d = det3x3(
            a00, a01, a02,
            a10, a11, a12,
            a20, a21, a22
        )  ;

        // Compute transpose of the comatrix
        // (note that bij's are transposed in func call)
        comatrix3x3(
            a00, a01, a02,
            a10, a11, a12,
            a20, a21, a22,
            b00, b10, b20,
            b01, b11, b21,
            b02, b21, b22
        ) ;

        b00 /= d ; b01 /= d ; b02 /= d ;
        b10 /= d ; b11 /= d ; b12 /= d ;
        b20 /= d ; b21 /= d ; b22 /= d ;
    }

    template <class T> inline vec2g<T> triangle_circumcenter(
        const vec2g<T>& p, const vec2g<T>& q, const vec2g<T>& r
    ) {
        vec2g<T> qp = q-p ;
        T qp2 = qp.length2() ;
        vec2g<T> rp = r-p ;
        T rp2 = rp.length2() ;
            
        T num_x = det2x2(
            rp.y, qp.y,
            rp2,  qp2
        );

        T num_y = det2x2(
            rp.x, qp.x,
            rp2,  qp2
        ) ;

        T den = det2x2(
            qp.x,qp.y,
            rp.x,rp.y
        ) ;

        den *= 2.0 ;
            
        return vec2g<T>(
            p.x + num_x / den,
            p.y - num_y / den
        ) ;

    }

    template <class T> inline T triangle_area(
        const vec2g<T>& p, const vec2g<T>& q, const vec2g<T>& r
    ) {
        vec2g<T> U = q-p ;
        vec2g<T> V = r-p ;
        return T(1) / T(2) * det2x2(U.x,V.x,U.y,V.y) ;
    }

    template <class T> inline vec2g<T> triangle_centroid(
        const vec2g<T>& p, const vec2g<T>& q, const vec2g<T>& r
        
    ) {
        return T(1) / T(3) * (p+q+r) ;
    }

    template <class T> inline bool seg_line_intersects(
        const vec2g<T>& p1, const vec2g<T>& p2, const Line<T>& L
    ) {
        return (L.side(p1) * L.side(p2)) < T(0) ;
    }

    template <class T> inline vec2g<T> ray_line_intersection(
        const vec2g<T>& p0, const vec2g<T>& dir, const Line<T>& L
    ) {
        vec2g<T> N = L.normal() ;
        T t = - (L.c + dot(p0,N)) / dot(dir,N) ;
        return p0 + t * dir ;
    }

    template <class T> inline vec3g<T> seg_line_intersection(
        const vec2g<T>& p1, const vec2g<T>& p2, const Line<T>& L
    ) {
        return ray_line_intersection(p1, p2-p1, L) ;
    }

    template <class T> inline vec2g<T> two_lines_intersection(
        const Line<T>& L1, const Line<T>& L2
    ) {
        T b00, b01 ;
        T b10, b11 ;
        // Note: b is transposed
        comatrix2x2(
            L1.a, L1.b,
            L2.a, L2.b,
            b00, b10, 
            b01, b11
        ) ;
        return -T(1) / det2x2(
            L1.a, L1.b, 
            L2.a, L2.b
        ) * vec2g<T>(
            L1.c * b00 + L2.c * b01, 
            L1.c * b10 + L2.c * b11
        ) ;
    }

    template <class T> inline Line<T> median_line(const vec2g<T>& p1, const vec2g<T>& p2) {
        return Line<T>( (T(1) / T(2)) * (p1 + p2), p2 - p1) ;
    }


    template <class T> inline T orient(const vec2g<T>& p1, const vec2g<T>& p2, const vec2g<T>& p3) {
        return det(p2-p1, p3-p1) ;
    }

    inline bool seg_seg_intersect(const vec2& p1, const vec2& p2, const vec2& p3, const vec2& p4) {
        double s1 = orient(p1,p2,p3) ;
        double s2 = orient(p1,p2,p4) ;
        if(s1 >= 0 && s2 >= 0) { return false ; }
        if(s1 <= 0 && s2 <= 0) { return false ; }
        double s3 = orient(p3,p4,p1) ;
        double s4 = orient(p3,p4,p2) ;
        if(s3 >= 0 && s4 >= 0) { return false ; }
        if(s3 <= 0 && s4 <= 0) { return false ; }
        return true ;
    }

    inline bool intersect_segments(
        const vec2& p1, const vec2& p2,
        const vec2& q1, const vec2& q2,
        vec2& result
    ) {

        vec2 Vp = p2 - p1 ;
        vec2 Vq = q2 - q1 ;
        vec2 pq = q1 - p1 ;
        
        double a =  Vp.x ;
        double b = -Vq.x ;
        double c =  Vp.y ;
        double d = -Vq.y ;
        
        double delta = a*d-b*c ;
        if(delta == 0.0) {
            return false ;
        }
        
        double tp = (d * pq.x -b * pq.y) / delta ;
        
        result = vec2(
            (1.0 - tp) * p1.x + tp * p2.x,
            (1.0 - tp) * p1.y + tp * p2.y
        ) ;
        
        return true ;
    }

    inline bool intersect_seg_line(
        const vec2& p1, const vec2& p2,
        const Line<real>& L,
        vec2& result
    ) {
        vec2 V = p2 - p1 ;
        double delta = L.a * V.x + L.b * V.y ;
        if(delta == 0.0) {
            return false ;
        }
        double t = -(L.a*p1.x + L.b*p1.y + L.c) / delta ;
        result = p1 + t*V ;
        return true ;
    }

	template <class T> inline bool same_point(const vec2g<T>& p0, const vec2g<T>& p1) {
		if(distance(p0, p1)==0.0)
			return true ;
		return false ; 
	}

	inline double to_degree(const double angle) { return angle/M_PI*180; }
	inline double to_arc(const double angle) { return angle/180*M_PI; }
}

#endif
