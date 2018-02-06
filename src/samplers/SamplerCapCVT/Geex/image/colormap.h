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
 
 
 
 

#ifndef __GEEX_IMAGE_TYPES_COLORMAP__
#define __GEEX_IMAGE_TYPES_COLORMAP__

//#include <OGF/image/common/common.h>
//#include <OGF/image/types/types.h>
#include <Geex/basics/counted.h>
#include <Geex/image/types.h>
#include <Geex/basics/smart_pointer.h>

namespace Geex {


//_________________________________________________________

    class GEEX_API Colormap : public Counted {
    public:
        typedef ImageTypes::Color_uint8 ColorCell ;
        Colormap(int size_in = 256) ;
        virtual ~Colormap() ;

        const ColorCell& color_cell(int index) const ;
        ColorCell& color_cell(int index) ;
    
        int size() const ;

        void set_color(int index, float r, float g, float b) ;

        void set_color(int index, float r, float g, float b, float a) ;

        void color_ramp_component(
            int component,
            int index1, Numeric::uint8 val1,
            int index2, Numeric::uint8 val2
        ) ;

        void color_ramp_rgba(
            int index1, const Color& c1,
            int index2, const Color& c2
        ) ;

        void color_ramp_rgb(
            int index1, const Color& c1,
            int index2, const Color& c2
        ) ;

    private:
        ColorCell* cells_ ;
        int size_ ;
    } ;

    typedef SmartPointer<Colormap> Colormap_var ;

//_________________________________________________________

    inline const Colormap::ColorCell& Colormap::color_cell(
        int index
    ) const {
        gx_assert(index >= 0 && index < size_) ;
        return cells_[index] ;
    }

    inline Colormap::ColorCell& Colormap::color_cell(int index) {
        gx_assert(index >= 0 && index < size_) ;
        return cells_[index] ;
    }

    inline int Colormap::size() const {
        return size_ ;
    }

//_________________________________________________________

}
#endif

