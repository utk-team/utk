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
 
#ifndef ___PROPERTY__
#define ___PROPERTY__

#include <Geex/properties/property_manager.h>
#include <Geex/properties/property_store.h>
#include <Geex/properties/property_life_cycle.h>

namespace Geex {
//_________________________________________________________

    class PropertyBase {
    protected:
        static PropertyStore* resolve_named_property_store(
            PropertyManager* manager, const std::string& name
        ) {
            return manager->resolve_named_property_store(name) ;
        }

        static void bind_named_property_store(
            PropertyManager* manager, 
            const std::string& name, PropertyStore* as
        ) {
            manager->bind_named_property_store(name,as) ;
        }
    } ;

    /**
     * enables access and (creation or retreival) to the properties
     * stored in an PropertyManager. Note that distinct Properties
     * can refer to the same property (they can share the same
     * PropertyStore).
     */

    template <class RECORD, class PROPERTY> 
    class Property : public PropertyBase {
    public:

        typedef RECORD Record;
        typedef GenericPropertyManager<RECORD> PropertyManager;
        typedef GenericPropertyStore<PROPERTY> PropertyStore;
        typedef GenericPropertyLifeCycle<PROPERTY> DefaultPropertyLifeCycle;
        typedef Property<RECORD,PROPERTY> thisclass;
        
        typedef ::Geex::PropertyManager::Mode Mode ;

        /**
         * constructs an unbound Property.
         */
        Property() { } 

        /**
         * constructs a transient Property. If specified, the 
         * optional PropertyLifeCycle is used to manage property 
         * creation and destruction.
         */
        Property(
            PropertyManager* manager, PropertyLifeCycle* life_cycle=nil
        ) ;

        /**
         * constructs or retreives a persistent Property (according
         * to the specified mode). mode is one of FIND, CREATE, 
         * FIND_OR_CREATE (default). Note that two properties of
         * different types cannot have the same name (the type
         * is dynamically detected by the PropertyManager, using
         * RTTI). If specified, the optional PropertyLifeCycle is
         * used to manage property creation and destruction.
         */
        Property(
            PropertyManager* manager, const std::string& name, 
            Mode mode=PropertyManager::FIND_OR_CREATE, 
            PropertyLifeCycle* life_cycle=nil
        ) ;
        
        /**
         * constructs an Property refering to the same property
         * as rhs.
         */
        Property(const thisclass& rhs):store_(rhs.store_) { }

        /**
         * makes this Property refer to the same property as rhs.
         */
        thisclass& operator=(const thisclass& rhs) { 
            store_=rhs.store_; return *this;
        }

        bool is_bound() const { return !store_.is_nil(); }

        PropertyManager* property_manager() const {
            gx_assert(is_bound()) ;
            PropertyManager* result = dynamic_cast<PropertyManager*>(
                store_->property_manager()
            ) ;
            gx_assert(result != nil) ;
            return result ;
        }
        
        void bind(
            PropertyManager* manager, PropertyLifeCycle* life_cycle=nil
        ) ;

        void bind(
            PropertyManager* manager, const std::string& name, 
            Mode mode=PropertyManager::FIND_OR_CREATE, 
            PropertyLifeCycle* life_cycle=nil
        ) ;
        
        bool bind_if_defined(
            PropertyManager* manager, const std::string& name
        ) ;

        void unbind() ;

        PROPERTY& operator[](const RECORD& record) {
            return *data(record) ;
        }

        const PROPERTY& operator[](const RECORD& record) const {
            return *data(record) ;
        }
        
        PROPERTY& operator[](const RECORD* record) {
            return *data(*record) ;
        }

        const PROPERTY& operator[](const RECORD* record) const {
            return *data(*record) ;
        }

        /**
         * Checks whether manager has an property of this type
         * bound with the specified name.
         */
        static bool is_defined(
            PropertyManager* manager, const std::string& name
        ) ; 


    protected:
        PROPERTY* data(const RECORD& record) const {
            return reinterpret_cast<PROPERTY*>(store_->data(record)) ;
        }

    private:
        PropertyStore_var store_ ;
    } ;

    
//__________________________________________________________

    template <class RECORD, class PROPERTY> inline 
    void Property<RECORD,PROPERTY>::bind(
        PropertyManager* manager, PropertyLifeCycle* life_cycle
    ) {
        if(life_cycle == nil) {
            life_cycle = new DefaultPropertyLifeCycle ;
        }
        store_ = new PropertyStore(life_cycle, manager);
    }

    template <class RECORD, class PROPERTY> inline 
    bool Property<RECORD,PROPERTY>::is_defined(
        PropertyManager* manager, const std::string& name
    ) {
        return (
            manager->named_property_is_bound(name) &&
            dynamic_cast<PropertyStore*>(
                resolve_named_property_store(manager,name)
            ) != nil
        ) ;
    } 

    template <class RECORD, class PROPERTY> inline 
    void Property<RECORD,PROPERTY>::bind(
        PropertyManager* manager, const std::string& name, 
        Mode mode, PropertyLifeCycle* life_cycle
    ) {
        switch(mode) {
        case PropertyManager::FIND:
        {
            gx_assert(manager->named_property_is_bound(name)) ;
            gx_assert(life_cycle == nil) ;
            store_ = resolve_named_property_store(manager,name) ;
            gx_assert(store_ != nil) ;
            // Sanity check, checks the property type.
            Geex::PropertyStore* check_type = store_ ;
            gx_assert(dynamic_cast<PropertyStore*>(check_type) != nil) ;
            break ;
        }
        case PropertyManager::CREATE:
        {
            gx_assert(!manager->named_property_is_bound(name)) ;
            if(life_cycle == nil) {
                life_cycle = new DefaultPropertyLifeCycle ;
            }
            store_ = new PropertyStore(life_cycle,manager);
            bind_named_property_store(manager,name,store_) ;
            break ;
        }
        case PropertyManager::FIND_OR_CREATE:
        {
            if(manager->named_property_is_bound(name)) {
                store_ = resolve_named_property_store(manager,name) ;
                gx_assert(store_ != nil) ;
                // Sanity check, checks the property type.
                Geex::PropertyStore* check_type = store_ ;
                gx_assert(dynamic_cast<PropertyStore*>(check_type) != nil) ;
            } else {
                if(life_cycle == nil) {
                    life_cycle = new DefaultPropertyLifeCycle ;
                }
                store_ = new PropertyStore(life_cycle,manager) ;
                bind_named_property_store(manager,name,store_) ;
            }
            break ;
        }
        }
    }

    template <class RECORD, class PROPERTY> inline 
    void Property<RECORD,PROPERTY>::unbind() {
        store_.forget() ;
    }

    template <class RECORD, class PROPERTY> inline 
    Property<RECORD,PROPERTY>::Property(
        PropertyManager* manager, PropertyLifeCycle* life_cycle
    ) {
        bind(manager,life_cycle) ;
    }
    
    template <class RECORD, class PROPERTY> inline 
    Property<RECORD,PROPERTY>::Property(
        PropertyManager* manager, const std::string& name, 
        Mode mode, PropertyLifeCycle* life_cycle
    ) {
        bind(manager,name,mode,life_cycle) ;
    }

    template <class RECORD, class PROPERTY> inline 
    bool Property<RECORD,PROPERTY>::bind_if_defined(
        PropertyManager* manager,
        const std::string& name
    ) {
        if(is_defined(manager,name)) {
            bind(manager, name) ;
            return true ;
        }
        return false ;
    }

//_________________________________________________________

}

#endif
