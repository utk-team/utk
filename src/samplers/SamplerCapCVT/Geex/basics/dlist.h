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
 
#ifndef __GEEX_BASICS_DLIST__
#define __GEEX_BASICS_DLIST__

#include <Geex/basics/types.h>
#include <Geex/basics/assert.h>

#include <vector>
#include <string.h>


// Note: if GX_DLIST_CHECK is defined,
// some operations (such as destroy()) can
// have linear complexity (rather than constant).

#ifdef GX_PARANOID
#define GX_DLIST_CHECK
#endif

#ifdef GX_DLIST_CHECK
#define gx_dlist_assert(x) gx_assert(x)
#else
#define gx_dlist_assert(x)
#endif

// If your compiler barks somewhere in this file, it is
// likely that it lacks the new.h header, in that case,
// uncomment the following line:
// #define DOES_NOT_HAVE_NEW_H

#ifdef DOES_NOT_HAVE_NEW_H
inline void *operator new(size_t, void* p) { return p; }
#else
#include <new>
#endif

// The following code makes use of C++ oddities,
// enabling to separate memory management from
// object initialization and termination. 
// The correct way to call a constructor for an
// object of class T located at address addr is:
//    new(addr) T()
// which is the placement syntax for operator new.
// This means that new() uses the address addr instead
// of allocating space for the object.
// The correct way to call a destructor for an 
// object of class T located at address addr is:
//    addr-> ~T()
// Note that the placement syntax of operator new and
// direct calls to the destructor DO NOT allocate NOR
// deallocate memory.


namespace Geex {

//_________________________________________________________

    template <class T> class DListItr ;

    /**
     * DList is a collection and an allocator
     * at the same time. It is internally represented
     * by a doubly connected list. The nodes of the list
     * are allocated in chunks, which both improves the
     * speed and reduces the storage. 
     */

