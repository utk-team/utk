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
 
 
 
 
 
 
 
 

#ifndef ___PROPERTY_MANAGER__
#define ___PROPERTY_MANAGER__

#include <Geex/properties/rat.h>
#include <Geex/properties/property_store.h>

#include <set>
#include <map>
#include <vector>
#include <string>
#include <typeinfo>

namespace Geex {

//_________________________________________________________

    class GEEX_API PropertyManager {
    public:

        enum Mode { FIND=1, CREATE=2, FIND_OR_CREATE=3} ;

        PropertyManager() : size_(0) { }
        virtual ~PropertyManager() ;
        size_t capacity() { return rat_.capacity(); }
        size_t size() { return size_; }

        void clear() ;

        /**
         * creates new record properties, and puts the resulting id
         * in the specified Record
         */
        void new_record(Record* to) ;


        /**
         * creates new record properties, initialized from the source record,
         * and puts the resulting id in the specified Record
         */
        void new_record(Record* to, const Record* from) ;

        /**
         * copies all the properties of the from Record to the to Record.
         */
        void copy_record(Record* to, const Record* from) ;

        /**
         * destroys the record properties corresponding to the
         * specified record.
         */
        void delete_record(Record* record) ;

        /**
         * For each property, to <- a1 * x1 + a2 * x2 
         */
        void interpolate_record_2(
            Record* to, double a1, Record* x1, double a2, Record* x2
        ) ;

        /**
         * For each property, to <- a1 * x1 + a2 * x2 + a3 * x3
         */
        void interpolate_record_3(
            Record* to, double a1, Record* x1, double a2, Record* x2, double a3, Record* x3
        ) ;

        /**
         * For each property, to <- a1 * x1 + a2 * x2 + a3 * x3
         */
        void interpolate_record_4(
            Record* to, 
            double a1, Record* x1, double a2, Record* x2, double a3, Record* x3, double a4, Record* x4
        ) ;

        void interpolate_record_begin(Record* to) ;
        void interpolate_record_add(double a, Record* x) ;
        void interpolate_record_end() ;

        size_t nb_named_properties() const ;
        void list_named_properties(std::vector<std::string>& names) const ;
        bool named_property_is_bound(const std::string& name) const ;
        void delete_named_property(const std::string& name) ;

        virtual const std::type_info& record_type_id() const = 0 ;

    protected:

        /**
         * adds the PropertyStore to the list of managed
         * properties, resizes it, and constructs
         * all the elements which are not in the free list.
         */
        void register_property_store(PropertyStore* as) ;

        /**
         * detroys all the elements which are not in the
         * free list, and removes the PropertyStore from
         * the list of managed properties.
         */
        void unregister_property_store(PropertyStore* as) ;

        void bind_named_property_store(
            const std::string& name, PropertyStore* as
        ) ;
        
        PropertyStore* resolve_named_property_store(
            const std::string& name
        ) ;

        RAT& rat() { return rat_ ; }
        const RAT& rat() const { return rat_ ; }

        friend class PropertyStore ;
        friend class PropertyBase ;
        template <class RECORD> friend class PropertyCopier ;

    private:
        RAT rat_ ;
        std::set<PropertyStore*> properties_ ;
        std::map<std::string, PropertyStore_var> named_properties_ ;

        size_t size_ ;
    } ;


    template <class RECORD> 
    class GenericPropertyManager : public PropertyManager {
        virtual const std::type_info& record_type_id() const {
            return typeid(RECORD) ;
        }
    } ;


//_________________________________________________________

}
#endif

