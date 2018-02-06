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
 
#ifndef __GEEX_NUMERICS_SPARSE_MATRIX_CRS__
#define __GEEX_NUMERICS_SPARSE_MATRIX_CRS__

#include <Geex/basics/arrays.h>
#include <Geex/numerics/sparse_matrix_sse.h>
#include <Geex/numerics/quick_blas.h>

#include <iostream>
#include <set>

namespace Geex {

    class Permutation : public Array1d<index_t> {
    public:
        typedef Array1d<index_t> superclass ;

        /**
         * Constructs the identity permutation.
         */
        Permutation() : superclass() { }

        /**
         * Copy constructor (only allowed for identity permutation, used by 
         * the default argument of some functions).
         */
        Permutation(const Permutation& rhs) : superclass() { 
            gx_assert(rhs.size() == 0) ;  
        }

        void load_identity() { clear() ; }

        bool is_identity() const { return size() == 0 ; }

        /**
         * result <- permutation^{-1}
         */
        void invert(Permutation& result) const ;

        /** 
         * Computes the permutation applied to i.
         */
        index_t operator()(index_t i) const {
            return is_identity() ? i : (*this)[i] ;
        }

        /**
         * result <- permutation(x)
         */
        template<class T1, class T2> inline void permute_vector(
            const Array1d<T1>& x, Array1d<T2>& result
        ) const ;

        /**
         * result <- permutation^{-1}(x)
         */
        template<class T1, class T2> inline void invert_permute_vector(
            const Array1d<T1>& x, Array1d<T2>& result
        ) const ;

    } ;

    /**
     * SparseMatrixPattern in Compressed Row Storage mode.
     */
    class SparseMatrixPatternCRS {
    public:
        typedef Array1d<index_t> IndexArray ;
        enum { NO_INDEX = ~0 } ;

        SparseMatrixPatternCRS() {
            N = 0 ;
            symmetric_storage = false ;
        }

        void clear() {
            N = 0 ;
            symmetric_storage = false ;
            colind.clear() ;
            rowptr.clear() ;
        }


        /** number of rows */
        size_t m() const { return rowptr.size() - 1 ; }

        /** number of columns */
        size_t n() const { return N ; }

        /** number of non-zero coefficients */
        size_t nnz() const { return colind.size() ; }

        /** number of non-zero coefficients in row i */
        size_t row_nnz(index_t i) const { 
            return rowptr[i+1] - rowptr[i] ;
        }

        /**
         * if symmetric storage is used, this function reconstructs the
         * pattern without the symmetric storage.
         */
        void expand_symmetric_pattern(SparseMatrixPatternCRS& rhs) const ;

        /** Compute Reverse Cuthill McKee ordering */
        void compute_RCMK_ordering(Permutation& result) ;

        /** 
         * Returns the maximum distance between a non-zero coefficient
         * and the diagonal.
         */
        size_t bandwidth() const ;

        size_t mem_usage() const {
            return colind.mem_usage() + rowptr.mem_usage() + sizeof(size_t) + sizeof(bool) ;
        }

    public:
        IndexArray colind ;
        IndexArray rowptr ;

        size_t N ;
        bool symmetric_storage ;

    protected:
        // Used by compute_RCMK_ordering(),
        // returns -1 if no furthest node is found.
        signed_index_t find_furthest_node(index_t x, IndexArray& dist) ;
    } ;

//______________________________________________________________________________

    template<class T> class SparseMatrixCRS : public SparseMatrixPatternCRS {
    public:
        typedef Array1d<T> CoeffArray ;

        inline SparseMatrixCRS() : separate_diag(true) { }
        inline void permute(const Permutation& permutation, SparseMatrixCRS& rhs) ;
        inline void mult(const T* x, T* y) const ;
        inline void mult(const Array1d<T>& x, Array1d<T>& y) const {
            mult(x.data(), y.data()) ;
        }
        inline void print(std::ostream& out) ;
        inline double filling_ratio() const ;        

        size_t mem_usage() const {
            return SparseMatrixPatternCRS::mem_usage() + a.mem_usage() + diag.mem_usage() + sizeof(bool) ;
        }

        CoeffArray a ;
        CoeffArray diag ;
        bool separate_diag ;
    } ;
        
//______________________________________________________________________________

