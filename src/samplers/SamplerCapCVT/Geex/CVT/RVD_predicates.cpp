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

#include <Geex/CVT/RVD_predicates.h>
#include <Geex/CVT/geometry.h>
#include <Geex/CVT/RVD.h>


#include <CGAL/Simple_cartesian.h>
#include <CGAL/Interval_nt.h>
#include <CGAL/Uncertain.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Cartesian_converter.h>
#include <CGAL/determinant.h>

// #define NO_STATIC_FILTERS

#ifdef WIN32
#define NO_STATIC_FILTERS
#endif

#ifndef NO_STATIC_FILTERS
#define FPG_UNCERTAIN_VALUE -2 

// SF stands for Static Filter
namespace RVD_CGAL_SF {
#include "side1_static_filter.h" 
#include "side2_static_filter.h" 
#include "side3_static_filter.h" 
}
#endif

namespace Geex {

    // The predicates side2 and side3 compute two polynoms simultaneously 
    // (the result is the product of the signs). Rather than computing the
    // product of the polynoms, we compute both terms and check both for 
    // "certainty".
    // This requires directly calling filtered and exact predicates instead
    // of using CGAL's Filtered_predicate class.
    //
    // Performances (tested on Bruno's machine with pegaso.obj, 1 seed per facet, 3 iterations)
    //      doubles: 1.6s / iter (inexact)
    //  doubles+sym:   4s / iter (inexact)
    //    Version 0: 160s / iter (using naive implementation, with exact constructions)
    //    Version 1: 30s  / iter (separated filtered numerator and denominator, 
    //                            recomputes denominator twice).
    //    Version 2: 25s  / iter (Version 1 with Yang's 'try hard' MAPLE expression simplification)
    //    Version 3: 20s  / iter (combined computation and filtering of numerator and denominator)
    //    Version 4: 18s  / iter (Version 3 with Yang's 'try hard' MAPLE expression simplification)
    //    Version 5: 7s   / iter (Version 4 + Meyer and Pion's quasi-static filtering
    //
    // ========================================================================
    //
    // Notes: * temporary variables that are no-longer modified are declared as
    //  'const', as done in CGAL's kernel (seems to help the compiler generate
    //  faster code, to be checked).
    //        * I copied the framework that triggers exact computations from
    //  CGAL's 'Filtered_predicate' class. I do not think that the 'try-catch'
    //  bloc is necessary, since we check 'is_certain' before converting to 
    //  plain Comparison_result ('get_certain'), therefore the 'try-catch' bloc
    //  is removed.
    

    namespace RVD_CGAL_P {
        typedef CGAL::Simple_cartesian<double> K;
        typedef CGAL::Simple_cartesian<CGAL::Interval_nt_advanced> FK;
        typedef CGAL::Simple_cartesian<CGAL::MP_Float> EK;
        typedef CGAL::Cartesian_converter<K, EK> C2E;
        typedef CGAL::Cartesian_converter<K, FK> C2F;
        typedef K::Comparison_result Comparison_result ;
        C2E c2e ;
        C2F c2f ;

        int total_pred_count = 0 ;
        int exact_pred_count = 0 ;
        int dynfilt_pred_count = 0 ;

        bool verbose = true ;

        void reset_stats() {
            total_pred_count = 0 ;
            dynfilt_pred_count = 0 ;
            exact_pred_count = 0 ;
        }

        void show_stats() {
            if(verbose) {
                std::cerr << "RVD exact predicates stats" << std::endl ;
                std::cerr << "   total calls = " << total_pred_count << std::endl ;
                std::cerr << "   calls that required dynamic filtering = " 
                          << dynfilt_pred_count << std::endl ;
                std::cerr << "   calls that required MP_Float = " << exact_pred_count << std::endl ;
                if(dynfilt_pred_count != 0) {
                    std::cerr << "dynamic filtering % = " 
                              << double(dynfilt_pred_count) * 100.0 / double(total_pred_count) 
                              << std::endl ;
                }
                if(exact_pred_count != 0) {
                    std::cerr << "MP_Float % = " 
                              << double(exact_pred_count) * 100.0 / double(total_pred_count) 
                              << std::endl ;
                }
            }
        }

