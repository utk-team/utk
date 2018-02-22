#ifndef _SPARSE_ARRAY_H_
#define _SPARSE_ARRAY_H_ 1

#include <vector>
#include <iostream>
#include <string.h>

class SparseArray {
private:
    unsigned _size;
    std::vector<long int> _index;
    std::vector<double>   _value;
    
public:
    // Ctor
    SparseArray(unsigned N = 1) : _size(N)
    { }
    
    SparseArray(const SparseArray& rhs) 
    : _size(rhs._size), _index(rhs._index), _value(rhs._value) 
    { }
    
    // Dtor
    virtual ~SparseArray() 
    { }
    
    // Copy
    unsigned size() const
    { return _size; }
    
    unsigned numNonZeros() const
    { return _value.size(); }
    
    unsigned copy(long* index, double* value) const {
        memcpy(index, &(_index[0]), numNonZeros()*sizeof(long int));
        memcpy(value, &(_value[0]), numNonZeros()*sizeof(double));
        return numNonZeros();
    }
    
    // Access
    double getValue(unsigned i) const;
    
    void setValue(unsigned i, const double x);
    
    int findIndex(int index) const;
    
    int readIndex(unsigned i) const;

    double readValue(unsigned i) const;

    void insert(unsigned i, const double x);

    void replace(unsigned i, const double x);

    // Operators
    SparseArray& operator = (const SparseArray& rhs);
    
    SparseArray& operator *= (const double rhs);
    
    SparseArray& operator /= (const double rhs);
    
    SparseArray& operator += (const SparseArray& rhs);
    
    SparseArray& operator -= (const SparseArray& rhs);
    
    // Measures
    double normalize();
    
    double length2() const;
    
    double length() const;
    
    double getMax() const;
    
    double getMin() const;
    
    // Friend
    friend SparseArray operator - (const SparseArray& rhs);
    
    friend SparseArray operator + (const SparseArray& a, const SparseArray& b);
    
    friend SparseArray operator - (const SparseArray& a, const SparseArray& b);
    
    friend SparseArray operator * (const SparseArray& a, const double b);
    
    friend SparseArray operator * (const double a, const SparseArray& b);
    
    friend SparseArray operator / (const SparseArray& a, const double b);
    
    friend double dot(const SparseArray& a, const SparseArray& b);

    friend std::ostream& operator << (std::ostream& out, const SparseArray& a);
    
    friend std::istream& operator >> (std::istream& in, SparseArray& a);
    
    friend class SparseMatrix;
};
    
inline 
SparseArray operator - (const SparseArray& rhs) { 
    SparseArray neg(rhs);
    for (unsigned i=0; i<neg.numNonZeros(); ++i) 
        neg._value[i] = - neg._value[i];
    return neg;
}

inline 
SparseArray operator + (const SparseArray& a, const SparseArray& b) { 
    SparseArray rt(a); 
    rt += b; 
    return rt; 
}

inline 
SparseArray operator - (const SparseArray& a, const SparseArray& b) { 
    SparseArray rt(a); 
    rt -= b; 
    return rt;
}

inline 
SparseArray operator * (const SparseArray& a, const double b) { 
    SparseArray rt(a); 
    rt *= b; 
    return rt; 
}

inline 
SparseArray operator * (const double a, const SparseArray& b) { 
    SparseArray rt(b); 
    rt *= a; 
    return rt; 
}

inline 
SparseArray operator / (const SparseArray& a, double b) { 
    SparseArray rt(a); 
    rt /= b; 
    return rt; 
}

inline 
double dot(const SparseArray& a, const SparseArray& b) { 
    double sum = 0.; 
    for (unsigned i=0; i<a.numNonZeros(); ++i) {
        int pos = b.findIndex( a.readIndex(i) );
        if (pos != -1) sum += a.readValue(i) * b.readValue(pos);
    }
    return sum; 
}

inline 
std::ostream& operator << (std::ostream& out, const SparseArray& a) { 
    for (unsigned i=0; i<a.numNonZeros(); ++i) {
        out << a._index[i] << " "; 
        out << a._value[i] << " "; 
    }
    return out; 
}

inline 
std::istream& operator >> (std::istream& in, SparseArray& a) {
    for (unsigned i=0; i<a.numNonZeros(); ++i) {
        in >> a._index[i];
        in >> a._value[i];
    }
    return in;
}

#endif
