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
 
 
 
 
 
 
 

#include <Geex/properties/property_manager.h>
#include <typeinfo>

namespace Geex {

//_________________________________________________________

    PropertyManager::~PropertyManager() {
        // Makes sure that some properties are not still connected to
        // this PropertyManager. 
        // Displays an error message to help debugging.

        if(properties_.size() != named_properties_.size()) {
            std::cerr 
                << "Fatal error: PropertyManager destroyed "
                << "before one of its properties"
                << std::endl ;

            int nb_dangling_attrs = 
                (int)(properties_.size() - named_properties_.size()) ;

            std::cerr
                << "found " << nb_dangling_attrs << " dangling property(s)"
                << std::endl ;
            std::cerr
                << "Could come from a Map destroyed "
                << "before an algorithm class plugged on it"
                << std::endl ;
            std::cerr
                << "To fix the problem: put the algorithm class in a scope { }"
                << std::endl ;
            std::cerr
                << "     or make the algorithm class unbind its properties"
                << " at the end of apply()"
                << std::endl ;
            std::cerr
                << "exiting..."
                << std::endl ;
            gx_assert(false) ;
        }
    }

    void PropertyManager::new_record(Record* record) {
        if(rat_.is_full()) {
            rat_.grow() ;
            for(std::set<PropertyStore*>::iterator 
                    it=properties_.begin(); it!=properties_.end(); it++
            ) {
                (*it)->grow() ;
                gx_property_assert((*it)->capacity() == capacity()) ;
            }
        }
        record->set_record_id(rat_.new_record_id()) ;
        index_t chunk  = record->record_id().chunk() ;
        index_t offset = record->record_id().offset() ;
        for(std::set<PropertyStore*>::iterator 
                 it=properties_.begin(); it!=properties_.end(); it++
        ) {
            (*it)->construct(  (*it)->data(chunk,offset), record ) ;
        } 
        size_++ ;
    }

    void PropertyManager::new_record(Record* record, const Record* from) {
        if(rat_.is_full()) {
            rat_.grow() ;
            for(std::set<PropertyStore*>::iterator 
                    it=properties_.begin(); it!=properties_.end(); it++
            ) {
                (*it)->grow() ;
                gx_property_assert((*it)->capacity() == capacity()) ;
           }
        }
        record->set_record_id(rat_.new_record_id()) ;
        index_t chunk  = record->record_id().chunk() ;
        index_t offset = record->record_id().offset() ;

        index_t chunk_from  = from->record_id().chunk() ;
        index_t offset_from = from->record_id().offset() ; 

        for(std::set<PropertyStore*>::iterator 
                 it=properties_.begin(); it!=properties_.end(); it++
        ) {
            (*it)->copy_construct(  
                (*it)->data(chunk,offset), record,
                (*it)->data(chunk_from,offset_from), from
            ) ;
        } 
        size_++ ;
    }

    void PropertyManager::copy_record(Record* record, const Record* from) {
        index_t chunk  = record->record_id().chunk() ;
        index_t offset = record->record_id().offset() ;

        index_t chunk_from  = from->record_id().chunk() ;
        index_t offset_from = from->record_id().offset() ; 

        for(std::set<PropertyStore*>::iterator 
                 it=properties_.begin(); it!=properties_.end(); it++
        ) {
            (*it)->copy(  
                (*it)->data(chunk,offset), record,
                (*it)->data(chunk_from,offset_from), from
            ) ;
        } 
    }

    void PropertyManager::delete_record(Record* record) {
        index_t chunk  = record->record_id().chunk() ;
        index_t offset = record->record_id().offset() ;
        for(std::set<PropertyStore*>::iterator 
                 it=properties_.begin(); it!=properties_.end(); it++
        ) {
            (*it)->destroy(  (*it)->data(chunk,offset), record ) ;
        }    
        rat_.delete_record_id(record->record_id()) ;
        record->record_id().forget() ;
        size_-- ;
    }

    void PropertyManager::interpolate_record_2(
        Record* record, 
        double a1, Record* x1, 
        double a2, Record* x2
    ) {
        index_t chunk  = record->record_id().chunk() ;
        index_t offset = record->record_id().offset() ;

        index_t chunk1 = x1->record_id().chunk() ;
        index_t offset1 = x1->record_id().offset() ; 

        index_t chunk2 = x2->record_id().chunk() ;
        index_t offset2 = x2->record_id().offset() ; 

        for(std::set<PropertyStore*>::iterator 
                it=properties_.begin(); it!=properties_.end(); it++
        ) {
            (*it)->interpolate_2(  
                (*it)->data(chunk,offset), record,
                a1, (*it)->data(chunk1,offset1), x1,
                a2, (*it)->data(chunk2,offset2), x2
            ) ;
        } 
    }
    
    void PropertyManager::interpolate_record_3(
        Record* record, 
        double a1, Record* x1, 
        double a2, Record* x2, 
        double a3, Record* x3
    ) {
        index_t chunk  = record->record_id().chunk() ;
        index_t offset = record->record_id().offset() ;

        index_t chunk1 = x1->record_id().chunk() ;
        index_t offset1 = x1->record_id().offset() ; 

        index_t chunk2 = x2->record_id().chunk() ;
        index_t offset2 = x2->record_id().offset() ; 

        index_t chunk3 = x3->record_id().chunk() ;
        index_t offset3 = x3->record_id().offset() ; 

        for(std::set<PropertyStore*>::iterator 
                it=properties_.begin(); it!=properties_.end(); it++
        ) {
            (*it)->interpolate_3(  
                (*it)->data(chunk,offset), record,
                a1, (*it)->data(chunk1,offset1), x1,
                a2, (*it)->data(chunk2,offset2), x2,
                a3, (*it)->data(chunk3,offset3), x3
            ) ;
        } 
    }