        inline K::Comparison_result mult(K::Comparison_result s1, K::Comparison_result s2) {
            if(s1 == CGAL::EQUAL || s2 == CGAL::EQUAL) { return CGAL::EQUAL ; }
            return (s1 == s2 ? CGAL::LARGER : CGAL::SMALLER) ;
        }

        inline ::Geex::Sign gx_sign(K::Comparison_result r) {
            return ::Geex::Sign(r) ;
        }


        // ======================= side1 =======================
        //
        // side of q relative to the bisector of [p1,p2]
        // Positive side (resp. negative) is p1's side (resp. p2)
        // Configuration 1: q is a vertex of the boundary

        // Non-optimized version, not used, kept here for reference
        template <class RT> inline RT side1_generic_noopt (
            const RT& p1x, const RT& p1y, const RT& p1z,
            const RT& p2x, const RT& p2y, const RT& p2z,
            const RT& qx,  const RT& qy,  const RT& qz
        ) {
            static RT const two(2) ;

            const RT v1x = p1x - p2x ;
            const RT v1y = p1y - p2y ;
            const RT v1z = p1z - p2z ;
            
            const RT v2x = two*qx - (p1x + p2x) ;
            const RT v2y = two*qy - (p1y + p2y) ;
            const RT v2z = two*qz - (p1z + p2z) ;
            
            return v1x*v2x + v1y*v2y + v1z*v2z ;
        }

        // Optimized version
        template <class RT> inline RT side1_generic(
            const RT& p1x, const RT& p1y, const RT& p1z,
            const RT& p2x, const RT& p2y, const RT& p2z,
            const RT& qx,  const RT& qy,  const RT& qz
        ) {
            static RT const two(2) ;

            return (p1x-p2x)*(two*qx-p1x-p2x)+(p1y-p2y)*(two*qy-p1y-p2y)+(p1z-p2z)*(two*qz-p1z-p2z);

        }
        
        inline Comparison_result side1_filtered(
            double p1x, double p1y, double p1z,
            double p2x, double p2y, double p2z,
            double qx,  double qy,  double qz
        ) {
            // Filtered version
            {
                ::CGAL::Protect_FPU_rounding<true> p ;
                FK::Comparison_result res = sign(
                    side1_generic(
                        c2f(p1x), c2f(p1y), c2f(p1z),
                        c2f(p2x), c2f(p2y), c2f(p2z),
                        c2f(qx),  c2f(qy),  c2f(qz)
                    ) 
                ) ;
                if(is_certain(res)) {
                    return get_certain(res) ;
                }
            }
            // Exact version (if filtered version failed)
            exact_pred_count++ ;
            ::CGAL::Protect_FPU_rounding<false> p(CGAL_FE_TONEAREST) ;
            return sign(
                side1_generic(
                    c2e(p1x), c2e(p1y), c2e(p1z),
                    c2e(p2x), c2e(p2y), c2e(p2z),
                    c2e(qx),  c2e(qy),  c2e(qz)
                ) 
            ) ;
        }

#ifndef NO_STATIC_FILTERS
        inline Comparison_result side1_static_filtered(
            double p1x, double p1y, double p1z,
            double p2x, double p2y, double p2z,
            double qx,  double qy,  double qz
        ) {
            ::CGAL::Protect_FPU_rounding<false> p(CGAL_FE_TONEAREST) ;
            int result = ::RVD_CGAL_SF::side1(
                p1x, p1y, p1z,
                p2x, p2y, p2z,
                qx,  qy,  qz
            ) ;
            
            if(result == FPG_UNCERTAIN_VALUE) {
                dynfilt_pred_count++ ;
                return side1_filtered(
                    p1x, p1y, p1z,
                    p2x, p2y, p2z,
                    qx,  qy,  qz
                ) ;
            }
            return Comparison_result(result) ;
        }
#endif

