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
 
 
 
 
 
 
 

#ifndef ___PROPERTY_STORE__
#define ___PROPERTY_STORE__

#include <Geex/properties/raw_property_store.h>
#include <Geex/properties/property_life_cycle.h>
#include <Geex/properties/property_interpolator.h>
#include <Geex/basics/smart_pointer.h>
#include <Geex/basics/counted.h>

#include <typeinfo>

namespace Geex {

//_________________________________________________________

    class PropertyManager ;
    class Record ;

    /**
     * stores an property, and knows how to construct,copy,destroy
     * instances of the property. This class should not be used
     * directly by client code.
     */
    class GEEX_API PropertyStore : public Counted, public RawPropertyStore {
    public:
        
        PropertyStore(
            PropertyLifeCycle* life_cycle,
            PropertyManager* manager = nil
        ) : RawPropertyStore( life_cycle->item_size() ), 
            life_cycle_(life_cycle), manager_(nil), interpolator_(nil) {
            bind(manager) ;
        }
        
        virtual ~PropertyStore() ;

        void construct(
            Memory::pointer addr, Record* record = 0
        ) {
            life_cycle_->construct(addr,record) ;
        }

        void destroy(
            Memory::pointer addr, Record* record = 0
        ) {
            life_cycle_->destroy(addr,record) ;
        }

        void copy(
            Memory::pointer lhs, Record* record_lhs,
            Memory::pointer rhs, const Record* record_rhs
        ) {
            life_cycle_->copy(lhs,record_lhs,rhs,record_rhs) ;
        }

        void copy_construct(
            Memory::pointer lhs, Record* record_lhs,
            Memory::pointer rhs, const Record* record_rhs
        ) {
            life_cycle_->copy_construct(lhs,record_lhs,rhs,record_rhs) ;
        }

        void interpolate_2(
            Memory::pointer to, Record* record_to,
            double a1, Memory::pointer x1, const Record* record_x1,
            double a2, Memory::pointer x2, const Record* record_x2
        ) {
            if(interpolator_ != nil) {
                interpolator_->interpolate_2(to, a1, x1, a2, x2) ;
            } else {
                life_cycle_->destroy(to) ;
                life_cycle_->construct(to) ;
            }
        }

        void interpolate_3(
            Memory::pointer to, Record* record_to,
            double a1, Memory::pointer x1, const Record* record_x1,
            double a2, Memory::pointer x2, const Record* record_x2,
            double a3, Memory::pointer x3, const Record* record_x3
        ) {
            if(interpolator_ != nil) {
                interpolator_->interpolate_3(to, a1, x1, a2, x2, a3, x3) ;
            } else {
                life_cycle_->destroy(to) ;
                life_cycle_->construct(to) ;
            }
        }

        void interpolate_4(
            Memory::pointer to, Record* record_to,
            double a1, Memory::pointer x1, const Record* record_x1,
            double a2, Memory::pointer x2, const Record* record_x2,
            double a3, Memory::pointer x3, const Record* record_x3,
            double a4, Memory::pointer x4, const Record* record_x4
        ) {
            if(interpolator_ != nil) {
                interpolator_->interpolate_4(to, a1, x1, a2, x2, a3, x3, a4, x4) ;
            } else {
                life_cycle_->destroy(to) ;
                life_cycle_->construct(to) ;
            }
        }

        void interpolate_begin(Memory::pointer to, Record* record_to) {
            if(interpolator_ != nil) {
                interpolator_->interpolate_begin(to) ;
            } else {
                life_cycle_->destroy(to) ;
                life_cycle_->construct(to) ;
            }
        }

        void interpolate_add(
            double a, Memory::pointer x, Record* record_x
        ) {
            if(interpolator_ != nil) {
                interpolator_->interpolate_add(a,x) ;
            }
        }

        void interpolate_end() {
            if(interpolator_ != nil) {
                interpolator_->interpolate_end() ;
            }
        }

        void bind(PropertyManager* manager) ;
        PropertyManager* property_manager() const { return manager_; }

        virtual const std::type_info& property_type_id() const = 0 ;

        /** returns an empty PropertyStore() of the same type. */
        virtual PropertyStore* clone() = 0 ;

    protected:
        PropertyLifeCycle_var life_cycle_ ;
        PropertyManager* manager_ ;
        PropertyInterpolator* interpolator_ ;
    } ;

    typedef SmartPointer<PropertyStore> PropertyStore_var ;

//_________________________________________________________

    /**
     * A typed PropertyStore, templated by the 
     * Record class and the Property class. This
     * is used for static and dynamic type checking
     * in the PropertyManager.
     */
    template <class PROPERTY> 
    class GenericPropertyStore : public PropertyStore {
    public:
        GenericPropertyStore(
            PropertyLifeCycle* life_cycle,
            PropertyManager* manager = nil
        ) : PropertyStore(life_cycle, manager) { 
            interpolator_ = PropertyInterpolator::resolve_by_type(typeid(PROPERTY)) ;
        }        
        virtual ~GenericPropertyStore() { }
        virtual const std::type_info& property_type_id() const {
            return typeid(PROPERTY) ;
        }
        virtual PropertyStore* clone() {
            return new GenericPropertyStore<PROPERTY>((PropertyLifeCycle*)life_cycle_) ;
        }
    } ;

//_________________________________________________________

}

#endif

