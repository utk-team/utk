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
 
 
 
 

#ifndef __Geex_IMAGE_TYPES_TYPES__
#define __Geex_IMAGE_TYPES_TYPES__

//#include <OGF/image/common/common.h>
//#include <OGF/image/types/color.h>
//#include <OGF/basic/types/types.h>
#include <Geex/image/color.h>
#include <Geex/basics/types.h>

namespace Geex {

//_________________________________________________________


    class GEEX_API ImageTypes {

    public:

        //_______________________ Colors
        
        typedef GenColor<Numeric::int8> Color_int8 ;
        typedef GenColor<Numeric::uint8> Color_uint8 ;
        typedef GenColor<Numeric::int16> Color_int16 ;
        typedef GenColor<Numeric::uint16> Color_uint16 ;
        typedef GenColor<Numeric::int32> Color_int32 ;
        typedef GenColor<Numeric::uint32> Color_uint32 ;
        typedef GenColor<Numeric::float32> Color_float32 ;
        typedef GenColor<Numeric::float64> Color_float64 ;
    } ;

    typedef ImageTypes::Color_float64 Color ;

//_________________________________________________________

}
#endif