    /**
     * The base class for all bloc compressed row storage matrices.
     */
    template <class T, int BLOCM, int BLOCN> class GenericSparseMatrixBCRS {
    public:
        typedef Array1d<index_t> IndexArray ;
        typedef Array1d<T> CoeffArray ;
        enum { BM=BLOCM } ;
        enum { BN=BLOCN } ;
        enum { BLOC_SIZE=BM*BN } ;

        inline GenericSparseMatrixBCRS() : N_(0) { } 

        inline double filling_ratio() const ;

        /** number of bloc rows */
        size_t M() const { return rowptr.size() - 1 ; }
        /** number of bloc columns */
        size_t N() const { return N_ ; }
        /** number of rows */
        size_t m() const { return M() * BM ; } 
        /** number of columns */
        size_t n() const { return N() * BN ; } 

        size_t number_of_blocs_in_row(index_t i) {
            return rowptr[i+1] - rowptr[i] ;
        }

        size_t max_number_of_blocs_in_row() {
            size_t result = 0 ;
            for(index_t i=0; i<M(); i++) {
                result = gx_max(result, number_of_blocs_in_row(i)) ;
            }
            return result ;
        }

        inline void mult(const T* x, T* y) const ;
        inline void mult(const Array1d<T>& x, Array1d<T>& y) const {
            mult(x.data(), y.data()) ;
        }

        void print(std::ostream& out) const ;

        size_t mem_usage() const {
            return 
                rowptr.mem_usage() + 
                colind.mem_usage() + 
                a.mem_usage() + 
                diag.mem_usage() + 
                sizeof(size_t) ;
        }

        IndexArray rowptr ;
        IndexArray colind ;
        CoeffArray a ;
        CoeffArray diag ;
        size_t N_ ;
    } ;

//_____________________________________________________________________________________________

    /**
     * The general implementation of block compressed row storage
     * matrices.
     */
    template <class T, int BLOCM, int BLOCN> class SparseMatrixBCRS : 
        public GenericSparseMatrixBCRS<T, BLOCM, BLOCN> {
    public:
        typedef GenericSparseMatrixBCRS<T, BLOCM, BLOCN> superclass ;
        typedef Array1d<index_t> IndexArray ;
        SparseMatrixBCRS() { }
    } ;

//_____________________________________________________________________________________________

    /**
     * A specialization optimized with SSE2.
     */
    template<> class SparseMatrixBCRS<double,2,2> : public GenericSparseMatrixBCRS<double,2,2> {
    public:
        typedef GenericSparseMatrixBCRS<double,2,2> superclass ;
        SparseMatrixBCRS() { }
        inline void mult(const double* x, double* y) const {
            if(
                sparse_matrix_bcrs_double_2_2_mult_SSE2(
                    M(), a.data(), colind.data(), rowptr.data(), x, y
                )
            )  {
                QuickBLAS::flops_counter += (a.size() * 2) ;
            } else {
                superclass::mult(x,y) ;
            }
        }
        inline void mult(const Array1d<double>& x, Array1d<double>& y) const {
            mult(x.data(), y.data()) ;
        }
    } ;

//_____________________________________________________________________________________________

    /**
     * A specialization optimized with SSE2.
     */
    template<> class SparseMatrixBCRS<double,4,2> : public GenericSparseMatrixBCRS<double,4,2> {
    public:
        typedef GenericSparseMatrixBCRS<double,4,2> superclass ;
        SparseMatrixBCRS() { }
        inline void mult(const double* x, double* y) const {
            if(sparse_matrix_bcrs_double_4_2_mult_SSE2(M(), a.data(), colind.data(), rowptr.data(), x, y)) {
                QuickBLAS::flops_counter += (a.size() * 2) ;
            } else {
                superclass::mult(x,y) ;
            }
        }
        inline void mult(const Array1d<double>& x, Array1d<double>& y) const {
            mult(x.data(), y.data()) ;
        }
    } ;

//_____________________________________________________________________________________________

