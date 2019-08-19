/*
 * Hélène Perrier helene.perrier@liris.cnrs.fr
 * and David Coeurjolly david.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2018 CNRS Université de Lyon
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the Halton project.
 */
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
