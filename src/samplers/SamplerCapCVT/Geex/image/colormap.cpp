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
 
 
 
 

#include <Geex/image/colormap.h>

namespace Geex {

//_________________________________________________________

    Colormap::Colormap(int size_in) : size_(size_in) {
        cells_ = new ColorCell[size_in] ;
    }

    Colormap::~Colormap() {
        delete[] cells_ ;
    }


    void Colormap::color_ramp_component(
        int component,
        int index1, Numeric::uint8 alpha1,
        int index2, Numeric::uint8 alpha2
    ) {
        if(index1 == index2) {
            color_cell(index2)[component] = alpha2 ;
        } else {
            int n = gx_abs(index2 - index1) ;
            float delta = (alpha2 - alpha1) / float(n) ;
            int sgn = gx_sgn(index2 - index1) ;
            float alpha = alpha1 ;
            int index = index1 ;
            for(int i=0 ; i<=n ; i++) {
                color_cell(index)[component] =
                    Numeric::uint8(alpha) ;
                index += sgn ;
                alpha += delta ;
            }
        }
    }


    void Colormap::color_ramp_rgba(
        int index1, const Color& c1,
        int index2, const Color& c2
    ) {
        if(index1 == index2) {
            Colormap::ColorCell c(
                Numeric::uint8(c2.r() * 255.0),
                Numeric::uint8(c2.g() * 255.0),
                Numeric::uint8(c2.b() * 255.0),
                Numeric::uint8(c2.a() * 255.0)
            ) ; 
            color_cell(index2) = c ;
        } else {
        
            int n = gx_abs(index2 - index1) ;
            int sgn = gx_sgn(index2 - index1) ;
        
            float r = static_cast<float>(c1.r()) ;
            float g = static_cast<float>(c1.g()) ;
            float b = static_cast<float>(c1.b()) ;
            float a = static_cast<float>(c1.a()) ;
            float dr = static_cast<float>((c2.r() - c1.r()) / float(n)) ;
            float dg = static_cast<float>((c2.g() - c1.g()) / float(n)) ;
            float db = static_cast<float>((c2.b() - c1.b()) / float(n)) ;
            float da = static_cast<float>((c2.a() - c1.a()) / float(n)) ;
            int index = index1 ;

            for(int i=0 ; i<=n ; i++) {
                set_color(index, r, g, b, a) ;
                index += sgn ;
                r += dr ;
                g += dg ;
                b += db ;
                a += da ;
            }
        }
    }


    void Colormap::color_ramp_rgb(
        int index1, const Color& c1,
        int index2, const Color& c2
    ) {
        if(index1 == index2) {
            Colormap::ColorCell c(
                Numeric::uint8(c2.r() * 255.0),
                Numeric::uint8(c2.g() * 255.0),
                Numeric::uint8(c2.b() * 255.0),
                color_cell(index2).a()
            ) ; 
            color_cell(index2) = c ;
        } else {
        
            int n = gx_abs(index2 - index1) ;
            int sgn = gx_sgn(index2 - index1) ;
        
            float r = static_cast<float>(c1.r()) ;
            float g = static_cast<float>(c1.g()) ;
            float b = static_cast<float>(c1.b()) ;

            float dr = static_cast<float>((c2.r() - c1.r()) / float(n)) ;
            float dg = static_cast<float>((c2.g() - c1.g()) / float(n)) ;
            float db = static_cast<float>((c2.b() - c1.b()) / float(n)) ;
            int index = index1 ;

            for(int i=0 ; i<=n ; i++) {
                set_color(index, r, g, b) ;
                index += sgn ;
                r += dr ;
                g += dg ;
                b += db ;
            }
        }
    }


    void Colormap::set_color(int index, float r, float g, float b) {
        r *= 255.0 ;
        g *= 255.0 ;
        b *= 255.0 ;
        gx_clamp(r, float(0), float(255)) ;
        gx_clamp(g, float(0), float(255)) ;
        gx_clamp(b, float(0), float(255)) ;
        Colormap::ColorCell c = Colormap::ColorCell(
            Numeric::uint8(r),
            Numeric::uint8(g),
            Numeric::uint8(b),
            color_cell(index).a()
        ) ; 
        color_cell(index) = c ;
    }

    void Colormap::set_color(int index, float r, float g, float b, float a) {
        r *= 255.0 ;
        g *= 255.0 ;
        b *= 255.0 ;
        a *= 255.0 ;
        gx_clamp(r, float(0), float(255)) ;
        gx_clamp(g, float(0), float(255)) ;
        gx_clamp(b, float(0), float(255)) ;
        gx_clamp(a, float(0), float(255)) ;
        Colormap::ColorCell c = Colormap::ColorCell(
            Numeric::uint8(r),
            Numeric::uint8(g),
            Numeric::uint8(b),
            Numeric::uint8(a)
        ) ; 
        color_cell(index) = c ;
    }


//_________________________________________________________

}