        inline Comparison_result side1_inexact(
            double p1x, double p1y, double p1z,
            double p2x, double p2y, double p2z,
            double qx,  double qy,  double qz
        ) {
            return CGAL::sign(
                side1_generic(
                    p1x, p1y, p1z,
                    p2x, p2y, p2z,
                    qx,  qy,  qz
                ) 
            ) ;
        }


        // ======================= side2 =======================
        //
        // side of q relative to the bisector of [p1,p2]
        // Positive side (resp. negative) is p1's side (resp. p2)
        // Configuration 2: q is the intersection between 
        //    a bisector and an edge of the boundary
        // q = bisect(p1,p3) /\ [q1,q2]

        // Non-optimized version, not used, kept here for reference
        template <class RT> inline void side2_generic_noopt(
            RT& result,
            RT& denom,
            const RT& p1x, const RT& p1y, const RT& p1z,
            const RT& p2x, const RT& p2y, const RT& p2z,
            const RT& p3x, const RT& p3y, const RT& p3z,
            const RT& q1x, const RT& q1y, const RT& q1z,
            const RT& q2x, const RT& q2y, const RT& q2z
        ) {
            static RT const two(2) ;

            const RT nx = p1x - p3x ;
            const RT ny = p1y - p3y ;
            const RT nz = p1z - p3z ;
            
            const RT vx = q2x - q1x ;
            const RT vy = q2y - q1y ;
            const RT vz = q2z - q1z ;

            const RT num = (p1x + p3x - two * q1x) * nx +
                     (p1y + p3y - two * q1y) * ny +
                     (p1z + p3z - two * q1z) * nz ;

            denom = two * (vx*nx + vy*ny + vz*nz) ;

            const RT qx = denom * q1x + num*vx ;
            const RT qy = denom * q1y + num*vy ;
            const RT qz = denom * q1z + num*vz ;

            // side test.
                
            const RT v1x = p1x - p2x ;
            const RT v1y = p1y - p2y ;
            const RT v1z = p1z - p2z ;

            const RT v2x = two * qx - denom * (p1x + p2x) ;
            const RT v2y = two * qy - denom * (p1y + p2y) ;
            const RT v2z = two * qz - denom * (p1z + p2z) ;

            result = v1x*v2x + v1y*v2y + v1z*v2z ;
        }

        // Optimized version
        template <class RT> inline void side2_generic(
            RT& result,
            RT& denom,
            const RT& p1x, const RT& p1y, const RT& p1z,
            const RT& p2x, const RT& p2y, const RT& p2z,
            const RT& p3x, const RT& p3y, const RT& p3z,
            const RT& q1x, const RT& q1y, const RT& q1z,
            const RT& q2x, const RT& q2y, const RT& q2z
        ) {
            static RT const two(2) ;
            const RT t17 = q2z-q1z;
            const RT t18 = q2y-q1y;
            const RT t19 = q2x-q1x;
            const RT t20 = p1z-p3z;
            const RT t21 = p1y-p3y;
            const RT t22 = p1x-p3x;
            denom = (t19*t22+t18*t21+t17*t20);
            const RT t28 = p1x-p2x;
            const RT t27 = p1x-two*q1x;
            const RT t26 = p1y-p2y;
            const RT t25 = p1y-two*q1y;
            const RT t24 = p1z-p2z;
            const RT t23 = p1z-two*q1z;
            result = 
                (t28*t19+t26*t18+t24*t17) * 
                ( (p3x+t27)*t22+(p3y+t25)*t21 + (p3z+t23)*t20 ) +
                ( t28 * (-p2x-t27) + t26*(-p2y-t25)+t24*(-p2z-t23) ) * denom;
        }

