#ifndef _UTK_DOMAIN_HPP_
#define _UTK_DOMAIN_HPP_

#include "Point.hpp"
#include <vector>

namespace utk
{

/**
 * \class Domain
 * \brief A D-dimensional bounding box
 * \details This class is used as member of the utk::Pointset
 * class to represent the domain of definition of a point set.
*/
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