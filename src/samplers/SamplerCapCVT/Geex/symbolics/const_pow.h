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


#ifndef __GEEX_SYMBOLICS_CONST_POW__
#define __GEEX_SYMBOLICS_CONST_POW__

namespace Geex {


    // Rem: I do not know from which exponent using pow() is faster
    // than multiplications, to be tested and tuned ...

    template<int NUM, int DENOM> inline double const_pow(double x) {
        return ::pow(x, double(NUM) / double(DENOM)) ;
    }

    //-----------------------------

    template<> inline double const_pow<1,1>(double x) {
        return x ;
    }

    template<> inline double const_pow<1,2>(double x) {
        return sqrt(x) ;
    }

    template<> inline double const_pow<2,1>(double x) {
        return x*x ;
    }

    template<> inline double const_pow<3,1>(double x) {
        return x*x*x ;
    }

    template<> inline double const_pow<4,1>(double x) {
        const double t = x*x ;
        return t*t ;
    }

    template<> inline double const_pow<5,1>(double x) {
        const double t = x*x ;
        return t*t*x ;
    }

    template<> inline double const_pow<6,1>(double x) {
        const double t = x*x ;
        return t*t*t ;
    }

    template<> inline double const_pow<7,1>(double x) {
        const double t = x*x ;
        return t*t*t*x ;
    }

    template<> inline double const_pow<8,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return t2*t2 ;
    }

    template<> inline double const_pow<9,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return t2*t2*x ;
    }

    template<> inline double const_pow<10,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return t2*t2*t1 ;
    }

    template<> inline double const_pow<11,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return t2*t2*t1*x ;
    }

    template<> inline double const_pow<12,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return t2*t2*t2 ;
    }

    template<> inline double const_pow<13,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return t2*t2*t2*x ;
    }

    template<> inline double const_pow<14,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return t2*t2*t2*t1 ;
    }

    template<> inline double const_pow<15,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return t2*t2*t2*t1*x ;
    }

    template<> inline double const_pow<16,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        const double t3 = t2*t2 ;
        return t3*t3 ;
    }

    //-----------------------------

    template<> inline double const_pow<-1,1>(double x) {
        return 1.0/x ;
    }

    template<> inline double const_pow<-1,2>(double x) {
        return 1.0/sqrt(x) ;
    }

    template<> inline double const_pow<-2,1>(double x) {
        return 1.0/(x*x) ;
    }

    template<> inline double const_pow<-3,1>(double x) {
        return 1.0/(x*x*x) ;
    }

    template<> inline double const_pow<-4,1>(double x) {
        const double t = x*x ;
        return 1.0/(t*t) ;
    }

    template<> inline double const_pow<-5,1>(double x) {
        const double t = x*x ;
        return 1.0/(t*t*x) ;
    }

    template<> inline double const_pow<-6,1>(double x) {
        const double t = x*x ;
        return 1.0/(t*t*t) ;
    }

    template<> inline double const_pow<-7,1>(double x) {
        const double t = x*x ;
        return 1.0/(t*t*t*x) ;
    }

    template<> inline double const_pow<-8,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return 1.0/(t2*t2) ;
    }

    template<> inline double const_pow<-9,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return 1.0/(t2*t2*x) ;
    }

    template<> inline double const_pow<-10,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return 1.0/(t2*t2*t1) ;
    }

    template<> inline double const_pow<-11,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return 1.0/(t2*t2*t1*x) ;
    }

    template<> inline double const_pow<-12,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return 1.0/(t2*t2*t2) ;
    }

    template<> inline double const_pow<-13,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return 1.0/(t2*t2*t2*x) ;
    }

    template<> inline double const_pow<-14,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return 1.0/(t2*t2*t2*t1) ;
    }

    template<> inline double const_pow<-15,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        return 1.0/(t2*t2*t2*t1*x) ;
    }

    template<> inline double const_pow<-16,1>(double x) {
        const double t1 = x*x ;
        const double t2 = t1*t1 ;
        const double t3 = t2*t2 ;
        return 1.0/(t3*t3) ;
    }

    //-----------------------------


}

#endif
