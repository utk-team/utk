#ifndef _UTK_HISTOGRAMIO_HPP_
#define _UTK_HISTOGRAMIO_HPP_

#include "histogramIO_dat.hpp"

#include <iostream>
#include <cassert>

#include "../utils.hpp"

namespace utk
{

template <uint D, typename T0, typename T1>
class HistogramWriter
{
public:
	HistogramWriter() { m_writer = NULL; }
	~HistogramWriter() { if(m_writer!=NULL) delete m_writer; }

	bool open(const std::string arg_filename)
	{
		assert(!m_writer);
		std::string ext = getExt(arg_filename, ".dat");
		
		if(ext == ".dat" || ext == ".pcf")
			m_writer = new HistogramWriter_DAT<D, T0, T1>;
		else
		{
			ERROR("HistogramWriter::open unknown file format "+ext);
			return false;
		}
		
		return m_writer->open(arg_filename.c_str());
	}
	
	virtual bool writeHistogram(const HistogramND<D, T0, T1>& arg_histo) { return m_writer->writeHistogram(arg_histo); }
	
	void close()
	{
		assert(m_writer);
		m_writer->close();
		delete m_writer;
		m_writer=NULL;
	}
	
private:
	HistogramWriter_Model<D, T0, T1>* m_writer;
};

template <uint D, typename T0, typename T1>
class HistogramReader
{
public:
	HistogramReader() { m_reader = NULL; }
	~HistogramReader() { if(m_reader!=NULL) delete m_reader; }

	bool open(const std::string arg_filename)
	{
		assert(!m_reader);
		std::string ext = getExt(arg_filename, ".dat");
		
		if(ext == ".dat")
			m_reader = new HistogramReader_DAT<D, T0, T1>;
		else
		{
			ERROR("HistogramReader::open unknown file format "+ext);
			return false;
		}
		
		return m_reader->open(arg_filename.c_str());
	}
	
	virtual bool readHistogram(HistogramND<D, T0, T1>& arg_histo) { return m_reader->readHistogram(arg_histo); }
	
	void close()
	{
		assert(m_reader);
		m_reader->close();
		delete m_reader;
		m_reader=NULL;
	}
	
private:
	HistogramReader_Model<D, T0, T1>* m_reader;
};


}//end namespace utk

#endif