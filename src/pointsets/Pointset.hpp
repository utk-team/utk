#ifndef _UTK_POINTSET_HPP_
#define _UTK_POINTSET_HPP_

#include "Point.hpp"
#include "Domain.hpp"
#include "../io/messageStream.hpp"

#include <vector>
#include <typeinfo>
#include <bitset>
#include <array>

namespace utk
{

/**
 * \class Pointset
 * \brief A vector of D-dimensional samples with coordinates of type T
 * \details This class is used to represent
 * a point set. It is at the core of the UTK framework and is voluntarily kept
 * as simple as possible and heavily templated for the sake of code modularity.
*/
template <unsigned int D, typename T, typename P>
class Pointset : public std::vector< P >
{
public:
	Pointset() 
	{
		for(uint d=0; d<D; d++)
		{
			domain.pMin.pos()[d] = 0;
			domain.pMax.pos()[d] = 1;
		}
		toricity=-1;
	}

	double normalizePositions( Pointset<D, double, Point<D, double> >& pts2 ) const
	{
		if(domain.pMin.pos() > domain.pMax.pos())
		{
			ERROR("Poinset::normalizePositions Domain min point is superior to domain max point");
			return false;
		}

		if(domain.pMin == domain.pMax)
		{
			WARNING("Pointset::normalizePositions no domain is given, cannot normalize, return the original pointset");
			for(uint d=0; d<D; d++)
				pts2.domain.pMax.pos()[d] = 1;
			
			pts2.resize(this->size());
			
			for(uint i=0; i<this->size(); i++)
				for(uint d=0; d<D; d++)
					pts2[i].pos()[d] = this->at(i).pos()[d];
		}
		else
		{	
			for(uint d=0; d<D; d++)
				pts2.domain.pMax.pos()[d] = 1;
			
			pts2.resize(this->size());
			
			for(uint i=0; i<this->size(); i++)
				for(uint d=0; d<D; d++)
					pts2[i].pos()[d] = ((double)this->at(i).pos()[d] - (double)this->domain.pMin.pos()[d]) / ((double)this->domain.pMax.pos()[d] - (double)this->domain.pMin.pos()[d]);
		}
		return true;
	}

	Domain<P> domain;
	int toricity;
	
protected:
};

} //end namespace

#endif
