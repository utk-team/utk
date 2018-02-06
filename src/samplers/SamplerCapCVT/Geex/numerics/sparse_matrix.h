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
 
#ifndef ___GEEX_NUMERICS_SPARSE_MATRIX__
#define ___GEEX_NUMERICS_SPARSE_MATRIX__

#include <Geex/numerics/vector.h>

namespace Geex {

    /**
     * An allocator to create dynamically
     * resizable structures. Note: if needed,
     * a more efficient method may be used.
     */
    template <class T> class Allocator {
    public:
        static inline T* allocate(size_t number) {
            return new T[number] ;
        }
        
        static inline void deallocate(T*& addr) {
            delete[] addr ;
            addr = nil ; // makes it possible to track
                         // access to deallocated memory.
        }
        
        static inline void reallocate(
            T*& addr, size_t old_number, size_t new_number
        ) {
            T* new_addr = new T[new_number] ;
            for(unsigned int i=0; i<old_number; i++) {
                new_addr[i] = addr[i] ;
            }
            delete[] addr ;
            addr = new_addr ;
        }
    } ;
    
//_________________________________________________________

    /**
     * A coefficient of a SparseMatrix.
     */
    class Coeff {
    public:
        double a ;
        index_t index ;
    } ;

    //__________________________________________________

    /**
     * A row or a column of a SparseMatrix. SparseRowColumn is
     * compressed, and stored in the form of a list of
     * (value,index) couples.
     */
    class GEEX_API SparseRowColumn {
    public:
        SparseRowColumn() {
            coeff_ = Allocator<Coeff>::allocate(2) ;
            nb_coeffs_ = 0 ;
            capacity_ = 2 ;
        }

        ~SparseRowColumn() {
            Allocator<Coeff>::deallocate(coeff_) ;
        }

        size_t nb_coeffs() const { return nb_coeffs_ ; }

        Coeff& coeff(index_t ii) { 
            gx_debug_assert(ii < nb_coeffs_) ;
            return coeff_[ii] ;
        }
            
        const Coeff& coeff(index_t ii) const {
            gx_debug_assert(ii < nb_coeffs_) ;
            return coeff_[ii] ;
        }        
            
        /** a_{index} <- a_{index} + val */
        void add(index_t index, double val) ;
            
        /** sorts the coefficients by increasing index */
        void sort() ;


        /** 
         * removes all the coefficients and frees the allocated
         * space.
         */
        void clear() { 
            Allocator<Coeff>::deallocate(coeff_) ;
            coeff_ = Allocator<Coeff>::allocate(2) ;
            nb_coeffs_ = 0 ;
            capacity_ = 2 ;
        }

        /** 
         * removes all the coefficients, but keeps the
         * allocated space, that will be used by subsequent
         * calls to add().
         */
        void zero() { nb_coeffs_ = 0 ; }

    protected:
        void grow() ;
            
    private:
        Coeff* coeff_ ;
        size_t nb_coeffs_ ;
        size_t capacity_ ;
    } ;
        
//________________________________________________________________

    class GEEX_API SparseMatrix {
    public:

        typedef SparseRowColumn Row ;
        typedef SparseRowColumn Column ;

        enum Storage { NONE, ROWS, COLUMNS, ROWS_AND_COLUMNS} ;

        //__________ constructors / destructor _____

        /**
         * Constructs a m*n sparse matrix.
         * @param Storage can be one of ROWS, COLUMNS, ROWS_AND_COLUMNS
         */
        SparseMatrix(size_t m, size_t n, Storage storage = ROWS) ;

        /**
         * Constructs a square n*n sparse matrix.
         * @param Storage can be one of ROWS, COLUMNS, ROWS_AND_COLUMNS
         * @param symmetric_storage if set, only entries a_ij such
         *   that j <= i are stored.
         */
        SparseMatrix(size_t n, Storage storage, bool symmetric_storage) ;

        SparseMatrix() ;

        ~SparseMatrix() ;

        //___________ access ________________________

        size_t m() const {
            return m_ ;
        }

        size_t n() const {
            return n_ ;
        }

        size_t diag_size() const {
            return diag_size_ ;
        }

        /** number of non-zero coefficients */
        size_t nnz() const ;

        bool rows_are_stored() const {
            return rows_are_stored_ ;
        }

        bool columns_are_stored() const {
            return columns_are_stored_ ;
        }

