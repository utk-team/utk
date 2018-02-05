#ifndef _UTK_POINT_HPP_
#define _UTK_POINT_HPP_

#include <iostream>
#include <iomanip>

#include "Vector.hpp"

namespace utk
{

/**
 * \class Point
 * \brief A D-dimensional sample with coordinates of type T
 * \details This class includes functions and operator to 
 * facilitate the manipulation of points in a point set.
*/
template <unsigned int D, typename T>
class Point
{
public:
	Point() : m_pos() {}
	Point(const T& f) : m_pos(f) {}
	Point(const T val[D]) : m_pos(val) {}
	Point(const T& i, const T& j) : m_pos(i, j) {}
	Point(const T& i, const T& j, const T& k) : m_pos(i, j, k) {}
	Point(const T& i, const T& j, const T& k, const T& l) : m_pos(i, j, k ,l) {}
	Point(const Point<D, T>& arg_pt) : m_pos(arg_pt.pos()) {}
	
	void pos(const Vector<D, T>& arg_pos) { m_pos = arg_pos; }
	Vector<D, T>& pos() { return m_pos; }
	Vector<D, T> pos() const { return m_pos; }
	//T* pos() { return m_pos; }
	//Vector<D, T> pos() const { return m_pos; }
	
	bool operator==(const Point<D,T> arg_pt) const { return m_pos == arg_pt.pos(); }
	
protected:
	Vector<D, T> m_pos;
	//T m_pos[D];
};

template <unsigned int D, typename T>
std::ostream& operator<<(std::ostream& os, const utk::Point<D, T>& obj)
{
	os << std::setprecision(15) << std::fixed;
	os << obj.pos();
	return os;
}

template <unsigned int D, typename T>
std::istream& operator>>(std::istream& is, utk::Point<D, T>& obj)
{
	is >> obj.pos();
	return is;
}


}//end namespace

#endif