    void PropertyManager::interpolate_record_4(
        Record* record, 
        double a1, Record* x1, 
        double a2, Record* x2, 
        double a3, Record* x3,
        double a4, Record* x4
    ) {
        index_t chunk  = record->record_id().chunk() ;
        index_t offset = record->record_id().offset() ;

        index_t chunk1 = x1->record_id().chunk() ;
        index_t offset1 = x1->record_id().offset() ; 

        index_t chunk2 = x2->record_id().chunk() ;
        index_t offset2 = x2->record_id().offset() ; 

        index_t chunk3 = x3->record_id().chunk() ;
        index_t offset3 = x3->record_id().offset() ; 

        index_t chunk4 = x4->record_id().chunk() ;
        index_t offset4 = x4->record_id().offset() ; 

        for(std::set<PropertyStore*>::iterator 
                it=properties_.begin(); it!=properties_.end(); it++
        ) {
            (*it)->interpolate_4(  
                (*it)->data(chunk,offset), record,
                a1, (*it)->data(chunk1,offset1), x1,
                a2, (*it)->data(chunk2,offset2), x2,
                a3, (*it)->data(chunk3,offset3), x3,
                a4, (*it)->data(chunk4,offset4), x4
            ) ;
        } 
    }

    void PropertyManager::interpolate_record_begin(Record* record) {
        index_t chunk  = record->record_id().chunk() ;
        index_t offset = record->record_id().offset() ;
        for(std::set<PropertyStore*>::iterator 
                it=properties_.begin(); it!=properties_.end(); it++
        ) {
            (*it)->interpolate_begin((*it)->data(chunk,offset), record) ;
        }
    }

    void PropertyManager::interpolate_record_add(double a, Record* x) {
        index_t chunk_x = x->record_id().chunk() ;
        index_t offset_x = x->record_id().offset() ; 
        for(std::set<PropertyStore*>::iterator 
                it=properties_.begin(); it!=properties_.end(); it++
        ) {
            (*it)->interpolate_add(  
                a, (*it)->data(chunk_x,offset_x), x
            ) ;
        } 
    }

    void PropertyManager::interpolate_record_end() {
        for(std::set<PropertyStore*>::iterator 
                it=properties_.begin(); it!=properties_.end(); it++
        ) {
            (*it)->interpolate_end() ;
        }
    }

    void PropertyManager::clear() {
        for(std::set<PropertyStore*>::iterator 
                 it=properties_.begin(); it!=properties_.end(); it++
        ) {
            for(index_t chunk=0; chunk<rat_.nb_chunks(); chunk++) {
                for(index_t offset=0; offset<RAT::CHUNK_SIZE; offset++) {
                    if(!rat_.cell(chunk,offset).is_free()) {
                        (*it)->destroy( (*it)->data(chunk,offset) ) ;
                    } 
                }
            }
            (*it)->clear() ;
        } 
        rat_.clear() ;
        size_ = 0 ;
    }
    
    size_t PropertyManager::nb_named_properties() const {
        return (size_t)named_properties_.size() ;
    }

    void PropertyManager::list_named_properties(
        std::vector<std::string>& names
    ) const {
        names.clear() ;
        for(
            std::map<std::string, PropertyStore_var>::const_iterator
                it=named_properties_.begin();
            it!=named_properties_.end(); it++
        ) {
            names.push_back(it->first) ;
        }
    }
    
    bool PropertyManager::named_property_is_bound(
        const std::string& name
    ) const {
        return (
            named_properties_.find(name) != 
            named_properties_.end()
        ) ;
    }

    void PropertyManager::register_property_store(PropertyStore* as) {
        gx_assert(
            properties_.find(as) == properties_.end() 
        ) ;
        properties_.insert(as) ;

        for(index_t chunk=0; chunk<rat_.nb_chunks(); chunk++) {
            for(index_t offset=0; offset<RAT::CHUNK_SIZE; offset++) {
                if(!rat_.cell(chunk,offset).is_free()) {
                    as->construct( as->data(chunk,offset) ) ;
                }
            }
        }
    }

    void PropertyManager::unregister_property_store(PropertyStore* as) {

        for(index_t chunk=0; chunk<rat_.nb_chunks(); chunk++) {
            for(index_t offset=0; offset<RAT::CHUNK_SIZE; offset++) {
                if(!rat_.cell(chunk,offset).is_free()) {
                    as->destroy( as->data(chunk,offset) ) ;
                } 
            }
        }


        std::set<PropertyStore*>::iterator it = properties_.find(as) ;
        gx_assert(it != properties_.end()) ;
        properties_.erase(it) ;
    }

    void PropertyManager::bind_named_property_store(
        const std::string& name, PropertyStore* as
    ) {
        gx_assert( !named_property_is_bound(name) ) ;
        named_properties_[name] = as ;
    }
        
    PropertyStore* PropertyManager::resolve_named_property_store(
        const std::string& name
    ) {
        std::map<std::string, PropertyStore_var>::iterator 
            it=named_properties_.find(name) ;
        gx_assert(it != named_properties_.end()) ;
        return it->second ;
    }

    void PropertyManager::delete_named_property(
        const std::string& name
    ) {
        std::map<std::string, PropertyStore_var>::iterator 
            it=named_properties_.find(name) ;
        gx_assert(it != named_properties_.end()) ;
        gx_assert(!it->second->is_shared()) ;
        named_properties_.erase(it) ;
    }

//_________________________________________________________

}

