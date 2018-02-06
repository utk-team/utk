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
 
#ifndef ___GEEX_NUMERICS_MATRIX_PARTITION__
#define ___GEEX_NUMERICS_MATRIX_PARTITION__

#include <Geex/numerics/sparse_matrix.h>

namespace Geex {

    /**
     * Given a sparse matrix and a desired number of parts, MatrixPartition computes a balanced partition of the graph defined
     * by the non-zero entries of the sparse matrix.
     */
    class GEEX_API MatrixPartition {
    public:
        virtual void partition_matrix(const SparseMatrix* M, int nb_parts, Array1d<int>& part) = 0 ;
        virtual ~MatrixPartition() ; 
    } ;

    /**
     * Implementation of MatrixPartition based on METIS.
     */
    class GEEX_API MatrixPartition_METIS : public MatrixPartition {
    public:
        virtual void partition_matrix(const SparseMatrix* M, int nb_parts, Array1d<int>& part) ;
    } ;

}

#endif
