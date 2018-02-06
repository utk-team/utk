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


#include <Geex/numerics/matrix_partition.h>

// There is a problem with the definition of that function in METIS, probably needs to be fixed.
#define __log2 __xxlog2 
extern "C" {
#include <METIS/metis.h>
}


namespace Geex {

    // Convert Geex's sparse matrix into compressed row storage format.
    static void sparse_matrix_to_metis(const SparseMatrix* M, int& n, int*& xadj, int*& adjncy) {
        n = M->n() ;

        // Sanity checks
        gx_assert(M->rows_are_stored()) ;
        gx_assert(!M->has_symmetric_storage()) ;
        gx_assert(M->is_square()) ;
        for(int i=0; i<n; i++) {
            gx_assert(M->diag(i) == 0.0) ;
        }
        
        int nnz = M->nnz() ;
        xadj = new int[n+1] ;
        adjncy = new int[nnz] ;
        
        int count = 0 ;
        for(int i=0; i<n; i++) {
            const SparseMatrix::Row& r = M->row(i) ;
            xadj[i] = count ;
            for(unsigned int jj=0; jj<r.nb_coeffs(); jj++) {
                adjncy[count] = r.coeff(jj).index ; 
                count++ ;
            }
        }
        xadj[n] = nnz ;
    }


    MatrixPartition::~MatrixPartition() {
    }

    //---------------------------------------------------------------------------------------------------------

    void MatrixPartition_METIS::partition_matrix(const SparseMatrix* M, int nb_parts, Array1d<int>& part) {
        int n = 0 ;
        int* xadj = nil ;
        int* adjncy = nil ;
        sparse_matrix_to_metis(M, n, xadj, adjncy) ;
        part.allocate(n) ;

        int options[5] ;
        options[0] = 0 ; // use default values

        int zero = 0;
        int edgecut = 0;

        METIS_PartGraphRecursive(
            &n, xadj, adjncy,  // The matrix
            nil, nil,          // No vertex weight, no edge weight
            &zero,             // No weights
            &zero,             // C-style indexing
            &nb_parts,
            options,
            &edgecut,
            part.data()
        ) ;

        delete[] xadj ;
        delete[] adjncy ;
    }

}
