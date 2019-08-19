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
 * either expressed or implied, of the UTK project.
 */
#ifndef _UTK_HISTOGRAM_HPP_
#define _UTK_HISTOGRAM_HPP_

#include <vector>
#include <array>
#include <cmath>

#include "../io/messageStream.hpp"

namespace utk
{

template <uint D, typename T0, typename T1>
using HistogramValue = std::pair< std::array<T0, D>, T1 >;

/**
 * \class Histogram
 * \brief A D-dimensional histogram
 * \details This class is mostly used to represent
 * the PCF of a sampling pattern (1D histogram), its fourier spectrum (2D histogram),
 * or its spectral radial average (1D histogram).
*/
template <uint D, typename T0, typename T1>
class Histogram : public std::vector< HistogramValue<D, T0, T1> >
{
public:
	
	bool L2(const Histogram<D, T0, T1>& histo, double& L2) const
	{
		if(this->size() != histo.size())
		{
			ERROR("Histogram::L2 given histograms are of different sizes");
			return false;
		}
		L2 = 0;
		for(int i=0; i<this->size(); i++)
		{
			T1 value = 0;
			bool coherent = true;
			for(int d= 0; d<D; d++) coherent &= fabs((T0)this->at(i).first[d] - (T0)histo[i].first[d]) > 1e-9;
			if(coherent)
			{
				ERROR("Histogram::L2 Uncoherent values between histograms");
				return false;
			}
			value = histo[i].second;
			
			double diff = (this->at(i).second - value);
			L2 += diff*diff;
		}
		return true;
	}
	
	/*bool isLinfUnderThreshold(double threshold, const HistogramND<D, T0, T1>& histo, double& Linf) const
	{
		if(!(this->size() == histo.size()))
		{
			ERROR("HistogramND::Linf given histograms are of different sizes");
			return false;
		}
		Linf = 0;
		for(uint i=0; i<this->size(); i++)
		{
			T1 value = 0;
			bool coherent = true;
			for(uint d= 0; d<D; d++) coherent &= fabs((T0)this->at(i).first[d] - (T0)histo[i].first[d]) > 1e-9;
			if(coherent)
			{
				ERROR("HistogramND::Linf Uncoherent values between histograms");
				return false;
			}
			value = histo[i].second;
			
			double diff = fabs(this->at(i).second - value);// * (this->at(i).first - this->at(i-1).first);
			if(diff > Linf)
				Linf = diff;
			if(Linf > threshold)
				return false;
		}
		return true;
	}*/
	
	bool Linf(const Histogram<D, T0, T1>& histo, double& Linf) const
	{
		if(!(this->size() == histo.size()))
		{
			ERROR("Histogram::Linf given histograms are of different sizes");
			return false;
		}
		Linf = 0;
		for(uint i=0; i<this->size(); i++)
		{
			T1 value = 0;
			bool coherent = true;
			for(uint d= 0; d<D; d++) coherent &= fabs((T0)this->at(i).first[d] - (T0)histo[i].first[d]) > 1e-9;
			if(coherent)
			{
				ERROR("Histogram::Linf Uncoherent values between histograms");
				return false;
			}
			value = histo[i].second;
			
			double diff = fabs((T0)this->at(i).second - (T0)value);// * (this->at(i).first - this->at(i-1).first);
			if(diff > Linf)
				Linf = diff;
		}
		return true;
	}
	
	Histogram<D, T0, T1> operator-(const Histogram<D, T0, T1>& histo) const
	{
		Histogram<D, T0, T1> histo_diff;
		
		if(!this->size() == histo.size())
		{
			ERROR("Histogram::operator- given histograms are of different sizes");
			return histo_diff;
		}
		
		histo_diff.resize(this->size());
		for(int i=0; i<this->size(); i++)
		{
			histo_diff[i].first = this->at(i).first;
			T1 value = 0;
			bool coherent = true;
			for(int d= 0; d<D; d++) coherent &= fabs((T0)this->at(i).first[d] - (T0)histo[i].first[d]) > 1e-9;
			if(coherent)
			{
				ERROR("HistogramND::operator- Uncoherent values between histograms");
				histo_diff.clear();
				return histo_diff;
			}
			value = histo[i].second;
			
			double diff = fabs((T0)this->at(i).second - (T0)value);
			histo_diff[i].second = diff;
		}
		
		return histo_diff;
	}
};

typedef Histogram<1, double, double> Histogram1dd;
typedef Histogram<2, double, double> Histogram2dd;

	
} //end namespace utk

#endif
