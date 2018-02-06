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
 
 
 
 
 
 
 

#include <Geex/numerics/sparse_matrix.h>
#include <algorithm>
#include <iomanip>

namespace Geex {

//_________________________________________________________

    void SparseRowColumn::add(index_t index, double val) {
        Coeff* coeff = nil ;
        // Search for a_{index}
        for(index_t ii=0; ii < nb_coeffs_; ii++) {
            if(coeff_[ii].index == index) {
                coeff = &(coeff_[ii]) ;
                break ;
            }
        }
        if(coeff != nil) {
            coeff->a += val ;
        } else {
            nb_coeffs_++ ;
            if(nb_coeffs_ > capacity_) {
                grow() ;
            }
            coeff = &(coeff_[nb_coeffs_ - 1]) ;
            coeff->a     = val ;
            coeff->index = index ;
        }
    }
    
    void SparseRowColumn::grow() {
        size_t old_capacity = capacity_ ;
        capacity_ = capacity_ * 2 ;
        Allocator<Coeff>::reallocate(
            coeff_, old_capacity, capacity_
        ) ;
    }

    class CoeffIndexCompare {
    public:
        bool operator()(const Coeff& c1, const Coeff& c2) {
            return c1.index < c2.index ;
        }
    } ;

    void SparseRowColumn::sort() {
        Coeff* begin = coeff_ ;
        Coeff* end   = coeff_ + nb_coeffs_ ;
        std::sort(begin, end, CoeffIndexCompare()) ;
    }

//_________________________________________________________

    SparseMatrix::SparseMatrix(size_t m, size_t n, Storage storage) {
        storage_ = NONE ;
        allocate(m,n,storage,false) ;
    }        

    SparseMatrix::SparseMatrix(
        size_t n, Storage storage, bool symmetric_storage
    ) {
        storage_ = NONE ;
        allocate(n,n,storage,symmetric_storage) ;
    }        

    SparseMatrix::~SparseMatrix() {
        deallocate() ;
    }

    SparseMatrix::SparseMatrix() {
        m_ = 0 ;
        n_ = 0 ;
        diag_size_ = 0 ;

        row_ = nil ;
        column_ = nil ;
        diag_ = nil ;

        storage_ = NONE ;
        rows_are_stored_ = false ;
        columns_are_stored_ = false ;
        symmetric_storage_ = false ;
        symmetric_tag_ = false ;
    }

    size_t SparseMatrix::nnz() const {
        size_t result = 0 ;
        if(rows_are_stored()) {
            for(index_t i=0; i<m(); i++) {
                result += row(i).nb_coeffs() ;
            }
        } else if(columns_are_stored()) {
            for(index_t j=0; j<n(); j++) {
                result += column(j).nb_coeffs() ;
            }
        } else {
            gx_assert_not_reached ;
        }
        return result ;
    }
        
    void SparseMatrix::deallocate() {
        m_ = 0 ;
        n_ = 0 ;
        diag_size_ = 0 ;

        delete[] row_ ;
        delete[] column_ ;
        delete[] diag_ ;
        row_ = nil ;
        column_ = nil ;
        diag_ = nil ;

        storage_ = NONE ;
        rows_are_stored_ = false ;
        columns_are_stored_ = false ;
        symmetric_storage_ = false ;
    }

    void SparseMatrix::allocate(
        size_t m, size_t n, Storage storage, bool symmetric_storage
    ) {
        if(storage_ != NONE) {
            deallocate() ;
        }

        m_ = m ;
        n_ = n ;
        diag_size_ = gx_min(m,n) ;
        symmetric_storage_ = symmetric_storage ;
        symmetric_tag_ = false ;
        storage_ = storage ;
        switch(storage) {
        case NONE:
            gx_assert(false) ;
            break ;
        case ROWS:
            rows_are_stored_    = true ;
            columns_are_stored_ = false ;
            break ;
        case COLUMNS:
            rows_are_stored_    = false ;
            columns_are_stored_ = true ;
            break ;
        case ROWS_AND_COLUMNS:
            rows_are_stored_    = true ;
            columns_are_stored_ = true ;
            break ;
        }
        diag_ = new double[diag_size_] ;
        for(index_t i=0; i<diag_size_; i++) {
            diag_[i] = 0.0 ;
        }

        if(rows_are_stored_) {
            row_ = new Row[m] ;
        } else {
            row_ = nil ;
        }

        if(columns_are_stored_) {
            column_ = new Column[n] ;
        } else {
            column_ = nil ;
        }
    }

    void SparseMatrix::sort() {
        if(rows_are_stored_) {
            for(index_t i=0; i<m_; i++) {
                row(i).sort() ;
            }
        }
        if(columns_are_stored_) {
            for(index_t j=0; j<n_; j++) {
                column(j).sort() ;
            }
        }
    }

    void SparseMatrix::zero() {
        if(rows_are_stored_) {
            for(index_t i=0; i<m_; i++) {
                row(i).zero() ;
            }
        }
        if(columns_are_stored_) {
            for(index_t j=0; j<n_; j++) {
                column(j).zero() ;
            }
        }
        for(index_t i=0; i<diag_size_; i++) {
            diag_[i] = 0.0 ;
        }
    }

