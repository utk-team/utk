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

#ifndef __GEEX_NUMERICS_SPARSE_MATRIX_COORDS__
#define __GEEX_NUMERICS_SPARSE_MATRIX_COORDS__

#include <Geex/numerics/sparse_matrix_crs.h>
#include <Geex/numerics/sparse_matrix_conversion.h>

namespace Geex {

    //________________________________________________________________________

    template <class T, int BLOCM, int BLOCN> class SparseMatrixBlockCoord {
    public:
        typedef Array1d<index_t> IndexArray ;
        typedef Array1d<T> CoeffArray ;
        enum { BM=BLOCM } ;
        enum { BN=BLOCN } ;
        enum { BLOC_SIZE=BM*BN } ;

        SparseMatrixBlockCoord() : M_(0), N_(0) { }

        /** number of bloc rows */
        size_t M() const { return M_ ; }
        /** number of bloc columns */
        size_t N() const { return N_ ; }
        /** number of rows */
        size_t m() const { return M() * BM ; } 
        /** number of columns */
        size_t n() const { return N() * BN ; } 

        inline void mult(const T* x, T* y) const ;
        inline void mult(const Array1d<T>& x, Array1d<T>& y) const {
            mult(x.data(), y.data()) ;
        }

        IndexArray rowind ;
        IndexArray colind ;
        CoeffArray a ;
        size_t M_ ;
        size_t N_ ;
    } ;

    //________________________________________________________________________

    template <class T, int N> inline SparseMatrix<T,N>::mult(const T* x, T* y) {
        QuickBLAS::flops_counter += 2*a.size() ;
        // TODO.
    }

    //________________________________________________________________________

}

#endif
