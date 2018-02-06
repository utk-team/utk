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

#ifndef __GEEX_MATH_TRGL_GRAD__
#define __GEEX_MATH_TRGL_GRAD__

#include <Geex/mathematics/geometry.h>

namespace Geex {
    
//_________________________________________________________
    
    /**
     * computes gradients in a triangle in 3D.
     */
    class GEEX_API TrglGradient {
    public:
        TrglGradient(
            const vec3& p0, const vec3& p1, const vec3& p2
        ) ;
        
        /** Creates an uninitialized TrglGradient */
        TrglGradient() ;
        
        void initialize(
            const vec3& p0, const vec3& p1, const vec3& p2
        ) ;

        /**
         * Returns the ith vertex of the triangle.
         * @param i is the index of the vertex, 
         *        which can be one of 0,1,2.
         */
        const vec3& vertex(int i) const ;
        
        /**
         * Returns the orthonormal basis in which gradient computation are
         * performed.
         */
        void basis(
            vec3& origin, vec3& X, vec3& Y, vec3& Z
        ) const ;
        
        /**
         * Returns the coefficients determining the gradient in this triangle.
         *
         * grad_X = sum_i { TX(i) * vertex(i)-> embedding(prop) }
         * Note: TX(2) == 0
         */
        double TX(int i) const ;
        
        /**
         * Returns the coefficients determining the gradient in this triangle.
         *
         * grad_Y = sum_i { TY(i) * vertex(i)-> embedding(prop) }
         */
        double TY(int i) const ;
        bool is_flat() const { return is_flat_ ; }

        vec3 gradient_3d(double value0, double value1, double value2) const ;

    private:
        double TX_[3] ;
        double TY_[3] ;
        vec3 vertex_[3] ;
        bool is_flat_ ;
    } ;
    
//___________________________________________________

    /**
     * computes gradients in a triangle in 2D.
     */
    class GEEX_API ParamTrglGradient {
    public:
        ParamTrglGradient() ;
        ParamTrglGradient(
            const vec2& p1, const vec2& p2, const vec2& p3
        ) ;
        const vec2& vertex(int i) const ;
        double TX(int i) const ;
        double TY(int i) const ;
        bool is_flat() const { return is_flat_ ; }

    private:
        double TX_[3] ;
        double TY_[3] ;
        vec2 vertex_[3] ;
        bool is_flat_ ;
    } ;

//___________________________________________________    
    
    inline const vec3& TrglGradient::vertex(int i) const { 
        return vertex_[i] ; 
    }

    inline double TrglGradient::TX(int i) const { return TX_[i] ; }
    inline double TrglGradient::TY(int i) const { return TY_[i] ; }

//_________________________________________________________

    inline const vec2& ParamTrglGradient::vertex(int i) const { 
        return vertex_[i] ; 
    }

    inline double ParamTrglGradient::TX(int i) const { return TX_[i] ; }
    inline double ParamTrglGradient::TY(int i) const { return TY_[i] ; }
    
//_________________________________________________________
}

#endif

