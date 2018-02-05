#ifndef _UTK_HISTOGRAMIO_MODEL_HPP_
#define _UTK_HISTOGRAMIO_MODEL_HPP_

#include <fstream>
#include <cassert>

#include "../pointsets/Histogram.hpp"

namespace utk
{

template <uint D, typename T0, typename T1>
class HistogramReader_Model
{
public:
	virtual ~HistogramReader_Model() {}
	
	virtual bool open(const char* arg_filename)
	{
		assert(!m_istream.is_open());
		m_istream.open(arg_filename);
		return m_istream.is_open();
	}
	
	virtual bool readHistogram(Histogram<D, T0, T1>& arg_histo) { return false; }
	
	virtual void close()
	{
		assert(m_istream.is_open());
		m_istream.close();
	}

protected:
	std::ifstream m_istream;
};


template <uint D, typename T0, typename T1>
class HistogramWriter_Model
{
public:
	virtual ~HistogramWriter_Model() {}
	
	virtual bool open(const char* arg_filename)
	{
		assert(!m_ostream.is_open());
		m_ostream.open(arg_filename);
		return m_ostream.is_open();
	}
	
	virtual bool writeHistogram(const Histogram<D, T0, T1>& arg_histo) { return false; }
	
	virtual void close()
	{
		assert(m_ostream.is_open());
		m_ostream.close();
	}

protected:
	std::ofstream m_ostream;
};

};//end namespace utk

#endif