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
 
 
 
 
 
 
 

#ifndef ___PROPERTY_INTERPOLATOR__
#define ___PROPERTY_INTERPOLATOR__

#include <Geex/basics/types.h>
#include <Geex/basics/counted.h>
#include <Geex/basics/smart_pointer.h>

#include <map>
#include <string>
#include <typeinfo>

namespace Geex {
    
    //_________________________________________________________________________________________________________

    /**
     * PropertyInterpolator is used to compute linear combinations
     * of properties attached to an object. For instance, it can be used
     * to interpolate property on a surface when generating new vertices.
     */
    class GEEX_API PropertyInterpolator : public Counted {
    public:
        static void initialize() ;
        static void terminate() ;

        static PropertyInterpolator* resolve_by_type(const std::type_info& property_type) ;

        virtual ~PropertyInterpolator() ;

        static void bind(
            const std::type_info& property_type,
            PropertyInterpolator* interpolator
        ) ;

        virtual void interpolate_2(
            Memory::pointer to, 
            double a1, Memory::pointer x1, 
            double a2, Memory::pointer x2
        ) ;

        virtual void interpolate_3(
            Memory::pointer to, 
            double a1, Memory::pointer x1, 
            double a2, Memory::pointer x2, 
            double a3, Memory::pointer x3
        ) ;

        virtual void interpolate_4(
            Memory::pointer to, 
            double a1, Memory::pointer x1, 
            double a2, Memory::pointer x2, 
            double a3, Memory::pointer x3,
            double a4, Memory::pointer x4
        ) ;

        virtual void interpolate_begin(Memory::pointer to)  = 0 ;
        virtual void interpolate_add(double a, Memory::pointer x) = 0 ;
        virtual void interpolate_end() = 0 ;

    private:
        typedef std::map<std::string, SmartPointer<PropertyInterpolator> > InterpolatorMap ;
        static InterpolatorMap* type_to_interpolator_ ;
    } ;

    typedef SmartPointer<PropertyInterpolator> PropertyInterpolator_var ;

    //_________________________________________________________________________________________________________

    template <class T> class NumericPropertyInterpolator : public PropertyInterpolator {
    public:
        virtual void interpolate_begin(Memory::pointer to) { 
            target_ = (T*) to ;
            *target_ = T() ;
        }

        virtual void interpolate_add(double a, Memory::pointer x) {
            *target_ += real(a) * (*(T*)x) ;
        }

        virtual void interpolate_end() {
            target_ = nil ;
        }

    protected:
        T* target_ ;
    } ;

    //_________________________________________________________________________________________________________

    template <class T> class gx_register_property_interpolator {
    public:
        gx_register_property_interpolator(PropertyInterpolator* interpolator) {
            PropertyInterpolator::bind(typeid(T), interpolator) ;
        }
    } ;

    //_________________________________________________________________________________________________________

    template <class T> class gx_register_numeric_property_interpolator {
    public:
        gx_register_numeric_property_interpolator() {
            PropertyInterpolator::bind(typeid(T), new NumericPropertyInterpolator<T>) ;
        }
    } ;

    //_________________________________________________________________________________________________________

}

#endif
