#ifndef _UTK_HISTOGRAMIO_DAT_HPP_
#define _UTK_HISTOGRAMIO_DAT_HPP_

#include "histogramIO_model.hpp"
#include <sstream>
#include <algorithm>

namespace utk
{

template <uint D, typename T0, typename T1>
class HistogramReader_DAT : public HistogramReader_Model<D, T0, T1>
{
public:
	HistogramReader_DAT() {}
	virtual ~HistogramReader_DAT() {}
	
	virtual bool readHistogram(HistogramND<D, T0, T1>& arg_histo) 
	{
		if(this->m_istream.eof())
			return false;
		
		unsigned int i=0;
		while(!this->m_istream.eof())
		{
			char line[1000];
			this->m_istream.getline(line, 1000);
			
			if(line[0] == '#' || line[0] == 0)
				break;
			
			if( i==0 && !arg_histo.empty())
			{
				WARNING("histogramIO_DAT::readHistogram the histogram to fill is not empty, clearing it ...");
				arg_histo.clear();
			}
			
			HistogramNDValue<D, T0, T1> ndval;
			std::stringstream ss;
			ss.str(line);
			for(uint d=0; d<D; d++)
				ss >> ndval.first[d];
			ss >> ndval.second;
			arg_histo.push_back(ndval);
			i++;
		}
		
		if(i==0)
			return false;

		return true;
	};
};

template <uint D, typename T0, typename T1>
bool sortHistoOnX(const HistogramNDValue<D, T0, T1>& v1, const HistogramNDValue<D, T0, T1>& v2)
{
	if(v1.first[0] == v2.first[0])
		return v1.first[1] < v2.first[1];
	
	return v1.first[0] < v2.first[0];
}

template <uint D, typename T0, typename T1>
class HistogramWriter_DAT : public HistogramWriter_Model<D, T0, T1>
{
public:
	HistogramWriter_DAT() { first=true; }
	virtual ~HistogramWriter_DAT() {}
	
	virtual bool writeHistogram(const HistogramND<D, T0, T1>& arg_histo) 
	{
		this->m_ostream.precision(10);
		
		/*if(D == 2)
		{
			HistogramND<D, T0, T1> sorted_histo = arg_histo;
			std::sort(sorted_histo.begin(), sorted_histo.end(), sortHistoOnX<D, T0, T1>);
			
			T0 last_x = sorted_histo[0].first[0];
			for(int i=0; i<sorted_histo.size(); i++)
			{
				if(sorted_histo[i].first[0] != last_x)
					this->m_ostream << std::endl;
				
				for(int d=0; d<D; d++)
					this->m_ostream << sorted_histo[i].first[d] << "\t";
				this->m_ostream << sorted_histo[i].second << std::endl;
				
				last_x = sorted_histo[i].first[0];
			}
		}
		else*/
		{
			for(uint i=0; i<arg_histo.size(); i++)
			{
				for(uint d=0; d<D; d++)
					this->m_ostream << arg_histo[i].first[d] << "\t";
				this->m_ostream << arg_histo[i].second << std::endl;
				
			}
		}
		
		return true;
	};
	
protected:
	bool first;
};

};// end namespace utk

#endif