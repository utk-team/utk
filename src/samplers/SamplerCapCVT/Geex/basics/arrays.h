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
 
#ifndef __GEEX_BASICS_ARRAYS__
#define __GEEX_BASICS_ARRAYS__

#include <Geex/basics/types.h>
#include <Geex/basics/assert.h>
#if defined(__MACH__)
#include <stdlib.h>
#else
#include <malloc.h>
#endif

#ifdef GX_PARANOID
#define GX_ARRAY_BOUND_CHECK
#endif

#ifdef GX_ARRAY_BOUND_CHECK
#define gx_array_check(index, size) gx_range_assert(index, 0, size-1) ;
#else 
#define gx_array_check(index, size) 
#endif

namespace Geex {

//_________________________________________________________


    template <class T> class Array1d {
    public:
        typedef Array1d<T> thisclass ;

        Array1d(size_t size = 0, size_t alignment = 1) {
            data_ = nil ;
            base_mem_ = nil ;
            size_ = 0 ;
            allocate(size, alignment) ;
        } 

        inline ~Array1d() { deallocate() ; }

        /** does not preserve previous values stored in the array */
        void allocate(size_t size, size_t alignment = 1) {
            if(size > size_ ) {
				deallocate() ;
                base_mem_ = (Memory::pointer)malloc(size * sizeof(T) + alignment -1) ;
                Memory::pointer p = base_mem_ ;
                while(Numeric::uint64(p) % alignment) {  p++ ; }
                data_ = (T*)p ;
                for(size_t i=0; i<size; i++) {
                    // Direct call to the constructor, see dlist.h for more explanations.
                    new(&data_[i])T() ;                    
                }
            }
            size_ = size ;
        }

        void set_all(const T& value) {
            for(size_t i=0; i<size_; i++) {
                data_[i] = value ;
            }
        }

        T& operator()(index_t i) {
            gx_array_check(i, size_) ;
            return data_[i] ;
        }

        const T& operator()(index_t i) const {
            gx_array_check(i, size_) ;
            return data_[i] ;
        }

        T& operator[](index_t index) {
            gx_array_check(index, size_) ;
            return data_[index] ;
        }

        const T& operator[](index_t index) const {
            gx_array_check(index, size_) ;
            return data_[index] ;
        }

        T& from_linear_index(index_t index) {
            gx_array_check(index, size_) ;
            return data_[index] ;
        }

        const T& from_linear_index(index_t index) const {
            gx_array_check(index, size_) ;
            return data_[index] ;
        }

        size_t size() const { return size_ ; }
        size_t alignment() const { return alignment_ ; }

        void clear() { allocate(0) ; }

        /** low-level access, for experts only. */
        const T* data() const { return data_ ; }

        /** low-level access, for experts only. */
        T* data() { return data_ ; }

        size_t mem_usage() const {
            return size_ * sizeof(T) + sizeof(thisclass) ;
        }

    protected:
        T* data_ ;
        size_t size_ ;
        Memory::pointer base_mem_ ;
        size_t alignment_ ;

    protected:
        void deallocate() {
            if(size_ != 0) {
                for(index_t i=0; i<size_; i++) {
                    // direct call to the destructor, see dlist.h for more explanations
                    data_[i].~T() ;
                }
                free(base_mem_) ;
                data_ = nil ;
                base_mem_ = nil ;
                size_ = 0 ;
            }
        }

    private:
        Array1d(const thisclass& rhs) ;
        thisclass& operator=(const thisclass& rhs) ;
    } ;

//_________________________________________________________

