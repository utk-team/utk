#include <cstdlib>
#include "sparse_matrix.h"

// Ctor
SparseMatrix::SparseMatrix(unsigned nrows, unsigned ncols) :
_row(nrows, SparseArray(ncols)) {
}

SparseMatrix::SparseMatrix(const SparseMatrix& rhs)
: _row(rhs._row) { 
}

// Dtor
SparseMatrix::~SparseMatrix() { 
}

// Access

unsigned SparseMatrix::numRows() const {
    return _row.size();
}

unsigned SparseMatrix::numColumns() const {
    return _row[0].size();
}

unsigned SparseMatrix::numNonZeros() const {
    unsigned N = 0;
    for (unsigned i=0; i<numRows(); ++i) {
        N += _row[i].numNonZeros();
    }
    return N;
}

unsigned SparseMatrix::copyCRS(long*& rowptr, long*& index, double*& value) const {
    unsigned N = numNonZeros();
	rowptr = (long int*) malloc((numRows() + 1) * sizeof(long int));
	index  = (long int*) malloc(N * sizeof(long int));
	value  = (double*) malloc(N * sizeof(double));
	
	rowptr[0] = 0;
	for (unsigned i = 0; i < numRows(); ++i) {
		int offset = _row[i].numNonZeros();
        _row[i].copy( &(index[rowptr[i]]), &(value[rowptr[i]]) );
		rowptr[i+1] = rowptr[i] + offset;
	}
	return N;
}    

unsigned SparseMatrix::copyCCS(long*& colptr, long*& index, double*& value) const {
	unsigned N = numNonZeros();
	colptr = (long int*) malloc((numColumns() + 1) * sizeof(long int));
	index  = (long int*) malloc(N * sizeof(long int));
	value  = (double*) malloc(N * sizeof(double));
	
	colptr[0] = 0;
	for (unsigned i = 0; i < numColumns(); ++i) {
        SparseArray col = getColumn(i);
		int offset = col.numNonZeros();
        col.copy( &(index[colptr[i]]), &(value[colptr[i]]) );
		colptr[i+1] = colptr[i] + offset;
	}
	return N;
}

double SparseMatrix::getValue(unsigned i, unsigned j) const { 
    return _row[i].getValue(j);
}

void SparseMatrix::setValue(unsigned i, unsigned j, const double value) { 
    _row[i].setValue(j, value);
}

SparseArray SparseMatrix::getRow(unsigned i) const {
    return _row[i];
}

SparseArray SparseMatrix::getColumn(unsigned i) const { 
    SparseArray column;
    for (unsigned j=0; j<numRows(); ++j) {
        int pos = _row[j].findIndex(i);
        if (pos != -1) column.insert(j, _row[j].readValue(pos));
    }
    return column;
}

SparseMatrix SparseMatrix::getTranspose() const {
    SparseMatrix transpose(numColumns(), numRows());
    for (unsigned i=0; i<numRows(); ++i) {
        unsigned nnz = _row[i].numNonZeros();
        for (unsigned j=0; j<nnz; ++j) {
            int  index = _row[i].readIndex(j);
            double value = _row[i].readValue(j);
            transpose._row[index].insert(i, value);
        }
    }
    return transpose;
}

void SparseMatrix::setRow(unsigned i, const SparseArray& row) {
    _row[i] = row;
}

void SparseMatrix::setColumn(unsigned i, const SparseArray& col) {
    unsigned nnz = col.numNonZeros();
    for (unsigned j=0; j<nnz; ++j) {
        int  index = col.readIndex(j);
        double value = col.readValue(j);
        _row[index].setValue(i, value);
    }
}

// Operators

SparseMatrix& SparseMatrix::operator = (const SparseMatrix& rhs) {
    _row = rhs._row;
    return *this;
}

SparseMatrix& SparseMatrix::operator += (const SparseMatrix& rhs) {
    for (unsigned i=0; i<numRows(); ++i) _row[i] += rhs._row[i];
    return *this;
}

SparseMatrix& SparseMatrix::operator -= (const SparseMatrix& rhs) {
    for (unsigned i=0; i<numRows(); ++i) _row[i] -= rhs._row[i];
    return *this;
}

SparseMatrix& SparseMatrix::operator *= (const SparseMatrix& rhs) {
    *this = multiply(rhs);
    return *this;
}

SparseMatrix& SparseMatrix::operator *= (const double rhs) {
    *this = multiply(rhs);
    return *this;
}    

SparseMatrix& SparseMatrix::operator /= (const double rhs) {
    for (unsigned i=0; i<numRows(); ++i) {
        _row[i] /= rhs;
    }
    return *this;
}    

// return (this * b)
SparseMatrix SparseMatrix::multiply(const SparseMatrix& b) const 
{
    SparseMatrix c(numRows(), b.numColumns());
    SparseMatrix bt = b.getTranspose();
    for (unsigned i = 0; i < c.numRows(); ++i) 
    {
        SparseArray c_i(c.numColumns());        
        SparseArray a_i = getRow(i);
        for (unsigned j = 0; j < c.numColumns(); ++j) 
        {
            double value = dot(a_i, bt.getRow(j));
            if (value > 1e-10) c_i.insert(j, value);
        }
        c.setRow(i, c_i);
    }
    return c;
}

SparseMatrix SparseMatrix::multiply(const double b) const 
{
    SparseMatrix c(numRows(), numColumns());
    for (unsigned i=0; i<numRows(); ++i)
        c.setRow(i, getRow(i) * b);
    return c;    
}
