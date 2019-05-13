#ifndef _SPARSE_MATRIX_H_
#define _SPARSE_MATRIX_H_ 1

/*
* class Sparsematrix
* any rectangular sparse matrix
* values stored by row (CRS)
* CRS = Compressed by Row Storage
*/

#include <vector>

#include "sparse_array.h"

class SparseMatrix {
private:
    std::vector<SparseArray> _row;
    
public:
    // Ctor
    SparseMatrix(unsigned nrows, unsigned ncols);

    SparseMatrix(const SparseMatrix& rhs);
    
    // Dtor
    ~SparseMatrix();
    
    // Access
    
    unsigned numRows() const;

    unsigned numColumns() const;
    
    unsigned numNonZeros() const;
    
    unsigned copyCRS(long*& rowptr, long*& index, double*& value) const;
    
    unsigned copyCCS(long*& colptr, long*& index, double*& value) const;

    double getValue(unsigned i, unsigned j) const;
    
    void setValue(unsigned i, unsigned j, const double value); 
    
    SparseArray getRow(unsigned i) const;
    
    SparseArray getColumn(unsigned i) const;
    
    SparseMatrix getTranspose() const;

    void setRow(unsigned i, const SparseArray& row);

    void setColumn(unsigned i, const SparseArray& col);

    // Operators
    
    SparseMatrix& operator = (const SparseMatrix& rhs);

    SparseMatrix& operator += (const SparseMatrix& rhs);

    SparseMatrix& operator -= (const SparseMatrix& rhs);
    
    SparseMatrix& operator *= (const SparseMatrix& rhs);

    SparseMatrix& operator *= (const double rhs);

    SparseMatrix& operator /= (const double rhs);
    
    SparseMatrix multiply(const double b) const;
    
    SparseMatrix multiply(const SparseMatrix& b) const;
};

// Friend Operators

inline
SparseMatrix operator - (const SparseMatrix& rhs) {
    SparseMatrix A(rhs.numRows(), rhs.numColumns());
    for (unsigned i=0; i<rhs.numRows(); ++i) {
        A.setRow(i, -rhs.getRow(i));
    }
    return A;
}

inline
SparseMatrix operator + (const SparseMatrix& a, const SparseMatrix& b) {
    SparseMatrix c = a;
    c += b;
    return c;
}

inline
SparseMatrix operator - (const SparseMatrix& a, const SparseMatrix& b) {
    SparseMatrix c = a;
    c -= b;
    return c;        
}

inline
SparseMatrix operator * (const SparseMatrix& a, const SparseMatrix& b) {
    return a.multiply(b);
}

inline
SparseMatrix operator * (const SparseMatrix& a, const double b) {
    return a.multiply(b);
}

inline
SparseMatrix operator * (const double a, const SparseMatrix& b) {
    return b.multiply(a);
}

inline
SparseMatrix operator / (const SparseMatrix& a, const double b) {
    SparseMatrix c(a.numRows(), a.numColumns());
    for (unsigned i=0; i<a.numRows(); ++i)
        c.setRow(i, a.getRow(i) / b);
    return c;        
}

// Concatenation

inline
SparseMatrix Concatenate(const SparseMatrix& A, const SparseMatrix& B) {
    SparseMatrix C(A.numRows() + B.numRows(), A.numColumns());
    
    // copy A
    for (unsigned i=0; i<A.numRows(); ++i) {
        C.setRow(i, A.getRow(i));
    }
    
    // copy B
    for (unsigned i=0; i<B.numRows(); ++i) {
        C.setRow(A.numRows() + i, B.getRow(i));
    }
    
    return C;        
}

#endif