    /**
     * A specialization optimized with SSE2.
     */
    template<> class SparseMatrixBCRS<double,4,4> : public GenericSparseMatrixBCRS<double,4,4> {
    public:
        typedef GenericSparseMatrixBCRS<double,4,4> superclass ;
        SparseMatrixBCRS() { }
        inline void mult(const double* x, double* y) const {
            if(sparse_matrix_bcrs_double_4_4_mult_SSE2(M(), a.data(), colind.data(), rowptr.data(), x, y)) {
                QuickBLAS::flops_counter += (a.size() * 2) ;
            } else {
                superclass::mult(x,y) ;
            }
        }
        inline void mult(const Array1d<double>& x, Array1d<double>& y) const {
            mult(x.data(), y.data()) ;
        }
    } ;

//_____________________________________________________________________________________________

    /**
     * A specialization optimized with SSE2.
     */
    template<> class SparseMatrixBCRS<float,4,4> : public GenericSparseMatrixBCRS<float,4,4> {
    public:
        typedef GenericSparseMatrixBCRS<float,4,4> superclass ;
        SparseMatrixBCRS() { }
        inline void mult(const float* x, float* y) const {
            if(sparse_matrix_bcrs_float_4_4_mult_SSE2(M(), a.data(), colind.data(), rowptr.data(), x, y)) {
                QuickBLAS::flops_counter += (a.size() * 2) ;
            } else {
                superclass::mult(x,y) ;
            }
        }
        inline void mult(const Array1d<float>& x, Array1d<float>& y) const {
            mult(x.data(), y.data()) ;
        }
    } ;

//_____________________________________________________________________________________________

}


namespace Geex {

//_____________________________________________________________

    template<class T1, class T2> inline void Permutation::permute_vector(
        const Array1d<T1>& x, Array1d<T2>& result
    ) const {
        // Note: x and result's size may be larger than this->size(),
        // due to alignment, therefore we check for >=.
        gx_assert(x.size() >= size()) ;
        gx_assert(result.size() >= size()) ;
        if(is_identity()) {
            for(index_t i=0; i<gx_min(x.size(), result.size()); i++) {
                result[i] = T2(x[i]) ;
            }
        } else {
            for(index_t i=0; i<size(); i++) {
                result[i] = T2(x[(*this)[i]]) ;
            }
        }
    }

    template<class T1, class T2> inline void Permutation::invert_permute_vector(
        const Array1d<T1>& x, Array1d<T2>& result
    ) const {
        // Note: x and result's size may be larger than this->size(),
        // due to alignment, therefore we check for >=.
        gx_assert(x.size() >= size()) ;
        gx_assert(result.size() >= size()) ;
        if(is_identity()) {
            for(index_t i=0; i<gx_min(x.size(), result.size()); i++) {
                result[i] = T2(x[i]) ;
            }
        } else {
            for(index_t i=0; i<size(); i++) {
                result[(*this)[i]] = T2(x[i]) ;
            }
        }
    }
        
//_____________________________________________________________

    template <class T> inline void 
    SparseMatrixCRS<T>::mult(const T* x, T* y) const {
        QuickBLAS::flops_counter += 2*a.size() ;
        if(separate_diag) {
            QuickBLAS::flops_counter += 2*diag.size() ;
        }
        if(separate_diag) {
            if(symmetric_storage) {
                // Symmetric storage and separate diagonal
                for(index_t i=0; i<diag.size(); i++) {
                    y[i] = x[i] * diag[i] ;
                }
                for(index_t i=diag.size(); i<m(); i++) {
                    y[i] = 0.0 ;
                }
                for(index_t i=0; i<m(); i++) {
                    for(index_t j=rowptr[i] ; j<rowptr[i+1]; j++) {
                        y[i] += a[j] * x[colind[j]] ;
                        y[colind[j]] += a[j] * x[i] ;                        
                    }
                }
            } else {
                // No symmetric storage and separate diagonal
                for(index_t i=0; i<m(); i++) {
                    T sum = 0.0 ;
                    for(index_t j=rowptr[i] ; j<rowptr[i+1]; j++) {
                        sum += a[j] * x[colind[j]] ;
                    }
                    y[i] = sum ;
                }
                for(index_t i=0; i<diag.size(); i++) {
                    y[i] += x[i] * diag[i] ;
                }
            }
        } else {
            if(symmetric_storage) {
                // Symmetric storage, no separate diagonal
                Memory::clear(y, n() * sizeof(T)) ;
                for(index_t i=0; i<m(); i++) {
                    for(index_t j=rowptr[i] ; j<rowptr[i+1]; j++) {
                        y[i] += a[j] * x[colind[j]] ;
                        if(colind[j] != i) {
                            y[colind[j]] += a[j] * x[i] ;                        
                        }
                    }
                }
            } else {
                // No symmetric storage, no separate diagonal
                for(index_t i=0; i<m(); i++) {
                    T sum = 0.0 ;
                    for(index_t j=rowptr[i] ; j<rowptr[i+1]; j++) {
                        sum += a[j] * x[colind[j]] ;
                    }
                    y[i] = sum ;
                }
            }
        }
    }

