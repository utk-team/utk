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
 
 
 
 
 
 
 

#include <Geex/properties/property_interpolator.h>

namespace Geex {

    PropertyInterpolator::InterpolatorMap* PropertyInterpolator::type_to_interpolator_ = nil ;

    PropertyInterpolator::~PropertyInterpolator() {
    }

    void PropertyInterpolator::initialize() {
        gx_assert(type_to_interpolator_ == nil) ;
        type_to_interpolator_ = new InterpolatorMap ;
    }
    
    void PropertyInterpolator::terminate() {
        delete type_to_interpolator_ ;
        type_to_interpolator_ = nil ;
    }
    
    PropertyInterpolator* PropertyInterpolator::resolve_by_type(const std::type_info& property_type) {
        gx_assert(type_to_interpolator_ != nil) ;
        InterpolatorMap::iterator it = type_to_interpolator_->find(property_type.name()) ;
        if(it == type_to_interpolator_->end()) {
            return nil ;
        }
        return it->second ;
    }

    void PropertyInterpolator::bind(
        const std::type_info& property_type, 
        PropertyInterpolator* interpolator
    ) {
        gx_assert(resolve_by_type(property_type) == nil) ;
        (*type_to_interpolator_)[property_type.name()] = interpolator ;
    }
    

    void PropertyInterpolator::interpolate_2(
        Memory::pointer to, 
        double a1, Memory::pointer x1, 
        double a2, Memory::pointer x2
    ) {
        interpolate_begin(to) ;
        interpolate_add(a1, x1) ;
        interpolate_add(a2, x2) ;
        interpolate_end() ;
    }
    
    void PropertyInterpolator::interpolate_3(
        Memory::pointer to, 
        double a1, Memory::pointer x1, 
        double a2, Memory::pointer x2, 
        double a3, Memory::pointer x3
    ) {
        interpolate_begin(to) ;
        interpolate_add(a1, x1) ;
        interpolate_add(a2, x2) ;
        interpolate_add(a3, x3) ;
        interpolate_end() ;
    }

    void PropertyInterpolator::interpolate_4(
        Memory::pointer to, 
        double a1, Memory::pointer x1, 
        double a2, Memory::pointer x2, 
        double a3, Memory::pointer x3,
        double a4, Memory::pointer x4
    ) {
        interpolate_begin(to) ;
        interpolate_add(a1, x1) ;
        interpolate_add(a2, x2) ;
        interpolate_add(a3, x3) ;
        interpolate_add(a4, x4) ;
        interpolate_end() ;
    }
    

}
