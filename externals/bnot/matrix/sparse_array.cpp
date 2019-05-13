#include <cmath>
#include "sparse_array.h"

int SparseArray::findIndex(int index) const {
    for (unsigned i=0; i<numNonZeros(); ++i)
        if (_index[i] == index) return i;
    return -1;
}

int SparseArray::readIndex(unsigned i) const {
    return _index[i];
}

double SparseArray::readValue(unsigned i) const {
    return _value[i];
}

void SparseArray::insert(unsigned i, const double x) {
    _index.push_back(i);
    _value.push_back(x);
}

void SparseArray::replace(unsigned i, const double x) {
    _value[i] = x;
}

double SparseArray::getValue(unsigned i) const {
    int pos = findIndex(i);
    if (pos == -1) return 0.;
    return readValue(pos);
}

void SparseArray::setValue(unsigned i, const double x) {
    int pos = findIndex(i);
    if (pos == -1) insert(i, x);
    else _value[pos] = x;
}

SparseArray& SparseArray::operator = (const SparseArray& rhs) {
    _size  = rhs._size;
    _index = rhs._index;
    _value = rhs._value;
    return *this;
}

SparseArray& SparseArray::operator *= (const double rhs) {
    for (unsigned i=0; i<numNonZeros(); ++i)
        _value[i] *= rhs;
    return *this;
}

SparseArray& SparseArray::operator /= (const double rhs) {
    for (unsigned i=0; i<numNonZeros(); ++i)
        _value[i] /= rhs;
    return *this;
}

SparseArray& SparseArray::operator += (const SparseArray& rhs) {
    for (unsigned i=0; i<rhs.numNonZeros(); ++i) {
        int pos = findIndex( rhs.readIndex(i) );
        if (pos == -1) insert( rhs.readIndex(i), rhs.readValue(i) );
        else _value[pos] += rhs.readValue(i);
    }
    return *this;
}

SparseArray& SparseArray::operator -= (const SparseArray& rhs) {
    for (unsigned i=0; i<rhs.numNonZeros(); ++i) {
        int pos = findIndex( rhs.readIndex(i) );
        if (pos == -1) insert( rhs.readIndex(i), -rhs.readValue(i) );
        else _value[pos] -= rhs.readValue(i);
    }
    return *this;
}

double SparseArray::length2() const { 
    double sum = 0; 
    for (unsigned i=0; i<numNonZeros(); ++i) 
        sum += _value[i]*_value[i]; 
    return sum; 
}

double SparseArray::length() const  {
    return std::sqrt( length2() ); 
}

double SparseArray::normalize() { 
    double len = length(); 
    if ( len > 1e-10 )
        *this /= len;
    return len;
}

double SparseArray::getMax() const {
    if (numNonZeros() == 0) return 0.;
    double value = _value[0];
    for (unsigned i=1; i<numNonZeros(); ++i)
        value = std::max(value, _value[i]);
    return value;
}

double SparseArray::getMin() const {
    if (numNonZeros() == 0) return 0.;
    double value = _value[0];
    for (unsigned i=1; i<numNonZeros(); ++i)
        value = std::min(value, _value[i]);
    return value;
}
