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
 
 
 
 
 
 
 
 
#ifndef __GEEX_BASICS_SMART_POINTER__
#define __GEEX_BASICS_SMART_POINTER__

#include <Geex/basics/assert.h>
#include <Geex/basics/types.h>

namespace Geex {

//____________________________________________________________________________

/**
 * Automatic memory management using reference counting. 
 * This class can be used with classes inheriting
 * the Counted class.
 * @see Counted
 */

    template <class T> class SmartPointer {

    public:
        SmartPointer() ;
        SmartPointer(T* ptr) ;
        SmartPointer(const SmartPointer<T>& rhs) ;
        ~SmartPointer() ;

        SmartPointer<T>& operator=(T* ptr) ;
        SmartPointer<T>& operator=(const SmartPointer<T>& rhs) ;

        /**
         * Makes the current instance a nil pointer ("forgets" the
         * current reference). 'p.forget();' is a shorthand for
         * 'p = nil;'
         */
        void forget() ;

        T* operator->() const ;
        T& operator*() const ;
        operator T*() const ;

        /**
         * 'if(p.is_nil()) {...}' is a shorthand for 'if(p == nil) {...}'
         */
        bool is_nil() const ;

    protected:
    private:
        T* pointer_ ;
    } ;

//____________________________________________________________________________

    template <class T> inline 
    SmartPointer<T>::SmartPointer() : pointer_(nil) {
    }

    template <class T> inline 
    SmartPointer<T>::SmartPointer(T* ptr) : pointer_(ptr) {
        T::ref(pointer_) ;
    }

    template <class T> inline 
    SmartPointer<T>::SmartPointer(
        const SmartPointer<T>& rhs
    ) : pointer_(rhs) {
        T::ref(pointer_) ;
    } 

    template <class T> inline
    SmartPointer<T>::~SmartPointer() {
        T::unref(pointer_) ;
    }

    template <class T> inline
    SmartPointer<T>& SmartPointer<T>::operator=(T* ptr) {
        if(ptr != pointer_) {
            T::unref(pointer_) ;
            pointer_ = ptr ;
            T::ref(pointer_) ;
        }
        return *this ;
    }

    template <class T> inline
    SmartPointer<T>& SmartPointer<T>::operator=(const SmartPointer<T>& rhs) {
        T* rhs_p = rhs ; 
        if(rhs_p != pointer_) {
            T::unref(pointer_) ;
            pointer_ = rhs_p ;
            T::ref(pointer_) ;
        }
        return *this ;
    }

    template <class T> inline
    void SmartPointer<T>::forget() {
        T::unref(pointer_) ;
        pointer_ = nil ;
    }

    template <class T> inline
    T* SmartPointer<T>::operator->() const {
        gx_assert(pointer_ != nil) ;
        return pointer_ ;
    }

    template <class T> inline
    T& SmartPointer<T>::operator*() const {
        gx_assert(pointer_ != nil) ;
        return *pointer_ ;
    }

    template <class T> inline 
    SmartPointer<T>::operator T*() const {
        return pointer_ ;
    }

    template <class T> inline 
    bool SmartPointer<T>::is_nil() const {
        return (pointer_ == nil) ;
    }

}

#endif