        inline Comparison_result side2_filtered(
            double p1x, double p1y, double p1z,
            double p2x, double p2y, double p2z,
            double p3x, double p3y, double p3z,
            double q1x, double q1y, double q1z,
            double q2x, double q2y, double q2z
        ) {
            // Filtered version
            {
                ::CGAL::Protect_FPU_rounding<true> p ;
                FK::FT result ;
                FK::FT denom ;
                side2_generic(
                    result, denom,
                    c2f(p1x), c2f(p1y), c2f(p1z),
                    c2f(p2x), c2f(p2y), c2f(p2z),
                    c2f(p3x), c2f(p3y), c2f(p3z),
                    c2f(q1x), c2f(q1y), c2f(q1z),
                    c2f(q2x), c2f(q2y), c2f(q2z)
                ) ;
                FK::Comparison_result s1 = sign(result) ;
                FK::Comparison_result s2 = sign(denom) ;
                
                if(is_certain(s1) && is_certain(s2)) {
                    return mult(get_certain(s1), get_certain(s2)) ;
                }
            }
            // Exact version (if filtered version failed)
            exact_pred_count++ ;
            ::CGAL::Protect_FPU_rounding<false> p(CGAL_FE_TONEAREST) ;
            EK::FT result ;
            EK::FT denom ;
            side2_generic(
                result, denom,
                c2e(p1x), c2e(p1y), c2e(p1z),
                c2e(p2x), c2e(p2y), c2e(p2z),
                c2e(p3x), c2e(p3y), c2e(p3z),
                c2e(q1x), c2e(q1y), c2e(q1z),
                c2e(q2x), c2e(q2y), c2e(q2z)
            ) ;
            return mult(sign(result), sign(denom)) ;
        }

#ifndef NO_STATIC_FILTERS
        inline Comparison_result side2_static_filtered(
            double p1x, double p1y, double p1z,
            double p2x, double p2y, double p2z,
            double p3x, double p3y, double p3z,
            double q1x, double q1y, double q1z,
            double q2x, double q2y, double q2z
        ) {
            int s_result, s_denom ;
            ::CGAL::Protect_FPU_rounding<false> p(CGAL_FE_TONEAREST) ;
            if(RVD_CGAL_SF::side2(
                   s_result, s_denom,
                   p1x, p1y, p1z,
                   p2x, p2y, p2z,
                   p3x, p3y, p3z,
                   q1x, q1y, q1z,
                   q2x, q2y, q2z
                ) == FPG_UNCERTAIN_VALUE) {
                dynfilt_pred_count++ ;
                return side2_filtered(
                    p1x, p1y, p1z,
                    p2x, p2y, p2z,
                    p3x, p3y, p3z,
                    q1x, q1y, q1z,
                    q2x, q2y, q2z
                ) ;
            }
            return mult(CGAL::Comparison_result(s_result), CGAL::Comparison_result(s_denom)) ;
        }
#endif

        inline Comparison_result side2_inexact(
            double p1x, double p1y, double p1z,
            double p2x, double p2y, double p2z,
            double p3x, double p3y, double p3z,
            double q1x, double q1y, double q1z,
            double q2x, double q2y, double q2z
        ) {
            double result ;
            double denom ;
            side2_generic(
                result, denom,
                p1x, p1y, p1z,
                p2x, p2y, p2z,
                p3x, p3y, p3z,
                q1x, q1y, q1z,
                q2x, q2y, q2z
            ) ;
            return mult(CGAL::sign(result), CGAL::sign(denom)) ;
        }


        // ======================= side3 =======================
        //
        // side of q relative to the bisector of [p1,p2]
        // Positive side (resp. negative) is p1's side (resp. p2)
        // Configuration 3: q is the intersection between 
        //    two bisectors and a facet of the boundary
        // q = bisect(p1,p3) /\ bisect(p1,p3) /\ plane(q1,q2,q3)

