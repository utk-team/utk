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
 
 #include <Geex/properties/property_serializer.h>

namespace Geex {

    PropertySerializer::SerializerMap* PropertySerializer::type_to_serializer_ = nil ;
    PropertySerializer::SerializerMap* PropertySerializer::name_to_serializer_  = nil ;
    PropertySerializer::StringMap*     PropertySerializer::alias_to_name_        = nil ;    
    PropertySerializer::StringMap*     PropertySerializer::type_to_name_        = nil ;    

    void PropertySerializer::initialize() {
        gx_assert(type_to_serializer_ == nil) ;
        type_to_serializer_ = new SerializerMap ;
        gx_assert(name_to_serializer_ == nil) ;
        name_to_serializer_ = new SerializerMap ;
        gx_assert(alias_to_name_ == nil) ;
        alias_to_name_ = new StringMap ;
        gx_assert(type_to_name_ == nil) ;
        type_to_name_ = new StringMap ;
    }
    
    void PropertySerializer::terminate() {
        delete type_to_serializer_ ;
        type_to_serializer_ = nil ;
        delete name_to_serializer_ ;
        name_to_serializer_ = nil ;
        delete alias_to_name_ ;
        alias_to_name_ = nil ;
        delete type_to_name_ ;
        type_to_name_ = nil ;
    }
    
    PropertySerializer* PropertySerializer::resolve_by_type(const std::type_info& property_type) {
        gx_assert(type_to_serializer_ != nil) ;
        SerializerMap::iterator it = type_to_serializer_->find(property_type.name()) ;
        if(it == type_to_serializer_->end()) {
            return nil ;
        }
        return it->second ;
    }

    PropertySerializer* PropertySerializer::resolve_by_name(const std::string& type_name_in) {
        gx_assert(alias_to_name_ != nil) ;
        std::string type_name = type_name_in ;
        {
            StringMap::const_iterator it = alias_to_name_->find(type_name_in) ;
            if(it != alias_to_name_->end()) {
                type_name = it->second ;
            }
        }
        gx_assert(name_to_serializer_ != nil) ;       
        SerializerMap::iterator it = name_to_serializer_->find(type_name) ;
        if(it == name_to_serializer_->end()) {
            return nil ;
        }
        return it->second ;
    }

    std::string PropertySerializer::find_name_by_type(const std::type_info& property_type) {
        gx_assert(type_to_name_ != nil) ; 
        StringMap::iterator it = type_to_name_->find(property_type.name()) ;
        if(it == type_to_name_->end()) {
            return "unknown" ;
        }
        return it->second ;
    }

    void PropertySerializer::bind(
        const std::type_info& property_type, const std::string& property_type_name, 
        PropertySerializer* serializer
    ) {
        gx_assert(resolve_by_type(property_type) == nil) ;
        gx_assert(resolve_by_name(property_type_name) == nil) ;
        (*type_to_serializer_)[property_type.name()] = serializer ;
        (*name_to_serializer_)[property_type_name] = serializer ;
        (*type_to_name_)[property_type.name()] = property_type_name ;
    }
    

}
