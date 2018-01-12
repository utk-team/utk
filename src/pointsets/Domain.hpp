#ifndef _UTK_DOMAIN_HPP_
#define _UTK_DOMAIN_HPP_

#include "Point.hpp"
#include <vector>

namespace utk
{

template <typename T>
struct Domain
{
//public:
	//Domain() {}
	T pMin;
	T pMax;
};

}

#endif