        // Non-optimized version, not used, kept here for reference.
        template <class RT> inline void side3_generic_noopt(
            RT& result,
            RT& denom,
            const RT& p1x, const RT& p1y, const RT& p1z,
            const RT& p2x, const RT& p2y, const RT& p2z,
            const RT& p3x, const RT& p3y, const RT& p3z,
            const RT& p4x, const RT& p4y, const RT& p4z,
            const RT& q1x, const RT& q1y, const RT& q1z,
            const RT& q2x, const RT& q2y, const RT& q2z,
            const RT& q3x, const RT& q3y, const RT& q3z
        ) {
            static RT const two(2) ;

            RT N1x = p3x - p1x ;
            RT N1y = p3y - p1y ;
            RT N1z = p3z - p1z ;

            const RT d1 = -(N1x*(p1x + p3x) + N1y*(p1y + p3y) + N1z*(p1z + p3z)) ;
            N1x *= two ;
            N1y *= two ;
            N1z *= two ;
            
            RT N2x = p4x - p1x ;
            RT N2y = p4y - p1y ;
            RT N2z = p4z - p1z ;
            const RT d2 = -(N2x*(p1x + p4x) + N2y*(p1y + p4y) + N2z*(p1z + p4z)) ;
            N2x *= two ;
            N2y *= two ;
            N2z *= two ;

            const RT W1x = q2x - q1x ;
            const RT W1y = q2y - q1y ;
            const RT W1z = q2z - q1z ;
            const RT W2x = q3x - q1x ;
            const RT W2y = q3y - q1y ;
            const RT W2z = q3z - q1z ;
            const RT N3x = W1y*W2z - W1z*W2y ;
            const RT N3y = W1z*W2x - W1x*W2z ;
            const RT N3z = W1x*W2y - W1y*W2x ;
            const RT d3 = -(N3x * q1x + N3y * q1y + N3z * q1z) ;

            RT b00, b01, b02 ;
            RT b10, b11, b12 ;
            RT b20, b21, b22 ;
            // Note: b is transposed
            comatrix3x3(
                    N1x, N1y, N1z,
                    N2x, N2y, N2z,
                    N3x, N3y, N3z,
                    b00, b10, b20,
                    b01, b11, b21,
                    b02, b12, b22
            ) ;

            // Reuses the minors of the co-matrix to compute the determinant.
            denom = N1x*b00 + N1y*b10 + N1z*b20;
                
            const RT qx = -(d1 * b00 + d2 * b01 + d3 * b02) ;
            const RT qy = -(d1 * b10 + d2 * b11 + d3 * b12) ;
            const RT qz = -(d1 * b20 + d2 * b21 + d3 * b22) ;
            
            // side test.
            const RT v1x = p1x - p2x ;
            const RT v1y = p1y - p2y ;
            const RT v1z = p1z - p2z ;

            const RT v2x = two * qx - denom * (p1x + p2x) ;
            const RT v2y = two * qy - denom * (p1y + p2y) ;
            const RT v2z = two * qz - denom * (p1z + p2z) ;

            result = v1x*v2x + v1y*v2y + v1z*v2z ;
        }

        // Optimized version.
        template <class RT> inline void side3_generic(
            RT& result,
            RT& denom,
            const RT& p1x, const RT& p1y, const RT& p1z,
            const RT& p2x, const RT& p2y, const RT& p2z,
            const RT& p3x, const RT& p3y, const RT& p3z,
            const RT& p4x, const RT& p4y, const RT& p4z,
            const RT& q1x, const RT& q1y, const RT& q1z,
            const RT& q2x, const RT& q2y, const RT& q2z,
            const RT& q3x, const RT& q3y, const RT& q3z
        ) {
            static RT const two(2) ;

            const RT t46 = p4z-p1z;
            const RT t47 = p4y-p1y;
            const RT t48 = p4x-p1x;
            const RT t55 = t48*(p1x+p4x)+t47*(p1y+p4y)+t46*(p1z+p4z);
            const RT t49 = -p1z+p3z;
            const RT t50 = -p1y+p3y;
            const RT t51 = -p1x+p3x;
            const RT t54 = t51*(p1x+p3x)+t50*(p1y+p3y)+t49*(p1z+p3z);
            const RT t40 = q3z-q1z;
            const RT t42 = q3x-q1x;
            const RT t43 = q2z-q1z;
            const RT t45 = q2x-q1x;
            const RT t37 = t43*t42-t45*t40;
            const RT t41 = q3y-q1y;
            const RT t44 = q2y-q1y;
            const RT t38 = t44*t40-t43*t41;
            const RT t39 = t45*t41-t44*t42;
            const RT t53 = two*(t38*q1x+t37*q1y+t39*q1z);
            const RT t34 = t48*t37-t47*t38;
            const RT t33 = t47*t39-t46*t37;
            const RT t32 = t46*t38-t48*t39;
            denom = t51*t33+t50*t32+t49*t34;
            result = 
                (p1x-p2x)*(t33*t54+(t49*t37-t50*t39)*t55+(t50*t46-t49*t47)*t53-denom*(p1x+p2x))+
                (p1y-p2y)*(t32*t54+(t51*t39-t49*t38)*t55+(t49*t48-t51*t46)*t53-denom*(p1y+p2y))+
                (p1z-p2z)*(t34*t54+(t50*t38-t51*t37)*t55+(t51*t47-t50*t48)*t53-denom*(p1z+p2z));

        }


