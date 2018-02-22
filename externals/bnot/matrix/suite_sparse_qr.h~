#ifndef _SUITESPARSEQR_WRAPPER_H
#define _SUITESPARSEQR_WRAPPER_H 1

// STL
#include <vector>

// SuiteSparseQR header
#include <SuiteSparseQR.hpp>
#include <cholmod.h>

// local
#include "sparse_matrix.h"

// 
// Class SuiteSparseQRFactorizer
//
class SuiteSparseQRFactorizer {
private:
    cholmod_common _common;
    SuiteSparseQR_factorization<double>* _QR;
    
public:
    SuiteSparseQRFactorizer()  {
        cholmod_l_start(&_common);
        _QR = NULL;
    }
    
    ~SuiteSparseQRFactorizer() {
        if (_QR) SuiteSparseQR_free<double>(&_QR, &_common);
        cholmod_l_finish(&_common);
    }
    
    bool factorize(const SparseMatrix& A) {
        if (_QR) return false;
        cholmod_sparse* At = convert_transpose_to_cholmod_sparse(A);
        _QR = SuiteSparseQR_factorize<double>(SPQR_ORDERING_DEFAULT, SPQR_DEFAULT_TOL, At, &_common);
        cholmod_l_free_sparse(&At, &_common);
        return true;
    }
    
    bool solve(const std::vector<double>& rhs, std::vector<double>& x) {
        if (!_QR) return false;
        
        cholmod_dense* b = convert_to_cholmod_dense(rhs);
        cholmod_dense* y = SuiteSparseQR_solve(SPQR_RTX_EQUALS_ETB, _QR, b, &_common);
        cholmod_l_free_dense(&b, &_common);
        
        cholmod_dense* z = SuiteSparseQR_qmult(SPQR_QX, _QR, y, &_common);
        cholmod_l_free_dense(&y, &_common);
        
        convert_to_array(z, x);
        cholmod_l_free_dense(&z, &_common);
        return true;
    }
    
private:
    cholmod_sparse* convert_transpose_to_cholmod_sparse(const SparseMatrix& A) {
        // B = A'
        long* colptr;
        long* index;
        double* value;
        int nnz = (int) A.copyCRS(colptr, index, value);
        cholmod_sparse* B = cholmod_l_allocate_sparse(A.numColumns(), A.numRows(), nnz, 
                                                      1, 1, 0, CHOLMOD_REAL, &_common);
        B->p = (void*) colptr;
        B->i = (void*) index;
        B->x = (void*) value;
        return B;
    }
    
    cholmod_dense* convert_to_cholmod_dense(const std::vector<double>& a) {
        cholmod_dense* b = cholmod_l_allocate_dense(a.size(), 1, a.size(), CHOLMOD_REAL, &_common);
        double* tmp = (double*) b->x;
        for (unsigned i=0; i<a.size(); ++i) tmp[i] = a[i];
        return b;
    }

    void convert_to_array(cholmod_dense* a, std::vector<double>& b) {
        b.resize(a->nrow);
        double* tmp = (double*) a->x;
        for (unsigned i=0; i<a->nrow; ++i) b[i] = tmp[i];
    }
};

#endif
