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

#ifndef ___PROPERTY_SERIALIZER__
#define ___PROPERTY_SERIALIZER__

#include <Geex/properties/property.h>

#include <map>
#include <string>
#include <typeinfo>

namespace Geex {
    
    //_________________________________________________________________________________________________________

    /**
     * PropertySerializer is used to save and load properties attached
     * to an object. This is the base class for serializing the value of 
     * an property and creating an PropertyStore given its type name.
     */
    class GEEX_API PropertySerializer : public Counted {
    public:
        static void initialize() ;
        static void terminate() ;

        static PropertySerializer* resolve_by_type(const std::type_info& property_type) ;
        static PropertySerializer* resolve_by_name(const std::string& property_type_name) ;
        static std::string find_name_by_type(const std::type_info& property_type) ;
        static void register_type_alias(const std::string& alias, const std::string& name) {
            (*alias_to_name_)[alias] = name ;
        }

        static void bind(
            const std::type_info& property_type,
            const std::string& property_type_name,
            PropertySerializer* serializer
        ) ;

        virtual PropertyStore* create_property_store(Geex::PropertyManager* manager) = 0 ;
        virtual bool serialize_read(std::istream& in,   Memory::pointer addr) = 0 ;
        virtual bool serialize_write(std::ostream& out, Memory::pointer addr) = 0 ;

    private:
        typedef std::map<std::string, SmartPointer<PropertySerializer> > SerializerMap ;
        typedef std::map<std::string, std::string> StringMap ;
        static SerializerMap* type_to_serializer_ ;
        static SerializerMap* name_to_serializer_ ;
        static StringMap*      alias_to_name_ ;
        static StringMap*     type_to_name_ ;
    } ;

    typedef SmartPointer<PropertySerializer> PropertySerializer_var ;

    //_________________________________________________________________________________________________________

    /**
     * Default implementation of PropertySerializer.
     */
    template <class PROPERTY> class GenericPropertySerializer : public PropertySerializer {
    public:
        typedef GenericPropertyStore<PROPERTY> PropertyStore;
        typedef GenericPropertyLifeCycle<PROPERTY> DefaultPropertyLifeCycle;

        virtual PropertyStore* create_property_store(Geex::PropertyManager* manager) {
            DefaultPropertyLifeCycle* life_cycle = new DefaultPropertyLifeCycle ;
            return new PropertyStore(life_cycle, manager) ;
        }
        virtual bool serialize_read(std::istream& in, Memory::pointer addr) {
            PROPERTY& attr = *reinterpret_cast<PROPERTY*>(addr) ;
            in >> attr ;
            return true ;
        }
        virtual bool serialize_write(std::ostream& out, Memory::pointer addr) {
            PROPERTY& attr = *reinterpret_cast<PROPERTY*>(addr) ;
            out << attr ;
            return true ;
        }

    } ;

    //_________________________________________________________________________________________________________

    /**
     * Use this class to declare a new serializable property type.
     * In the common.cpp file of the library, add:
     * gx_register_property_type<MyPropertyType>("MyPropertyType") ;
     */
    template <class T> class gx_register_property_type {
    public:
        gx_register_property_type(const std::string& type_name) {
            PropertySerializer::bind(typeid(T), type_name, new GenericPropertySerializer<T>()) ;
        }
    } ;

    //_________________________________________________________________________________________________________

    inline void gx_register_property_type_alias(const std::string& alias, const std::string& type_name) {
        PropertySerializer::register_type_alias(alias, type_name) ;
    }

    //_________________________________________________________________________________________________________

    /**
     * SerializedPropertyRef is what SerializedProperty::operator[] returns.
     * It is just meant to overload operator<< and operator>>.
     */
    class SerializedPropertyRef {
    public:
        SerializedPropertyRef(
            PropertySerializer* ser, Memory::pointer addr
        ) : serializer_(ser), addr_(addr) {
        }
        PropertySerializer* serializer() const { return serializer_ ; }
        Memory::pointer addr() const { return addr_ ; }
    private:
        PropertySerializer* serializer_ ;
        Memory::pointer addr_ ;
    } ;
    
    inline std::istream& operator>>(std::istream& in, const SerializedPropertyRef& r) {
        r.serializer()->serialize_read(in, r.addr()) ;
        return in ;
    }

    inline std::ostream& operator<<(std::ostream& out, const SerializedPropertyRef& r) {
        r.serializer()->serialize_write(out, r.addr()) ;
        return out ;
    }

    //_________________________________________________________________________________________________________

    /**
     * SerializedProperty allows writing property values into a stream,
     * reading property values from a stream, and creating an property
     * from its type name. 
     */
    template <class RECORD> class SerializedProperty : public PropertyBase {
    public:
        typedef GenericPropertyManager<RECORD> PropertyManager;

        void bind(PropertyManager* manager, const std::string& name) {
            property_manager_ = manager ;
            property_store_ = resolve_named_property_store(manager, name) ;
            if(property_store_ != nil) {
                serializer_ = PropertySerializer::resolve_by_type(property_store_->property_type_id()) ;
            }
            name_ = name ;
        }

        void bind(PropertyManager* manager, const std::string& name, const std::string& property_type_name) {
            property_manager_ = manager ;
            serializer_ = PropertySerializer::resolve_by_name(property_type_name) ;
            if(serializer_ != nil) {
                if(property_manager_->named_property_is_bound(name)) {
                    property_store_ = resolve_named_property_store(property_manager_,name) ;
                    gx_assert(
                        PropertySerializer::find_name_by_type(
                            property_store_->property_type_id()
                        ) == property_type_name
                    ) ;
                } else {
                    property_store_ = serializer_->create_property_store(property_manager_) ;
                    bind_named_property_store(property_manager_,name,property_store_) ;
                }
            }
            name_ = name ;
        }

        void unbind() {
            property_manager_ = nil ;
            property_store_ = nil ;
            serializer_ = nil ;
        }

        SerializedProperty() {
            property_manager_ = nil ;
            property_store_ = nil ;
            serializer_ = nil ;
        }

        SerializedProperty(PropertyManager* manager, const std::string& name) {
            bind(manager, name) ;
        }

        SerializedProperty(
            PropertyManager* manager, const std::string& name, const std::string& property_type_name
        ) {
            bind(manager, name, property_type_name) ;
        }

        SerializedProperty(const SerializedProperty& rhs) {
            property_manager_ = rhs.property_manager_ ;
            property_store_   = rhs.property_store_ ;
            serializer_        = rhs.serializer_ ;
            name_              = rhs.name_ ;
        }

        bool is_bound() const {
            return (property_manager_ != nil) && (property_store_ != nil) && (serializer_ != nil) ;
        }

        const std::string& name() const { return name_ ; }

        std::string type_name() const {
            gx_assert(property_store_ != nil) ;
            return PropertySerializer::find_name_by_type(property_store_->property_type_id()) ;
        }

        SerializedPropertyRef operator[](RECORD* record) {
            return SerializedPropertyRef(serializer_, property_store_->data(*record)) ;
        }

    private:
        PropertyManager* property_manager_ ;
        PropertyStore* property_store_ ;
        PropertySerializer* serializer_ ;
        std::string name_ ;
    } ;


    //_________________________________________________________________________________________________________


}

#endif