        inline Comparison_result side3_filtered(
            double p1x, double p1y, double p1z,
            double p2x, double p2y, double p2z,
            double p3x, double p3y, double p3z,
            double p4x, double p4y, double p4z,
            double q1x, double q1y, double q1z,
            double q2x, double q2y, double q2z,
            double q3x, double q3y, double q3z
        ) {
            // Filtered version
            {
                ::CGAL::Protect_FPU_rounding<true> p ;
                FK::FT result ;
                FK::FT denom ;
                side3_generic(
                    result, denom,
                    c2f(p1x), c2f(p1y), c2f(p1z),
                    c2f(p2x), c2f(p2y), c2f(p2z),
                    c2f(p3x), c2f(p3y), c2f(p3z),
                    c2f(p4x), c2f(p4y), c2f(p4z),
                    c2f(q1x), c2f(q1y), c2f(q1z),
                    c2f(q2x), c2f(q2y), c2f(q2z),
                    c2f(q3x), c2f(q3y), c2f(q3z)
                ) ;
                FK::Comparison_result s1 = sign(result) ;
                FK::Comparison_result s2 = sign(denom) ;
                
                if(is_certain(s1) && is_certain(s2)) {
                    return mult(get_certain(s1), get_certain(s2)) ;
                }
            }
            // Exact version (if filtered version failed)
            exact_pred_count++ ;
            ::CGAL::Protect_FPU_rounding<false> p(CGAL_FE_TONEAREST) ;
            EK::FT result ;
            EK::FT denom ;
            side3_generic(
                result, denom,
                c2e(p1x), c2e(p1y), c2e(p1z),
                c2e(p2x), c2e(p2y), c2e(p2z),
                c2e(p3x), c2e(p3y), c2e(p3z),
                c2e(p4x), c2e(p4y), c2e(p4z),
                c2e(q1x), c2e(q1y), c2e(q1z),
                c2e(q2x), c2e(q2y), c2e(q2z),
                c2e(q3x), c2e(q3y), c2e(q3z)
            ) ;
            return mult(sign(result), sign(denom)) ;
        }

#ifndef NO_STATIC_FILTERS
        inline Comparison_result side3_static_filtered(
            double p1x, double p1y, double p1z,
            double p2x, double p2y, double p2z,
            double p3x, double p3y, double p3z,
            double p4x, double p4y, double p4z,
            double q1x, double q1y, double q1z,
            double q2x, double q2y, double q2z,
            double q3x, double q3y, double q3z
        ) {
            int s_result, s_denom ;
            ::CGAL::Protect_FPU_rounding<false> p(CGAL_FE_TONEAREST) ;
            if(RVD_CGAL_SF::side3(
                   s_result, s_denom,
                   p1x, p1y, p1z,
                   p2x, p2y, p2z,
                   p3x, p3y, p3z,
                   p4x, p4y, p4z,
                   q1x, q1y, q1z,
                   q2x, q2y, q2z,
                   q3x, q3y, q3z
                ) == FPG_UNCERTAIN_VALUE) {
                dynfilt_pred_count++ ;
                return side3_filtered(
                   p1x, p1y, p1z,
                   p2x, p2y, p2z,
                   p3x, p3y, p3z,
                   p4x, p4y, p4z,
                   q1x, q1y, q1z,
                   q2x, q2y, q2z,
                   q3x, q3y, q3z
                ) ;
            }
            return mult(CGAL::Comparison_result(s_result), CGAL::Comparison_result(s_denom)) ;
        }
#endif

