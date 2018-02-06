
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

#ifndef __GENERIC_PROPERTIES_IO__
#define __GENERIC_PROPERTIES_IO__

#include <Geex/properties/property_manager.h>
#include <Geex/properties/property_serializer.h>
#include <iostream>

namespace Geex {
    
    template <class T> inline bool get_serializable_properties(
        GenericPropertyManager<T>* manager, std::vector<SerializedProperty<T> >& properties,
        std::ostream& out, const std::string& localisation
    ) {
        bool result = false ;
        std::vector<std::string> names ;
        manager->list_named_properties(names) ;
        for(unsigned int i=0; i<names.size(); i++) {
            properties.push_back(SerializedProperty<T>()) ;
            properties.rbegin()->bind(manager, names[i]) ;
            if(properties.rbegin()->is_bound()) {
                std::cerr << "Property " << names[i] << " on " << localisation << " : " 
                          << properties.rbegin()->type_name() << std::endl ;
                out << "# attribute " << names[i] << " " << localisation << " " 
                    << properties.rbegin()->type_name() << std::endl ;
                result = true ;
            } else {
                std::cerr << "Property " << names[i] << " on " << localisation 
                          << " is not serializable" << std::endl ;
                properties.pop_back() ;
            }
        }
        return result ;
    }
    
    template <class T> inline void serialize_read_properties(
        std::istream& in, T* item, std::vector<SerializedProperty<T> >& properties
    ) {
        for(unsigned int i=0; i<properties.size(); i++) {
            in >> properties[i][item] ;
        }
    }

    template <class T> inline void serialize_write_properties(
        std::ostream& out, T* item, std::vector<SerializedProperty<T> >& properties
    ) {
        for(unsigned int i=0; i<properties.size(); i++) {
            out << properties[i][item] << " " ;
        }
    }
}

#endif