    template <class T> inline void 
    SparseMatrixCRS<T>::print(std::ostream& out) {
        for(index_t i=0; i<m(); i++) {
            for(index_t jj=rowptr[i]; jj<rowptr[i+1]; jj++) {
                if(a[jj] != 0.0) {
                    index_t j = colind[jj] ;
                    out << i << " " << j << " " << a[jj] << std::endl ;
                    if(symmetric_storage && j != i) {
                        out << j << " " << i << " " << a[jj] << std::endl ;
                    }
                }
            }
        }
        if(separate_diag) {
            for(index_t i=0; i<diag.size(); i++) {
                if(diag[i] != 0.0) {
                    out << i << " " << i << " " << diag[i] << std::endl ;
                }
            }
        }
    }

    template <class T> inline 
    double SparseMatrixCRS<T>::filling_ratio() const {
        size_t non_zeros = 0 ;
        for(index_t i=0; i<a.size(); i++) {
            if(a[i] != 0.0) {
                non_zeros++ ;
            }
        }
        return double(non_zeros) / double(a.size()) ;
    }


//_____________________________________________________________

    template <class T, int BM, int BN> inline
    void GenericSparseMatrixBCRS<T,BM,BN>::mult(const T* x, T* y) const {
        QuickBLAS::flops_counter += 2*a.size() ;
        index_t y_base = 0 ;
        for(index_t I=0; I<M(); I++) {
            T sum[BM] ;
            for(index_t i=0; i<BM; i++) { sum[i] = 0 ; }
            for(index_t JJ=rowptr[I]; JJ<rowptr[I+1]; JJ++) {
                index_t ofs = 0 ;
                index_t J = colind[JJ] ;
                index_t a_base = JJ*BLOC_SIZE ;
                index_t x_base = J * BN ;
                for(index_t di=0; di<BM; di++) {
                    for(index_t dj=0; dj<BN; dj++) {
                        sum[di] += a[a_base+ofs] * x[x_base+dj] ;
                        ofs++ ;
                    }
                }
            }
            for(index_t i=0; i<BM; i++) {
                y[y_base + i] = sum[i] ;
            }
            y_base += BM ;
        }
    }

    template <class T, int BM, int BN> inline
    void GenericSparseMatrixBCRS<T,BM,BN>::print(std::ostream& out) const {
        for(index_t I=0; I<M(); I++) {
            for(index_t JJ=rowptr[I]; JJ<rowptr[I+1]; JJ++) {
                index_t ofs = 0 ;
                index_t J = colind[JJ] ;
                for(index_t di=0; di<BM; di++) {
                    for(index_t dj=0; dj<BN; dj++) {
                        if(a[JJ * BLOC_SIZE + ofs] != 0.0) {
                            out << (I * BM + di) << " " << (J * BN + dj) << " " << a[JJ * BLOC_SIZE + ofs] << std::endl ;
                        }
                        ofs++ ;
                    }
                }
            }
        }
    }

    template <class T, int BM, int BN> inline 
    double GenericSparseMatrixBCRS<T,BM,BN>::filling_ratio() const {
        size_t non_zeros = 0 ;
        for(index_t i=0; i<a.size(); i++) {
            if(a[i] != 0.0) {
                non_zeros++ ;
            }
        }
        return double(non_zeros) / double(a.size()) ;
    }

//_____________________________________________________________

}

#endif