        inline Comparison_result side3_inexact(
            double p1x, double p1y, double p1z,
            double p2x, double p2y, double p2z,
            double p3x, double p3y, double p3z,
            double p4x, double p4y, double p4z,
            double q1x, double q1y, double q1z,
            double q2x, double q2y, double q2z,
            double q3x, double q3y, double q3z
        ) {
            double result ;
            double denom ;
            side3_generic(
                result, denom,
                p1x, p1y, p1z,
                p2x, p2y, p2z,
                p3x, p3y, p3z,
                p4x, p4y, p4z,
                q1x, q1y, q1z,
                q2x, q2y, q2z,
                q3x, q3y, q3z
            ) ;
            return mult(CGAL::sign(result), CGAL::sign(denom)) ;
        }

    }

    bool RVD_predicates::find_edge(
        const TopoPolyMesh* M, int f1, int f2, vec3& q1, vec3& q2
    ) {
        for(unsigned int i=M->facet_begin(f1); i<M->facet_end(f1); i++) {
            if(M->vertex(i).f == f2) {
                q1 = M->vertex(i) ;
                unsigned int j = i+1 ;
                if(j == M->facet_end(f1)) {
                    j = M->facet_begin(f1) ;
                }
                q2 = M->vertex(j) ;
                return true ; 
            }
        }
//        std::cerr << "Edge does not exist: " << f1 << " " << f2 << std::endl ;
        return false ;
//        std::cerr << "Nb facets = " << M->nb_facets() << std::endl ;
//        gx_assert_not_reached ;
    }



#ifdef NO_STATIC_FILTERS
#define PREDICATE(x) x##_filtered
#else
#define PREDICATE(x) x##_static_filtered
#endif

//#define PREDICATE(x) x##_inexact        
//#define PREDICATE(x) x##_filtered
//#define PREDICATE(x) x##_static_filtered

    Sign RVD_predicates::side(
        const vec3& p1, const vec3& p2,
        const vec3Sym& v,
        const RestrictedVoronoiDiagram_poly* RVD
    ) {
//        if(v.sym.nb_bisectors() + v.sym.nb_boundary_facets() != 3) {
//            std::cerr << "Problem with sym: " << v.sym << std::endl ;
//        }
//        gx_debug_assert(v.sym.nb_bisectors() + v.sym.nb_boundary_facets() == 3) ;

        RVD_CGAL_P::total_pred_count++ ;

        switch(v.sym.nb_bisectors()) {
        case 0: {
            return RVD_CGAL_P::gx_sign(
                RVD_CGAL_P::PREDICATE(side1)(
                    p1.x, p1.y, p1.z, p2.x, p2.y, p2.z,
                    v.x, v.y, v.z
                ) 
            ) ;
        } break ;
        case 1: {
            unsigned int nbb = v.sym.nb_boundary_facets() ;
            vec3 p3 = RVD->delaunay()->vertex(v.sym.bisector(0)) ;
            vec3 q1,q2 ;
            bool foundb = 0 ;
            for(unsigned int ib=0; ib<nbb; ib++) {
                for(unsigned int jb=0; jb<nbb; jb++) {
                    if(!foundb && ib != jb && find_edge(
                           RVD->mesh(), v.sym.boundary_facet(ib), v.sym.boundary_facet(jb), q1, q2
                        )
                    ) {
                        foundb = true ;
                        break ;
                    }
                }
            }
            gx_debug_assert(foundb) ;

            return RVD_CGAL_P::gx_sign(
                RVD_CGAL_P::PREDICATE(side2)(
                    p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z,
                    q1.x, q1.y, q1.z, q2.x, q2.y, q2.z
                ) 
            ) ;
        } break ;
        default: {
            vec3 p3 = RVD->delaunay()->vertex(v.sym.bisector(0)) ;
            vec3 p4 = RVD->delaunay()->vertex(v.sym.bisector(1)) ;
            int f = v.sym.boundary_facet(0) ;
            const TopoPolyMesh* M = RVD->mesh() ;
            const vec3& q1 = M->vertex(M->facet_begin(f)) ;
            const vec3& q2 = M->vertex(M->facet_begin(f)+1) ;
            const vec3& q3 = M->vertex(M->facet_begin(f)+2) ;
            return RVD_CGAL_P::gx_sign(
                RVD_CGAL_P::PREDICATE(side3)(
                    p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, 
                    p3.x, p3.y, p3.z, p4.x, p4.y, p4.z,
                    q1.x, q1.y, q1.z, q2.x, q2.y, q2.z,
                    q3.x, q3.y, q3.z
                ) 
            ) ;
        } break ;
        }
        gx_assert_not_reached ;
        return ZERO ;
    }