    void SparseMatrix::clear() {
        if(rows_are_stored_) {
            for(index_t i=0; i<m_; i++) {
                row(i).clear() ;
            }
        }
        if(columns_are_stored_) {
            for(index_t j=0; j<n_; j++) {
                column(j).clear() ;
            }
        }
        for(index_t i=0; i<diag_size_; i++) {
            diag_[i] = 0.0 ;
        }
    }



//----------------------------------------------------------------------
//     Helpers for sparse matrix * vector product
//     according to storage modes:
//           rows or columns
//           symmetric
//----------------------------------------------------------------------

    static void mult_rows_symmetric(
        const SparseMatrix& A, const double* x, double* y
    ) {
        size_t m = A.m() ;
        for(index_t i=0; i<m; i++) {
            y[i] = 0 ;
            const SparseMatrix::Row& Ri = A.row(i) ;
            for(index_t  ij=0; ij<Ri.nb_coeffs(); ij++) {
                const Coeff& c = Ri.coeff(ij) ;
                y[i]       += c.a * x[c.index] ;
                if(i != c.index) {
                    y[c.index] += c.a * x[i] ;
                }
            }
        }
    }

    static void mult_rows(
        const SparseMatrix& A, const double* x, double* y
    ) {
        size_t m = A.m() ;
        for(index_t i=0; i<m; i++) {
            y[i] = 0 ;
            const SparseMatrix::Row& Ri = A.row(i) ;
            for(index_t ij=0; ij<Ri.nb_coeffs(); ij++) {
                const Coeff& c = Ri.coeff(ij) ;
                y[i] += c.a * x[c.index] ;
            }
        }
    }


    static void mult_cols_symmetric(
        const SparseMatrix& A, const double* x, double* y
    ) {
        ::memset(y, 0, A.m() * sizeof(double)) ;
        size_t n = A.n() ;
        for(index_t j=0; j<n; j++) {
            const SparseMatrix::Column& Cj = A.column(j) ;
            for(index_t ii=0; ii<Cj.nb_coeffs(); ii++) {
                const Coeff& c = Cj.coeff(ii) ;
                y[c.index] += c.a * x[j]       ;
                if(j != c.index) {
                    y[j]       += c.a * x[c.index] ;
                }
            }
        }
    }

    static void mult_cols(
        const SparseMatrix& A, const double* x, double* y
    ) {
        ::memset(y, 0, A.m() * sizeof(double)) ;
        size_t n = A.n() ;
        for(index_t j=0; j<n; j++) {
            const SparseMatrix::Column& Cj = A.column(j) ;
            for(index_t ii=0; ii<Cj.nb_coeffs(); ii++) {
                const Coeff& c = Cj.coeff(ii) ;
                y[c.index] += c.a * x[j] ;
            }
        }
    }


    // general driver routine for 
    // sparse matrix * vector product
    void mult(const SparseMatrix& A, const double* x, double* y) {
        if(A.rows_are_stored()) {
            if(A.has_symmetric_storage()) {
                mult_rows_symmetric(A, x, y) ;
            } else {
                mult_rows(A, x, y) ;
            }
        } else {
            if(A.has_symmetric_storage()) {
                mult_cols_symmetric(A, x, y) ;
            } else {
                mult_cols(A, x, y) ;
            }
        }
    }

//----------------------------------------------------------------------
//     Helpers for transpose sparse matrix * vector product
//     according to storage modes:
//           rows or columns
//           symmetric
//     Note: in the symmetric case, A = A^t (and we can reuse the
//      non-transpose product)        
//----------------------------------------------------------------------

    static void mult_xpose_rows(
        const SparseMatrix& A, const double* x, double* y
    ) {
        ::memset(y, 0, A.n() * sizeof(double)) ;
        size_t m = A.m() ;
        for(index_t i=0; i<m; i++) {
            const SparseMatrix::Row& Ri = A.row(i) ;
            for(index_t ij=0; ij<Ri.nb_coeffs(); ij++) {
                const Coeff& c = Ri.coeff(ij) ;
                y[c.index] += c.a * x[i] ;
            }
        }
    }

    static void mult_xpose_cols(
        const SparseMatrix& A, const double* x, double* y
    ) {
        size_t n = A.n() ;
        for(index_t j=0; j<n; j++) {
            y[j] = 0.0 ;
            const SparseMatrix::Column& Cj = A.column(j) ;
            for(index_t ii=0; ii<Cj.nb_coeffs(); ii++) {
                const Coeff& c = Cj.coeff(ii) ;
                y[j] += c.a * x[c.index] ;
            }
        }
    }


    // general driver routine for 
    // transpose sparse matrix * vector product
    void mult_transpose(
        const SparseMatrix& A, const double* x, double* y
    ) {
        if(A.rows_are_stored()) {
            if(A.has_symmetric_storage()) {
                mult_rows_symmetric(A, x, y) ;
            } else {
                mult_xpose_rows(A, x, y) ;
            }
        } else {
            if(A.has_symmetric_storage()) {
                mult_cols_symmetric(A, x, y) ;
            } else {
                mult_xpose_cols(A, x, y) ;
            }
        }
    }

    //_____________________________________________________________

}


