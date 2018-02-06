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
 
 
 
 
 
 
 

#ifndef ___PROPERTY_COPIER__
#define ___PROPERTY_COPIER__

#include <Geex/properties/property.h>

#include <string>
#include <typeinfo>
#include <vector>

namespace Geex {
    
    //_________________________________________________________________________________________________________

    template <class RECORD> class PropertyCopier {
    public:
        PropertyCopier() { 
            source_manager_ = nil ; source_store_ = nil ;
            dest_manager_ = nil ; dest_store_ = nil ;
        }
        bool source_is_bound() const { return source_store_ != nil ; }
        void bind_source(PropertyManager* manager, const std::string& name) {
            gx_parano_assert(manager->record_type_id() == typeid(RECORD)) ;
            source_manager_ = manager ;
            if(source_manager_->named_property_is_bound(name)) {
                source_store_ = source_manager_->resolve_named_property_store(name) ;
            } else {
                source_store_ = nil ;
            }
            source_name_ = name ;
        }
        bool destination_is_bound() const { return dest_store_ != nil ; }
        void bind_destination(PropertyManager* manager, const std::string& name) {
            gx_assert(source_is_bound()) ;
            dest_manager_ = manager ;
            if(dest_manager_->named_property_is_bound(name)) {
                dest_store_ = dest_manager_->resolve_named_property_store(name) ;
            } else {
                dest_store_ = source_store_->clone() ;
                dest_store_->bind(dest_manager_) ;
                dest_manager_->bind_named_property_store(name, dest_store_) ;
            }
            if(source_store_->property_type_id() != dest_store_->property_type_id()) {
                Logger::err("PropertyCopier") << "Property " << name 
                                                << " does not have same type as source property"
                                                << std::endl ;
                unbind_destination() ;
            }
            dest_name_ = name ;
        }
        void bind_destination(PropertyManager* manager) {
            bind_destination(manager, source_name_) ;
        }
        void unbind_source() {
            source_manager_ = nil ;
            source_store_ = nil ;
            source_name_ = "" ;
        }
        void unbind_destination() {
            dest_manager_ = nil ;
            dest_store_ = nil ;
            dest_name_ = "" ; 
        }
        PropertyManager* source_property_manager() const { return source_manager_ ; }
        const std::string& source_name() const { return source_name_ ; }
        PropertyManager* destination_property_manager() const { return dest_manager_ ; }
        const std::string& destination_name() const { return dest_name_ ; }
        void copy_property(RECORD* to, RECORD* from) {
            gx_assert(source_is_bound()) ;
            gx_assert(destination_is_bound()) ;
            Memory::pointer to_addr = dest_store_->data(*to) ;
            Memory::pointer from_addr = source_store_->data(*from) ;
            source_store_->copy(to_addr, to, from_addr, from) ;
        }
    private:
        PropertyManager* source_manager_ ;
        PropertyStore* source_store_ ;
        PropertyManager* dest_manager_ ;
        PropertyStore* dest_store_ ;
        std::string source_name_ ;
        std::string dest_name_ ;
    } ;

    //_________________________________________________________________________________________________________

    template <class RECORD> inline void copy_properties(
        std::vector<PropertyCopier<RECORD> >& copiers,
        RECORD* to, RECORD* from
    ) {
        for(index_t i=0; i<copiers.size(); i++) {
            if(copiers[i].source_is_bound() && copiers[i].destination_is_bound()) {
                copiers[i].copy_property(to, from) ;
            }
        }
    }

    template <class RECORD> inline void bind_source(
        std::vector<PropertyCopier<RECORD> >& copiers,
        PropertyManager* source_property_manager,
        const std::string& name
    ) {
        for(index_t i=0; i<copiers.size(); i++) {
            if(copiers[i].source_name() == name) {
                if(copiers[i].source_property_manager() == source_property_manager) {
                    return ;
                }
                copiers[i].bind_source(source_property_manager, name) ;
                return ;
            }
        }
        copiers.push_back(PropertyCopier<RECORD>()) ;
        copiers.rbegin()->bind_source(source_property_manager, name) ;
    }

    template <class RECORD> inline void bind_destinations(
        std::vector<PropertyCopier<RECORD> >& copiers,
        PropertyManager* destination_property_manager
    ) {
        for(index_t i=0; i<copiers.size(); i++) {
            if(copiers[i].source_is_bound()) {
                if(!copiers[i].destination_is_bound() ||
                    copiers[i].destination_name() != copiers[i].source_name() ||
                    copiers[i].destination_property_manager() != destination_property_manager
                ) {
                    copiers[i].bind_destination(destination_property_manager, copiers[i].source_name()) ;
                }
            }
        }
    }

    //_________________________________________________________________________________________________________

}

#endif
