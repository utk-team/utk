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

#ifndef __GEEX_CVT_CONTAINERS__
#define __GEEX_CVT_CONTAINERS__

#include <Geex/basics/assert.h>
#include <Geex/basics/arrays.h>
#include <set>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>

namespace Geex {

    /**
     * Similar to std::set, but with fixed maximum size
     * (and no dynamic memory allocation). Used to store
     * vertices equations (represented as plane indices 
     * triplets).
     */
    template <class T, int DIM> class small_set {
    public:
        typedef small_set<T,DIM> thisclass ;
        typedef T* iterator ;
        typedef const T* const_iterator ;
        typedef T& reference ;
        typedef T value_type ;
        
        small_set() : end_(data_) { }

        small_set(const thisclass& rhs) {
            copy(rhs) ;
        }
        
        thisclass& operator=(const thisclass& rhs) {
            copy(rhs) ; return *this ;
        }

        template <int DIM2> small_set(const small_set<T,DIM2>& rhs) {
            copy(rhs) ;
        }
        
        template <int DIM2> thisclass& operator=(const small_set<T,DIM2>& rhs) {
            copy(rhs) ; return *this ;
        }

        unsigned int size() const { return (unsigned int)(end_ - data_) ; }
        unsigned int capacity() const { return (unsigned int)DIM ; }

        iterator begin() { return data_ ; }
        iterator end() { return end_ ; }
        iterator end_of_storage() { return data_ + DIM ; }

        iterator* end_ptr() { return &end_ ; }

        const_iterator begin() const { return data_ ; }
        const_iterator end() const { return end_ ; }
        const_iterator end_of_storage() const { return data_ + DIM ; }

        iterator insert(const T& x) { 
            return insert(x, find_i(x)) ; 
        }

        iterator insert(const T& x, iterator where) {
            if(where == end()) { *where = x ; grow() ; return where ; }
            if(*where == x) { return where ; }
            grow() ;
            if(where == end() - 1) {
                *where = x ; return where ; 
            }
            for(iterator i = end()-1; i != where; i--) {
                gx_debug_assert(i != begin()) ;
                *i = *(i-1) ;
            }
            *where = x ;
#ifdef GX_DEBUG
            for(iterator i = begin(); i != end() - 1; i++) {
                gx_debug_assert(*i < *(i+1)) ;
            }
#endif
            return where ;
        }

        void clear() { end_ = data_ ; }

        iterator find(const T& x) {
            iterator result = find_i(x) ;
            if(*result != x) { result = end() ; }
            return result ;
        }

        const_iterator find(const T& x) const {
            const_iterator result = find_i(x) ;
            if(*result != x) { result = end() ; }
            return result ;
        }

        void push_back(const T& x) {
#ifdef GX_DEBUG
            for(iterator i = begin(); i != end(); i++) {
                gx_debug_assert(*i < x) ;
            }
#endif
            *end_ = x ;
            grow() ;
        }

        void print(std::ostream& out) const {
            out << "[ " ;
            for(const_iterator it=begin(); it!=end(); it++) {
                out << *it << " " ;
            }
            out << "]" ;
        }

        T& operator[](int i) {
            gx_debug_assert(i >= 0) ;
            gx_debug_assert(begin() + i < end()) ;
            return begin()[i] ;
        }

        const T& operator[](int i) const {
            gx_debug_assert(i >= 0) ;
            gx_debug_assert(begin() + i < end()) ;
            return begin()[i] ;
        }

    protected:

        void grow() {
            gx_debug_assert(end() != end_of_storage()) ;
            end_++ ;
        }


        template <int DIM2> void copy(const small_set<T,DIM2>& rhs) {
            end_ = data_ ;
            for(typename small_set<T,DIM2>::const_iterator it=rhs.begin(); it!=rhs.end(); it++) {
                push_back(*it) ;
            }
        }


        // Note: maybe we should start from end() instead of begin()
        // since negative indices are inserted first.
        iterator find_i(const T& x) {
            iterator result = begin() ;
            while(result != end() && *result < x) {
                result++ ;
            }
            return result ;
        }

        const_iterator find_i(const T& x) const {
            const_iterator result = begin() ;
            while(result != end() && *result < x) {
                result++ ;
            }
            return result ;
        }

    protected:
        T data_[DIM] ;
        iterator end_ ;
    } ;

    template <class T, int DIM> inline std::ostream& operator<<(
        std::ostream& out,
        const small_set<T, DIM>& S
    ) {
        S.print(out) ; return out ;
    }


    template <class T, int DIM1, int DIM2, int DIM3> inline void sets_intersect(
        const small_set<T,DIM1>& S1, const small_set<T,DIM2>& S2, small_set<T,DIM3>& I
    ) {
        I.clear() ;
        typename small_set<T,DIM1>::const_iterator i1 = S1.begin() ;
        typename small_set<T,DIM2>::const_iterator i2 = S2.begin() ; 
        while(i1 < S1.end() && i2 < S2.end()) {
            if(*i1 < *i2) 
                ++i1 ;
            else if (*i2 < *i1) 
                ++i2 ;
            else {
                I.push_back(*i1) ;
                ++i1 ;
                ++i2 ;
            }
        }
    }

    //--------------------------------------------------------------------------------

    template <class T, int DIM> inline std::ostream& operator<<(
        std::ostream& out,
        const std::set<T>& S
    ) {
        out << "[ " ;
        for(typename std::set<T>::const_iterator it = S.begin(); it != S.end(); it++) {
            out << *it << " " ;
        }
        out << "]" ;
        return out ;
    }
    
    template <class T> inline void sets_intersect(
        const std::set<T>& S1, const std::set<T>& S2, std::set<T>& I
    ) {
        I.clear() ;
        std::set_intersection(
            S1.begin(), S1.end(), S2.begin(), S2.end(), std::inserter(I, I.begin())
        ) ;
    }
    
    //--------------------------------------------------------------------------------

    template <class T> class CRSArray {
    public:
        CRSArray() : in_row_(false) { rowptr_.push_back(0) ; }
        void clear() { 
            rowptr_.resize(0); 
            data_.resize(0) ; 
            rowptr_.push_back(0) ; 
        }

        unsigned int nb_rows() const { return rowptr_.size() - 1 ; }
        unsigned int row_begin(unsigned int r) {
            gx_debug_assert(r < nb_rows()) ;
            return rowptr_[r] ;
        }
        unsigned int row_end(unsigned int r) {
            gx_debug_assert(r < nb_rows()) ;
            return rowptr_[r+1] ;
        }
        T& operator[](unsigned int i) { return data_[i] ; }
        const T& operator[](unsigned int i) const { return data_[i] ; }        
        T& item(unsigned int i) { return data_[i] ; }
        const T& item(unsigned int i) const { return data_[i] ; }        
        
        void begin_row() { 
            gx_debug_assert(!in_row_) ; 
            in_row_ = true ;
        }
        void end_row() {
            gx_debug_assert(in_row_) ;
            in_row_ = false ;
            rowptr_.push_back(data_.size()) ;
        }
        void append_to_row(const T& x) {
            gx_debug_assert(in_row_) ;
            data_.push_back(x) ;
        }

    private:
        std::vector<unsigned int> rowptr_ ;
        std::vector<T> data_ ;
        bool in_row_ ;
    } ;
    

    //----------------------------------------------------------------------------
}


#endif
