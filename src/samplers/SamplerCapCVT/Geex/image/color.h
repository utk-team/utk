/*
 *  OGF/Graphite: Geometry and Graphics Programming Library + Utilities
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
 

#ifndef __GEEX_IMAGE_TYPES_COLOR__
#define __GEEX_IMAGE_TYPES_COLOR__

//#include <OGF/image/common/common.h>
//#include <OGF/basic/debug/assert.h>
#include <Geex/basics/assert.h>
#include <iostream>

namespace Geex {

//_________________________________________________________

    template <class RT> class GenColor {
    public:
        GenColor(RT r=0, RT g=0, RT b=0, RT a = 0) ;
        RT r() const ;
        RT g() const ;
        RT b() const ;
        RT a() const ;
        void set_r(RT r) ;
        void set_g(RT g) ;
        void set_b(RT b) ;
        void set_a(RT a) ;
        RT& operator[](int i) ;
        const RT& operator[](int i) const ;

    private:
        RT components_[4] ;
    } ;

//_________________________________________________________

    template <class RT> inline
    GenColor<RT>::GenColor(RT r, RT g, RT b, RT a) {
        components_[0] = r ;
        components_[1] = g ;
        components_[2] = b ;
        components_[3] = a ;
    }

    template <class RT> inline
    RT GenColor<RT>::r() const {
        return components_[0] ;
    }

    template <class RT> inline
    RT GenColor<RT>::g() const {
        return components_[1] ;
    }

    template <class RT> inline
    RT GenColor<RT>::b() const {
        return components_[2] ;
    }

    template <class RT> inline
    RT GenColor<RT>::a() const {
        return components_[3] ;
    }

    template <class RT> inline
    void GenColor<RT>::set_r(RT r) {
        components_[0] = r ;
    }

    template <class RT> inline
    void GenColor<RT>::set_g(RT g) {
        components_[1] = g ;
    }

    template <class RT> inline
    void GenColor<RT>::set_b(RT b) {
        components_[2] = b ;
    }

    template <class RT> inline
    void GenColor<RT>::set_a(RT a) {
        components_[3] = a ;
    }

    template <class RT> inline
    RT& GenColor<RT>::operator[](int i) {
        gx_assert(i >= 0 && i <= 3) ;
        return components_[i] ;
    }

    template <class RT> inline
    const RT& GenColor<RT>::operator[](int i) const {
        gx_assert(i >= 0 && i <= 3) ;
        return components_[i] ;
    }


    template <class RT> inline
    std::ostream& operator<<(std::ostream& output, const GenColor<RT>& color) {
        return output << 
            color[0] << " " << color[1] << " " << color[2] << " " << color[3] ;
    }

    template <class RT> inline
    std::istream& operator>>(std::istream& input, GenColor<RT>& color) {
        return input >> color[0] >> color[1] >> color[2] >> color[3] ;
    }

//_________________________________________________________

}

#endif
