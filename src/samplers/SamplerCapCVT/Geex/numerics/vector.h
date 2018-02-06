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
 
 
 
 
 
 
 

#ifndef __GEEX_NUMERICS_VECTOR__
#define __GEEX_NUMERICS_VECTOR__

#include <Geex/basics/arrays.h>

namespace Geex {

    class Vector : public Array1d<double> {
    public:
        typedef Array1d<double> superclass ;
    public:
        void zero() { Memory::clear(superclass::data(), superclass::size() * sizeof(double)); } 
        Vector() { }
        Vector(size_t n, size_t alignment = 1) : superclass(n, alignment) { zero(); }
        Vector(const Vector& rhs) {
            allocate(rhs.size(), rhs.alignment()) ;
            Memory::copy(superclass::data(), rhs.data(), superclass::size() * sizeof(double)) ;
        }
        void allocate(size_t n, size_t alignment=1) { superclass::allocate(n, alignment); zero() ; } 
        void operator += (const Vector& rhs) {
            gx_debug_assert(rhs.size() == superclass::size()) ;
            for(index_t i=0; i<superclass::size(); i++) {
                (*this)(i) += rhs(i) ;
            }
        }
        void operator -= (const Vector& rhs) {
            gx_debug_assert(rhs.size() == superclass::size()) ;
            for(index_t i=0; i<superclass::size(); i++) {
                (*this)(i) -= rhs(i) ;
            }
        }
        Vector& operator=(const Vector& rhs) {
            gx_debug_assert(rhs.size() == superclass::size()) ;
            Memory::copy(superclass::data(), rhs.data(), superclass::size() * sizeof(double)) ;
            return *this ;
        }
    } ;
} 

#endif