        Storage storage() const {
            return storage_ ;
        }

        bool has_symmetric_storage() const {
            return symmetric_storage_ ;
        }

        bool is_square() const {
            return (m_ == n_) ;
        }

        bool is_symmetric() const {
            return (symmetric_storage_ || symmetric_tag_) ;
        }

        /**
         * For symmetric matrices that are not stored in symmetric mode,
         * one may want to give a hint that the matrix is symmetric.
         */
        void set_symmetric_tag(bool x) {
            symmetric_tag_ = x ;
        }

        /**
         * storage should be one of ROWS, ROWS_AND_COLUMNS
         * @param i index of the row, in the range [0, m-1]
         */
        Row& row(index_t i) {
            gx_debug_assert(i < m_) ;
            gx_debug_assert(rows_are_stored_) ;
            return row_[i] ;
        }

        /**
         * storage should be one of ROWS, ROWS_AND_COLUMNS
         * @param i index of the row, in the range [0, m-1]
         */
        const Row& row(index_t i) const {
            gx_debug_assert(i < m_) ;
            gx_debug_assert(rows_are_stored_) ;
            return row_[i] ;
        }

        /**
         * storage should be one of COLUMN, ROWS_AND_COLUMNS
         * @param i index of the column, in the range [0, n-1]
         */
        Column& column(index_t j) {
            gx_debug_assert(j < n_) ;
            gx_debug_assert(columns_are_stored_) ;
            return column_[j] ;
        }

        /**
         * storage should be one of COLUMNS, ROWS_AND_COLUMNS
         * @param i index of the column, in the range [0, n-1]
         */
        const Column& column(index_t j) const {
            gx_debug_assert(j < n_) ;
            gx_debug_assert(columns_are_stored_) ;
            return column_[j] ;
        }
        
        /**
         * returns aii.
         */
        double diag(index_t i) const {
            gx_debug_assert(i < diag_size_) ;
            return diag_[i] ;
        }

        /**
         * aij <- aij + val
         */
        void add(index_t i, index_t j, double val) {
            gx_debug_assert(i < m_) ;
            gx_debug_assert(j < n_) ;
            if(symmetric_storage_ && j > i) {
                return ;
            }
            if(i == j) {
                diag_[i] += val ;
            } 
            if(rows_are_stored_) {
                row(i).add(j, val) ;
            }
            if(columns_are_stored_) {
                column(j).add(i, val) ;
            }
        }
            

        /** sorts rows and columns by increasing coefficients */
        void sort() ;
            

        /**
         * removes all the coefficients and frees the allocated
         * space.
         */
        void clear() ;

        /**
         * removes all the coefficients, but keeps the allocated
         * storage, that will be used by subsequent calls to add().
         */
        void zero() ;

        void allocate(
            size_t m, size_t n, Storage storage, bool symmetric = false
        ) ;

        void deallocate() ;

    private:
        size_t m_ ;
        size_t n_ ;
        size_t diag_size_ ;

        SparseRowColumn* row_ ;
        SparseRowColumn* column_ ;
        double* diag_ ;

        Storage storage_ ;
        bool rows_are_stored_ ;
        bool columns_are_stored_ ;
        bool symmetric_storage_ ;
        bool symmetric_tag_ ;

        // SparseMatrix cannot be copied.
        SparseMatrix(const SparseMatrix& rhs) ;
        SparseMatrix& operator=(const SparseMatrix& rhs) ;
    } ;


    /**
     * Matrix * vector product: y = A.x
     */
    void GEEX_API mult(const SparseMatrix& A, const double* x, double* y) ;    

    inline void mult(
        const SparseMatrix& A, const Vector& x, Vector& y
    ) {
        gx_assert(x.size() == A.n()) ;
        gx_assert(y.size() == A.m()) ;
        mult(A, x.data(), y.data()) ;
    }

    /**
     * Matrix * vector product: y = At.x
     */
    void GEEX_API mult_transpose(
        const SparseMatrix& A, const double* x, double* y
    ) ;    

    inline void mult_transpose(
        const SparseMatrix& A, const Vector& x, Vector& y
    ) {
        gx_assert(x.size() == A.m()) ;
        gx_assert(y.size() == A.n()) ;
        mult_transpose(A, x.data(), y.data()) ;
    }

}
#endif