    void RVD_predicates::begin_stats() {
        RVD_CGAL_P::reset_stats() ;
    }

    void RVD_predicates::end_stats() {
        RVD_CGAL_P::show_stats() ;
    }
    
    void RVD_predicates::set_verbose(bool x) {
        RVD_CGAL_P::verbose = x ;
    }

    vec3 RVD_predicates::sym_to_geometry(
        unsigned int center_vertex, const vec3Sym& v,
        const RestrictedVoronoiDiagram_poly* RVD
    ) {
        vec3 p1 = RVD->delaunay()->vertex(center_vertex) ;
        switch(v.sym.nb_bisectors()) {
        case 0: {
            return v ;
        } break ;
        case 1: {
            unsigned int nbb = v.sym.nb_boundary_facets() ;
            vec3 p3 = RVD->delaunay()->vertex(v.sym.bisector(0)) ;
            vec3 q1,q2 ;
            bool foundb = 0 ;
            for(unsigned int ib=0; ib<nbb; ib++) {
                for(unsigned int jb=0; jb<nbb; jb++) {
                    if(!foundb && ib != jb && find_edge(
                           RVD->mesh(), v.sym.boundary_facet(ib), v.sym.boundary_facet(jb), q1, q2
                        )
                    ) {
                        foundb = true ;
                        break ;
                    }
                }
            }
            gx_debug_assert(foundb) ;

            return seg_plane_intersection(
                q1, q2,
                median_plane(p1, p3)
            ) ;
        } break ;
        case 2: {
            vec3 p3 = RVD->delaunay()->vertex(v.sym.bisector(0)) ;
            vec3 p4 = RVD->delaunay()->vertex(v.sym.bisector(1)) ;
            int f = v.sym.boundary_facet(0) ;
            const TopoPolyMesh* M = RVD->mesh() ;
            const vec3& q1 = M->vertex(M->facet_begin(f)) ;
            const vec3& q2 = M->vertex(M->facet_begin(f)+1) ;
            const vec3& q3 = M->vertex(M->facet_begin(f)+2) ;
            return three_planes_intersection(
                median_plane(p1, p3),
                median_plane(p1, p4),
                Plane<real>(q1, cross(q2-q1, q3-q1))
            ) ;
        } break ;
        default: {
            vec3 p3 = RVD->delaunay()->vertex(v.sym.bisector(0)) ;
            vec3 p4 = RVD->delaunay()->vertex(v.sym.bisector(1)) ;
            vec3 p5 = RVD->delaunay()->vertex(v.sym.bisector(2)) ;
            return three_planes_intersection(
                median_plane(p1, p3),
                median_plane(p1, p4),
                median_plane(p1, p5)
            ) ;
        }
        }
        gx_assert_not_reached ;
        return vec3(0,0,0) ;
    }

}