    template <class T> class Array2d {
    public:
        typedef Array2d<T> thisclass ;

        Array2d(size_t size1 = 0, size_t size2 = 0) {
            data_ = nil ;
            allocate(size1, size2) ;
        } 

        inline ~Array2d() { delete[] data_ ; data_ = nil ; }

        /** does not preserve previous values stored in the array */
        void allocate(size_t size1, size_t size2) {
            delete[] data_ ; 
            size_t size = size1 * size2 ;
            data_ = (size == 0) ? nil : new T[size]; 
            size_[0] = size1 ;
            size_[1] = size2 ;
        }

        void set_all(const T& value) {
            size_t size = size_[0] * size_[1] ;
            for(index_t i=0; i<size; i++) {
                data_[i] = value ;
            }
        }

        T& operator()(index_t index0, index_t index1) {
            gx_array_check(index0, size_[0]) ;
            gx_array_check(index1, size_[1]) ;
            return data_[index1 * size_[0] + index0] ;
        }

        const T& operator()(index_t index0, index_t index1) const {
            gx_array_check(index0, size_[0]) ;
            gx_array_check(index1, size_[1]) ;
            return data_[index1 * size_[0] + index0] ;
        }

        T& from_linear_index(index_t index) {
            gx_array_check(index, size_[0] * size_[1]) ;
            return data_[index] ;
        }

        const T& from_linear_index(index_t index) const {
            gx_array_check(index, size_[0] * size_[1]) ;
            return data_[index] ;
        }

        size_t size(int dim) const { 
            gx_array_check(dim, 2) ;
            return size_[dim] ;
        }

        void clear() { allocate(0,0) ; }

        const T* data() const { return data_ ; }
        T* data() { return data_ ; }

    private:
        T* data_ ;
        size_t size_[2] ;

    private:
        Array2d(const thisclass& rhs) ;
        thisclass& operator=(const thisclass& rhs) ;
    } ;


//_________________________________________________________


    template <class T> class Array3d {
    public:
        typedef Array3d<T> thisclass ;

        Array3d(size_t size1 = 0, size_t size2 = 0, size_t size3 = 0) {
            data_ = nil ;
            allocate(size1, size2, size3) ;
        } 

        inline ~Array3d() { delete[] data_ ; data_ = nil ; }

        /** does not preserve previous values stored in the array */
        void allocate(size_t size1, size_t size2, size_t size3) {
            delete[] data_ ; 
            size_t size = size1 * size2 * size3 ;
            data_ = (size == 0) ? nil : new T[size]; 
            size_[0] = size1 ;
            size_[1] = size2 ;
            size_[2] = size3 ;
        }

        void set_all(const T& value) {
            size_t size = size_[0] * size_[1] * size_[2] ;
            for(index_t i=0; i<size; i++) {
                data_[i] = value ;
            }
        }

        T& operator()(index_t index0, index_t index1, index_t index2) {
            gx_array_check(index0, size_[0]) ;
            gx_array_check(index1, size_[1]) ;
            gx_array_check(index2, size_[2]) ;
            return data_[index0 + size_[0] * (index1 + size_[1] * index2)] ;
        }

        const T& operator()(index_t index0, index_t index1, index_t index2) const {
            gx_array_check(index0, size_[0]) ;
            gx_array_check(index1, size_[1]) ;
            gx_array_check(index2, size_[2]) ;
            return data_[index0 + size_[0] * (index1 + size_[1] * index2)] ;
        }

        T& from_linear_index(index_t index) {
            gx_array_check(index, size_[0] * size_[1] * size_[2]) ;
            return data_[index] ;
        }

        const T& from_linear_index(index_t index) const {
            gx_array_check(index, size_[0] * size_[1] * size_[2]) ;
            return data_[index] ;
        }

        size_t size(int dim) const { 
            gx_array_check(dim, 3) ;
            return size_[dim] ;
        }

        void clear() { allocate(0,0,0) ; }

        const T* data() const { return data_ ; }
        T* data() { return data_ ; }

    private:
        T* data_ ;
        size_t size_[3] ;

    private:
        Array3d(const thisclass& rhs) ;
        thisclass& operator=(const thisclass& rhs) ;
    } ;

//_________________________________________________________

    /**
     * Fixed size array with bound checking, copy and copy constructor.
     * FixedArray1d can also be used as a Property, in contrast with
     * regular C++ arrays.
     */
    template <class T, int N> class FixedArray1d {
    public:
        enum { dimension = N } ;
        typedef FixedArray1d<T,N> thisclass ;

        FixedArray1d() { }
        FixedArray1d(const thisclass& rhs) { copy(rhs) ; }
        thisclass& operator=(const thisclass& rhs) { copy(rhs) ; return *this ; }

        const T& operator()(index_t i) const { 
            gx_array_check(i, N) ;
            return data_[i] ; 
        }

        T& operator()(index_t i) { 
            gx_array_check(i, N) ;
            return data_[i] ; 
        }        

        void set_all(const T& value) {
            for(index_t i=0; i<N; i++) {
                data_[i] = value ;
            }
        }

        const T* data() const { return data_ ; }
        T* data() { return data_ ; }

    protected:
        void copy(const thisclass& rhs) {
            for(index_t i=0; i<N; i++) {
                data_[i] = rhs.data_[i] ;
            }
        }

    private:
        T data_[N] ;
    } ;


//_________________________________________________________

}
#endif

