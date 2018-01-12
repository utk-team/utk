#ifndef _UTK_HISTOGRAM_HPP_
#define _UTK_HISTOGRAM_HPP_

#include <vector>
#include <array>
#include <cmath>

#include "../io/messageStream.hpp"

namespace utk
{
	
template <uint D, typename T0, typename T1>
using HistogramNDValue = std::pair< std::array<T0, D>, T1 >;

template <uint D, typename T0, typename T1>
class HistogramND : public std::vector< HistogramNDValue<D, T0, T1> >
{
public:
	
	bool L2(const HistogramND<D, T0, T1>& histo, double& L2) const
	{
		if(!this->size() == histo.size())
		{
			ERROR("HistogramND::L2 given histograms are of different sizes");
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
				ERROR("HistogramND::L2 Uncoherent values between histograms");
				return false;
			}
			value = histo[i].second;
			
			double diff = (this->at(i).second - value);
			L2 += diff*diff;
		}
		return true;
	}
	
	bool isLinfUnderThreshold(double threshold, const HistogramND<D, T0, T1>& histo, double& Linf) const
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
	}
	
	bool Linf(const HistogramND<D, T0, T1>& histo, double& Linf) const
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
			
			double diff = fabs((T0)this->at(i).second - (T0)value);// * (this->at(i).first - this->at(i-1).first);
			if(diff > Linf)
				Linf = diff;
		}
		return true;
	}
	
	HistogramND<D, T0, T1> operator-(const HistogramND<D, T0, T1>& histo) const
	{
		HistogramND<D, T0, T1> histo_diff;
		
		if(!this->size() == histo.size())
		{
			ERROR("HistogramND::operator- given histograms are of different sizes");
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
	
	bool sort()
	{
		ERROR("Oops forgot to write that");
		return false;
	}
};

typedef HistogramND<1, double, double> Histogram1dd;
typedef HistogramND<2, double, double> Histogram2dd;

	
} //end namespace utk

#endif
