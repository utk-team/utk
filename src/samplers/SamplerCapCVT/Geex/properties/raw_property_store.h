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
 
 
 
 
 
 
 

#ifndef _RAW_PROPERTY_STORE__
#define _RAW_PROPERTY_STORE__

#include <Geex/properties/record_id.h>

#include <vector>
#include <set>

namespace Geex {

//_________________________________________________________

    /**
     * Base class for the low-level storage for properties. Client
     * code should not need to use this directly. The storage space
     * is allocated in chunks, to make dynamic growing more efficient
     * (i.e. without needing to copy the data).
     */
    class GEEX_API RawPropertyStore {
    public:
        enum { CHUNK_SIZE = RecordId::MAX_OFFSET + 1 } ;
       
        RawPropertyStore(size_t item_size) : item_size_(item_size) { }
        virtual void clear() ; 
        virtual ~RawPropertyStore() ;
        size_t item_size() const { return item_size_ ; }
        size_t nb_chunks() const { return (size_t)data_.size() ; }
        size_t capacity() const { 
            return (size_t)data_.size() * CHUNK_SIZE ;
        }

        Memory::pointer data(
            index_t chunk, index_t offset
        ) const {
            gx_property_assert(chunk < data_.size()) ;
            gx_property_assert(offset < CHUNK_SIZE) ;
            return &(data_[chunk][offset * item_size_]) ;
        }

        Memory::pointer data(const Record& r) const {
            return data(r.record_id().chunk(), r.record_id().offset()) ;
        }

        virtual void grow() ;

    private:
        size_t item_size_ ;
        std::vector<Memory::pointer> data_ ;
    } ;

//_________________________________________________________

}
#endif

