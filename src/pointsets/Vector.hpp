#ifndef _UTK_VECTOR_HPP_
#define _UTK_VECTOR_HPP_

#include <iostream>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <cstring>

namespace utk
{

template <uint D, typename T>
class Vector{
private:
	
public:	
	
	T data[D];
	
	
	Vector(T f = 0) { for(uint i=0; i<D; i++) data[i] = f; }
	Vector(const T val[D]) { for(uint i=0; i<D; i++) data[i] = val[i]; }
	Vector(T i, T j, T k=0, T l=0) { data[0] = i; data[1] = j; if (D > 2) data[2] = k; if (D > 3) data[3] = l; for(uint i=4; i<D; i++) data[i] = 0; }
	Vector(const Vector<D, T>& arg_v) { memcpy(data, arg_v.data, D*sizeof(T)); }
	
	Vector<D,T> operator+(const Vector<D,T>& v) const { Vector<D,T> res; for(uint i=0; i<D; i++) res[i] = data[i]+v[i]; return res; }
	Vector<D,T>& operator+=(const Vector<D,T>& v) { for(uint i=0; i<D; i++) data[i]+=v[i]; return *this; }
	Vector<D,T> operator-(const Vector<D,T>& v) const { Vector<D,T> res; for(uint i=0; i<D; i++) res[i] = data[i]-v[i]; return res; }
	Vector<D,T>& operator-=(const Vector<D,T>& v) { for(uint i=0; i<D; i++) data[i]-=v[i]; return *this; }
	Vector<D,T> operator-() const { Vector<D,T> res; for(uint i=0; i<D; i++) res[i] = -data[i]; return res; }
	Vector<D,T> operator*(double f) const { Vector<D,T> res; for(uint i=0; i<D; i++) res[i] = f*data[i]; return res; }
	Vector<D,T>& operator*=(double f) { for(uint i=0; i<D; i++) data[i] *= f; return *this; }
	Vector<D,T> operator*(const Vector<D,T>& v) const { Vector<D,T> res; for(int i=0; i<D; i++) res[i] = v[i]*data[i]; return res; }
	Vector<D,T>& operator*=(const Vector<D,T>& v) {for(uint i=0; i<D; i++) data[i] *= v[i]; return *this; }
	Vector<D,T> operator/(double f) const { assert(f!=0); double inv = 1.f/f; return (*this)*inv; }
	Vector<D,T>& operator/=(double f) { assert(f!=0); double inv = 1.f/f; (*this)*=inv; return *this; }
	T operator[](uint i) const { assert(i>=0 && i<D); return data[i]; }
	T& operator[](uint i) { assert(i>=0 && i<D); return data[i]; }
	
	void setAll(T d) { for(uint i=0; i<D; i++) data[i] = d; }
	void set(uint i, T d) { assert(i>=0 && i<D); data[i] = d; }

	bool operator>(const Vector<D,T>& v) { for(uint i=0; i<D;i++) { if(v[i] >= data[i]) return false; } return true; }
	bool operator<(const Vector<D,T>& v) { for(uint i=0; i<D;i++) { if(v[i] <= data[i]) return false; } return true; }
	bool operator>=(const Vector<D,T>& v) { for(uint i=0; i<D;i++) { if(v[i] > data[i]) return false; } return true; }
	bool operator<=(const Vector<D,T>& v) { for(uint i=0; i<D;i++) { if(v[i] < data[i]) return false; } return true; }
	
	bool operator==(const Vector<D,T>& v) const { 
		for(uint i=0; i<D;i++) {
			if(v[i] != data[i]) 
				return false; 
		}
		return true; 
	}
	
	bool operator!=(const Vector<D,T>& v) { 
		for(uint i=0; i<D;i++) {
			if(v[i] != data[i]) 
				return true; 
		}
		return false; 
	}
	
	void operator=(const Vector<D, T>& arg_v) { memcpy(data, arg_v.data, D*sizeof(T)); }
	
	double lengthSquare() const { double res=0; for(uint i=0; i<D; i++) res += data[i]*data[i]; return res; }
	double length() const { return sqrt(lengthSquare()); }
	
	unsigned int nbCoord() { return D; }
	
	//void print() { std::cout << "("; for(int i=0; i<D-1; i++) std::cout << data[i] << ","; std::cout << data[D-1] << ")" << std::endl; }
	
	//T* tab() { return data; }
	//const T* tab() const { return data; }
};

template <unsigned int D, typename T>
std::ostream& operator<<(std::ostream& os, const utk::Vector<D, T>& obj)
{
	for(uint i=0; i<D-1; i++) os << obj[i] << "\t"; 
    os << obj[D-1];
	return os;
}

template <unsigned int D, typename T>
std::istream& operator>>(std::istream& is, utk::Vector<D, T>& obj)
{
	for(uint i=0; i<D; i++) {
		T d;
		is >> d;
		obj.set(i, d);
	}
	return is;
}

template <unsigned int D, typename T>
inline Vector<D,T> operator*(double f, const Vector<D,T>& v) { return v*f; }

template <unsigned int D, typename T>
inline double Dot(const Vector<D,T>& v1, const Vector<D,T>& v2) { double res = 0; for(uint i=0; i<D; i++) res += v1[i]*v2[i]; return res; }

template <unsigned int D, typename T>
inline double AbsDot(const Vector<D,T>& v1, const Vector<D,T>& v2) { return std::abs(Dot(v1, v2)); }

template <unsigned int D, typename T>																	
inline Vector<D, T> Normalize(const Vector<D, T>& v) { return v/(v.Length()); }

}

#endif
