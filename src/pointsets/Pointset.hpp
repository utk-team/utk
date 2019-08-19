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
	
	void setUnitToroidalDomain()
	{
		for(int d=0; d<D; d++)
		{
			domain.pMin.pos()[d] = 0;
			domain.pMax.pos()[d] = 1;
		}
		toricity=1;
	}

	Domain<P> domain;
	int toricity;
	
protected:
};

} //end namespace

#endif