    template <class T>  class DList {
    public:
        typedef T value_type ;
        enum { chunk_size = 128 } ;

        /**
         * Internal representation of the doubly connected
         * list. Client code does not need to access this
         * class directly.
         */
        struct Node {
            char data_[sizeof(T)] ;
            Node* prev_ ;
            Node* next_ ;
            T* data() { return reinterpret_cast<T*>(&data_) ;  }
            const T* data() const { 
                return reinterpret_cast<const T*>(&data_) ;  
            }
        
            /** 
             * sanity check: allocated memory is set to zero
             * (enables uninitialized pointers to be detected
             * by VM).
             */
            void clear_data() { Memory::clear(data_, sizeof(T)) ; }
            
            bool is_free() const {
                return (prev_ == nil) ;
            }
            
            bool is_used() const {
                return ((prev_ != nil) && (next_ != nil)) ;
            }
            
        } ;

    protected:
        bool node_is_in_list(Node* node, Node* list) {
            for(Node* i = list->next_; i != list; i = i->next_) {
                if(i == node) {
                    return true ;
                }
            }
            return false ;
        }

        void append_node_to_list(Node* node, Node* list) {
            gx_assert(node != list) ;
            node-> next_ = list ;
            list-> prev_-> next_ = node ;
            node-> prev_ = list-> prev_ ;
            list-> prev_ = node ;
        }

        void remove_node_from_list(Node* node) {
            node-> prev_-> next_ = node-> next_ ;
            node-> next_-> prev_ = node-> prev_ ;
        }

        void init() {
            end_ = &end_node_ ;
            end_-> next_ = end_ ;
            end_-> prev_ = end_ ;
            inactive_end_ = &inactive_end_node_ ;
            inactive_end_-> next_ = inactive_end_ ;
            inactive_end_-> prev_ = inactive_end_ ;
            size_ = 0 ;
            free_list_ = nil ;
        }

    public:

        class iterator {
        public:
            iterator(const iterator& rhs) : ptr_(rhs.ptr_) { }
            iterator(Node* n = nil) : ptr_(n) { }
            T& operator*()  { return *(ptr_-> data()) ; }
            const T& operator*() const { return *(ptr_-> data()) ; }
            T* operator->() { return ptr_-> data() ; }
            const T* operator->() const { return ptr_-> data() ; }
            operator T*()   { return ptr_-> data() ; } 
            iterator operator++() { 
                ptr_ = ptr_-> next_ ; 
                return *this ;
            }
            iterator operator++(int) {
                iterator tmp = *this ;
                ++*this ;
                return tmp ;
            }

            bool operator==(const T* rhs) const {
                return (ptr_ == rhs) ;
            }
            bool operator!=(const T* rhs) const {
                return (ptr_ != rhs) ;
            }

            bool operator==(const iterator& rhs) const {
                return (ptr_ == rhs.ptr_) ;
            }
            bool operator!=(const iterator& rhs) const {
                return (ptr_ != rhs.ptr_) ;
            }

        private:
            Node* ptr_ ;
        } ;

        class const_iterator {
        public:
            const_iterator(const iterator& rhs) {
                ptr_ = reinterpret_cast<const Node*>(&*rhs) ;
            }
            const_iterator(const const_iterator& rhs) : ptr_(rhs.ptr_) { }
            const_iterator(const Node* n = nil) : ptr_(n) { }
            const T& operator*() const { return *(ptr_-> data()) ; }
            const T* operator->() const { return ptr_-> data() ; }
            operator const T*() const { return ptr_-> data() ; } 
            const_iterator operator++() { 
                ptr_ = ptr_-> next_ ; 
                return *this ;
            }
            const_iterator operator++(int) {
                const_iterator tmp = *this ;
                ++*this ;
                return tmp ;
            }

            bool operator==(const T* rhs) {
                return (ptr_ == rhs) ;
            }
            bool operator!=(const T* rhs) {
                return (ptr_ != rhs) ;
            }
            bool operator==(const const_iterator& rhs) {
                return (ptr_ == rhs.ptr_) ;
            }
            bool operator!=(const const_iterator& rhs) {
                return (ptr_ != rhs.ptr_) ;
            }

        private:
            const Node* ptr_ ;
        } ;

        DList() : nb_locks_(0) {
            init() ;
        }

        /**
         * Frees all the memory associated with the data structure.
         */
        void clear() {
            
            // If an assertion fails here, then probably the list was cleared
            // with an iterator still attached to it.
            gx_assert(!is_locked()) ;

            // Step 1: call the destructors.
            for(
                Node* it = end_-> next_; it != end_; it = it-> next_
            ) {
                // Direct call of the destructor
                // Note that this does not deallocate any
                // memory, it is just the correct way for
                // calling a destructor directly (see 
                // the comments at the beginning of this 
                // file).
                it-> data()-> ~T() ;
            }
            for(
                Node* it=inactive_end_-> next_; it!=inactive_end_; it=it->next_
            ) {
                // Same as before
                it-> data()-> ~T() ;
            }

            // Step 2: deallocate memory.
            for(index_t i=0; i<chunks_.size(); i++) {
                // sanity check: freed memory is set to zero
                // (enables uninitialized pointers to be detected
                // by VM).
                Memory::clear(chunks_[i], sizeof(Node) * chunk_size) ;
                delete[] (chunks_[i]) ;
            }

            // Step 3: clear the chunks vector.
            chunks_.clear() ;

            init() ;
        }
        
        void clear_inactive_items() {
            Node* it = inactive_end_->next_ ;
            while(it != inactive_end_) {
                Node* next = it->next_ ;
                destroy(it->data()) ;
                it = next ;
            } 
        }

        ~DList() {
            // If an assertion fails here, then probably a DList was destroyed before 
            // an iterator attached to it.
            gx_assert(!is_locked()) ;
            clear() ;
        }

        size_t size() const     { return size_ ; }
        size_t capacity() const { 
            return chunks_.size() * chunk_size ;
        }

        iterator begin() const { return iterator(end_-> next_) ; }
        iterator end() const { return iterator(end_) ; }

        /*
         * creates a new element and appends it at the
         * end of the list.
         */

        T* create() {
            
            // If an assertion fails here, then probably an element was created in the list
            // while an iterator was still attached to it.
            gx_assert(!is_locked()) ;

            if(free_list_ == nil) {
                grow() ;
            }
            Node* new_node = free_list_ ;
            free_list_ = free_list_-> next_ ;

            // sanity check
            gx_assert(new_node-> is_free()) ;
            
            // sanity check: allocated memory is set to zero
            // (enables uninitialized pointers to be detected
            // by VM).
            new_node-> clear_data() ;
            
            append_node_to_list(new_node, end_) ;
            size_++ ;

            // Direct call of the constructor
            // (placement syntax for operator new).
            // Note that this does not allocate any
            // memory, it is just the correct way for
            // calling a constructor directly (see 
            // the comments at the beginning of this 
            // file).
            new(new_node-> data()) T() ;
            
            return new_node-> data() ;
        }

        void destroy(T* x) {

            // If an assertion fails here, then probably an element was removed from the list
            // while an iterator was still attached to it.
            gx_assert(!is_locked()) ;
            
            // Direct call of the destructor
            // Note that this does not deallocate any
            // memory, it is just the correct way for
            // calling a destructor directly (see 
            // the comments at the beginning of this 
            // file).
            x-> ~T() ;

            Node* node = reinterpret_cast<Node*>(x) ;

            // If this assertion fails, this
            // may be caused by a double deallocation
            // of the same element, or by a deallocation
            // of a previously deactivated element.
            gx_assert(node-> is_used()) ;
            gx_assert(node != end_ && node != inactive_end_) ;
            remove_node_from_list(node) ;

            size_-- ;
            node-> next_ = free_list_ ;
            node-> prev_ = nil ;
            free_list_ = node ;

            // sanity check: deallocated memory is set to zero
            // (enables access to pointers in deallocated objects
            // to be detected by VM).
            node-> clear_data() ;
        }


        /**
         * Removes an element from the list, without
         * destroying it.
         */
        void deactivate(T* x) {

            // If an assertion fails here, then probably an element was deactivated
            // while an iterator was still attached to the list.
            gx_assert(!is_locked()) ;

            Node* node = reinterpret_cast<Node*>(x) ;
            gx_assert(node != end_ && node != inactive_end_) ;
            gx_dlist_assert(node_is_in_list(node, end_)) ;
            remove_node_from_list(node) ;
            append_node_to_list(node, inactive_end_) ;
            size_-- ;
        }

        /**
         * Adds an element at the end of the list.
         * The element should have been previously
         * created using create() and removed from 
         * the list, using deactivate().
         */
        void activate(T* x) {

            // If an assertion fails here, then probably an element was activated
            // while an iterator was still attached to the list.
            gx_assert(!is_locked()) ;

            Node* node = reinterpret_cast<Node*>(x) ;
            gx_assert(node != end_ && node != inactive_end_) ;
            gx_dlist_assert(node_is_in_list(node, inactive_end_)) ;
            remove_node_from_list(node) ;
            append_node_to_list(node, end_) ;
            size_++ ;
        }

    protected:
        void grow() {
            Node* new_chunk = new Node[chunk_size] ;
            
            // sanity check: allocated memory is set to zero
            // (enables uninitialized pointers to be detected
            // by VM).
            Memory::clear(new_chunk, sizeof(Node) * chunk_size) ;

            Node* last_of_chunk = &(new_chunk[chunk_size - 1]) ;
            for(Node* it = new_chunk; it != last_of_chunk; it++) {
                it-> next_ = it + 1 ;
                it-> prev_ = nil ;
            }
            last_of_chunk-> next_ = free_list_ ;
            last_of_chunk-> prev_ = nil ;
            free_list_ = new_chunk ;
            chunks_.push_back(new_chunk) ;
        }

        void lock() const { nb_locks_++ ; }
        void unlock() const { nb_locks_-- ; gx_assert(nb_locks_ >= 0) ; }
        bool is_locked() const { return (nb_locks_ > 0) ; }

    private:
        Node end_node_ ;
        Node* end_ ;
        Node inactive_end_node_ ;
        Node* inactive_end_ ;
        size_t size_ ;
        Node* free_list_ ;
        std::vector<Node*> chunks_ ;
        mutable int nb_locks_ ;

    private:
        // No copy constructor nor operator= (if you really need
        //  them, I'll implement them ...
        DList(const DList<T>& rhs) ;
        DList<T>& operator=(const DList<T>& rhs) ;

        friend class DListItr<T> ;
    } ;

//_________________________________________________________

}

#endif
